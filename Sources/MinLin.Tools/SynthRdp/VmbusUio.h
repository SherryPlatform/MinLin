/*
 * PROJECT:    MinLin
 * FILE:       VmbusUio.h
 * PURPOSE:    Definition for VMBus Userspace I/O Backend
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef VMBUS_UIO
#define VMBUS_UIO

#include <Mile.Mobility.Portable.Types.h>

MO_DECLARE_HANDLE(VMBUS_UIO_HANDLE);

/**
 * @brief Releases device mappings before closing the descriptor, then
 *        releases the private context. Does not unbind the device or
 *        unregister its class.
 * @param Handle The UIO channel handle to close. It becomes invalid after
 *               this call, even if cleanup reports an error.
 * @return MO_TRUE on success; otherwise MO_FALSE with errno set to the first
 *         cleanup error. A null handle sets errno to EBADF.
*/
MO_EXTERN_C MO_BOOL VmbusUioClose(
    _Mo_In_ VMBUS_UIO_HANDLE Handle);

/**
 * @brief Opens a VMBus channel, automatically registering its class and
 *        binding the instance to the already loaded uio_hv_generic driver.
 *        Verifies the instance/class first and refuses another bound driver.
 *        Registration can affect other current and future unbound offers of
 *        the class; the caller must ensure affected channels are unused.
 *        Does not load drivers or wait for an absent instance to appear.
 * @param ClassId The device class GUID, passed by value.
 * @param InstanceId The device instance GUID, passed by value.
 * @param Flags The POSIX open flags, representable as a nonnegative int.
 *              Requires O_RDWR and rejects O_CREAT and O_TMPFILE.
 *              O_NONBLOCK and O_CLOEXEC are always added.
 * @param PipeMode Nonzero selects named-pipe payloads; zero selects raw
 *                 packet bodies with padding. The caller must select the
 *                 mode because sysfs does not expose the offer flag.
 * @return An exclusively owned opaque handle on success; otherwise NULL with
 *         errno set. Missing devices fail immediately; retry is the caller's
 *         responsibility. Completed registration/binding is not rolled back.
 *         Serialize handle operations; the handle is not a file descriptor.
*/
MO_EXTERN_C VMBUS_UIO_HANDLE MOAPI VmbusUioOpen(
    _Mo_In_ MO_GUID ClassId,
    _Mo_In_ MO_GUID InstanceId,
    _Mo_In_ MO_UINT32 Flags,
    _Mo_In_ MO_BOOL PipeMode);

/**
 * @brief Receives one complete packet immediately from the incoming ring.
 *        Pipe mode returns payload only; raw mode includes alignment padding.
 * @param Handle The valid, open UIO channel handle.
 * @param Buffer The writable destination buffer for the packet.
 * @param Size The destination capacity in bytes. Zero does not consume a
 *             packet.
 * @return The number of bytes copied on success; otherwise -1 with errno set.
 *         A consumed empty packet returns 0, which is not stream EOF.
 *         EAGAIN is retryable; ENOBUFS preserves the packet. Failure after
 *         publishing the read index is terminal and may follow consumption.
*/
MO_EXTERN_C MO_INTN VmbusUioRead(
    _Mo_In_ VMBUS_UIO_HANDLE Handle,
    _Mo_Out_ MO_POINTER Buffer,
    _Mo_In_ MO_UINTN Size);

/**
 * @brief Publishes one complete packet without waiting for ring space.
 *        Does not split or retry the payload.
 * @param Handle The valid, open UIO channel handle.
 * @param Buffer The read-only source payload buffer.
 * @param Size The payload size in bytes. Zero sends no packet. Pipe payloads
 *             are limited to 16384 bytes; raw payloads are limited by ring
 *             and descriptor capacity.
 * @return Size on success; otherwise -1 with errno set. EAGAIN leaves the
 *         packet unpublished; EMSGSIZE indicates an oversized payload.
 *         Failure after publication is terminal; never replay that payload.
*/
MO_EXTERN_C MO_INTN VmbusUioWrite(
    _Mo_In_ VMBUS_UIO_HANDLE Handle,
    _Mo_In_ MO_CONSTANT_POINTER Buffer,
    _Mo_In_ MO_UINTN Size);

#endif // !VMBUS_UIO
