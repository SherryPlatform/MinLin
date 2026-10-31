/*
 * PROJECT:    MinLin
 * FILE:       VmbusPipe.h
 * PURPOSE:    Definition for VMBus Kernel Pipe Backend
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef VMBUS_PIPE
#define VMBUS_PIPE

#include <Mile.Mobility.Portable.Types.h>
#include <sys/types.h>

/**
 * @brief Closes a real file descriptor using the normal Linux file lifetime
 *        rules. Does not change device bindings.
 * @param FileDescriptor The file descriptor to close.
 * @return If the function succeeds, it returns 0. Otherwise, it returns -1
 *         and sets errno.
*/
MO_EXTERN_C int MOAPI VmbusPipeClose(
    _Mo_In_ int FileDescriptor);

/**
 * @brief Opens a VMBus device through hv_vmbus_pipe without loading modules
 *        or changing bindings. The caller must ensure the channel is unused.
 * @param InstanceId The device instance GUID, passed by value.
 * @param ClassId The device class GUID, passed by value.
 * @param Flags The flags passed to open. O_CREAT and O_TMPFILE are rejected.
 * @param PipeMode A compatibility hint. The kernel determines the mode from
 *                 the actual offer flag; this parameter neither overrides
 *                 nor validates that mode.
 * @return If the function succeeds, it returns a real file descriptor.
 *         Otherwise, it returns -1 and sets errno. Missing nodes fail without
 *         waiting or retrying; the caller determines the retry policy.
*/
MO_EXTERN_C int MOAPI VmbusPipeOpen(
    _Mo_In_ MO_GUID InstanceId,
    _Mo_In_ MO_GUID ClassId,
    _Mo_In_ int Flags,
    _Mo_In_ MO_BOOL PipeMode);

/**
 * @brief Reads one complete packet immediately through the kernel interface.
 *        Named-pipe mode returns payload only; raw mode includes padding.
 * @param FileDescriptor The open VMBus device file descriptor.
 * @param Buffer The writable destination buffer for the packet.
 * @param Size The destination capacity in bytes. Zero does not consume a
 *             packet.
 * @return If the function succeeds, it returns the number of bytes copied.
 *         A consumed empty packet also returns 0, which is not stream EOF.
 *         Otherwise, it returns -1 and sets errno. EAGAIN indicates temporary
 *         unavailability; ENOBUFS preserves the packet for a larger buffer.
*/
MO_EXTERN_C ssize_t MOAPI VmbusPipeRead(
    _Mo_In_ int FileDescriptor,
    _Mo_Out_ MO_POINTER Buffer,
    _Mo_In_ size_t Size);

/**
 * @brief Writes one complete packet immediately through the kernel interface.
 *        Does not split the payload or wait for ring space.
 * @param FileDescriptor The open VMBus device file descriptor.
 * @param Buffer The read-only source payload buffer.
 * @param Size The payload size in bytes. Zero sends no packet. Named-pipe
 *             payloads are limited to 16384 bytes; raw payloads are limited
 *             by the kernel ring capacity.
 * @return If the function succeeds, it returns Size. Otherwise, it returns -1
 *         and sets errno. EAGAIN indicates insufficient ring space;
 *         EMSGSIZE indicates an oversized payload.
*/
MO_EXTERN_C ssize_t MOAPI VmbusPipeWrite(
    _Mo_In_ int FileDescriptor,
    _Mo_In_ MO_CONSTANT_POINTER Buffer,
    _Mo_In_ size_t Size);

#endif // !VMBUS_PIPE
