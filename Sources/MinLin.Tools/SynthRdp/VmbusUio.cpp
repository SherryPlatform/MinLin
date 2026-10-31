/*
 * PROJECT:    MinLin
 * FILE:       VmbusUio.cpp
 * PURPOSE:    Implementation for VMBus Userspace I/O Backend
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "VmbusUio.h"

#include <Mile.Mobility.Utilities.MemoryAccess.h>
#include <Mile.HyperV.VMBus.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct _VMBUS_UIO_CONTEXT
{
    int FileDescriptor;
    MO_POINTER Mappings[3];
    size_t MappingSizes[3];
    MO_UINT32 DataSize;
    PVMRCB OutgoingControl;
    PMO_UINT8 OutgoingData;
    PVMRCB IncomingControl;
    PMO_UINT8 IncomingData;
    bool NamedPipe;
    bool UsesMonitor;
    MO_UINT32 ChannelId;
    MO_UINT32 MonitorId;
    int TerminalError;
} VMBUS_UIO_CONTEXT, *PVMBUS_UIO_CONTEXT;

MO_EXTERN_C MO_BOOL MOAPI VmbusUioClose(
    _Mo_In_ VMBUS_UIO_HANDLE Handle)
{
    if (!Handle)
    {
        errno = EBADF;
        return MO_FALSE;
    }
    PVMBUS_UIO_CONTEXT Device = reinterpret_cast<PVMBUS_UIO_CONTEXT>(Handle);
    int ErrorCode = 0;
    // Mappings hold file references; remove them before the final close.
    for (size_t Index = MO_ARRAY_SIZE(Device->Mappings); Index; --Index)
    {
        if (Device->MappingSizes[Index - 1] &&
            -1 == ::munmap(
                Device->Mappings[Index - 1],
                Device->MappingSizes[Index - 1]))
        {
            if (!ErrorCode)
            {
                ErrorCode = errno;
            }
        }
    }
    if (-1 != Device->FileDescriptor &&
        -1 == ::close(Device->FileDescriptor))
    {
        if (!ErrorCode)
        {
            ErrorCode = errno;
        }
    }
    if (-1 == ::munmap(Device, sizeof(*Device)) && !ErrorCode)
    {
        ErrorCode = errno;
    }
    if (ErrorCode)
    {
        errno = ErrorCode;
        return MO_FALSE;
    }
    return MO_TRUE;
}

/**
 * @brief Formats a GUID for Linux device paths and sysfs attributes.
 * @param Identifier The GUID value to format.
 * @param Buffer Receives lowercase canonical text without braces. The buffer
 *               must provide at least 37 bytes, including the null terminator.
 * @return This function does not return a value.
*/
static void VmbusUioFormatGuid(
    _Mo_In_ MO_GUID Identifier,
    _Mo_Out_ MO_STRING Buffer)
{
    ::snprintf(Buffer, 37,
        "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        static_cast<unsigned int>(Identifier.Data1),
        static_cast<unsigned int>(Identifier.Data2),
        static_cast<unsigned int>(Identifier.Data3),
        static_cast<unsigned int>(Identifier.Data4[0]),
        static_cast<unsigned int>(Identifier.Data4[1]),
        static_cast<unsigned int>(Identifier.Data4[2]),
        static_cast<unsigned int>(Identifier.Data4[3]),
        static_cast<unsigned int>(Identifier.Data4[4]),
        static_cast<unsigned int>(Identifier.Data4[5]),
        static_cast<unsigned int>(Identifier.Data4[6]),
        static_cast<unsigned int>(Identifier.Data4[7]));
}

/**
 * @brief Parses lowercase canonical GUID text from Linux metadata.
 * @param Text The null-terminated GUID text without braces.
 * @param Identifier Receives the GUID value on success; unchanged on failure.
 * @return If the function succeeds, it returns 0. Otherwise, it returns -1
 *         and sets errno to EINVAL.
*/
static int VmbusUioParseGuid(
    _Mo_In_ MO_CONSTANT_STRING Text,
    _Mo_Out_ PMO_GUID Identifier)
{
    if (!Text || !Identifier || 36 != ::strlen(Text))
    {
        errno = EINVAL;
        return -1;
    }
    MO_UINT8 Bytes[16] = {};
    size_t Position = 0;
    for (size_t Index = 0; Index < 36; ++Index)
    {
        char Character = Text[Index];
        if (8 == Index || 13 == Index || 18 == Index || 23 == Index)
        {
            if ('-' != Character)
            {
                errno = EINVAL;
                return -1;
            }
            continue;
        }
        MO_UINT8 Digit = 0;
        if (Character >= '0' && Character <= '9')
        {
            Digit = static_cast<MO_UINT8>(Character - '0');
        }
        else if (Character >= 'a' && Character <= 'f')
        {
            Digit = static_cast<MO_UINT8>(Character - 'a' + 10);
        }
        else
        {
            errno = EINVAL;
            return -1;
        }
        Bytes[Position / 2] = static_cast<MO_UINT8>(
            (Bytes[Position / 2] << 4) | Digit);
        ++Position;
    }
    MO_GUID Result = {};
    Result.Data1 =
        (static_cast<MO_UINT32>(Bytes[0]) << 24) |
        (static_cast<MO_UINT32>(Bytes[1]) << 16) |
        (static_cast<MO_UINT32>(Bytes[2]) << 8) |
        Bytes[3];
    Result.Data2 = static_cast<MO_UINT16>((Bytes[4] << 8) | Bytes[5]);
    Result.Data3 = static_cast<MO_UINT16>((Bytes[6] << 8) | Bytes[7]);
    ::memcpy(Result.Data4, Bytes + 8, sizeof(Result.Data4));
    *Identifier = Result;
    return 0;
}

/**
 * @brief Reads a text attribute, retries interrupted reads, and removes
 *        trailing spaces, tabs, carriage returns, and newlines.
 * @param Path The null-terminated path of the attribute to read.
 * @param Buffer Receives the null-terminated text on success.
 * @param BufferSize The nonzero capacity of Buffer in bytes, including the
 *                   null terminator. The untrimmed input must fit.
 * @return If the function succeeds, it returns 0. Otherwise, it returns -1
 *         and sets errno. EOVERFLOW indicates that the input does not fit.
*/
static int VmbusUioReadText(
    _Mo_In_ MO_CONSTANT_STRING Path,
    _Mo_Out_ MO_STRING Buffer,
    _Mo_In_ size_t BufferSize)
{
    int FileDescriptor = ::open(Path, O_RDONLY | O_CLOEXEC);
    if (-1 == FileDescriptor)
    {
        return -1;
    }
    int ErrorCode = 0;
    size_t Length = 0;
    for (;;)
    {
        // Probe EOF even when the buffer is exactly full.
        char Extra = '\0';
        bool Full = Length == BufferSize - 1;
        ssize_t Result = ::read(
            FileDescriptor,
            Full ? &Extra : Buffer + Length,
            Full ? 1 : BufferSize - 1 - Length);
        if (Result > 0)
        {
            if (Full)
            {
                ErrorCode = EOVERFLOW;
                break;
            }
            Length += static_cast<size_t>(Result);
            continue;
        }
        if (-1 == Result)
        {
            if (EINTR == errno)
            {
                continue;
            }
            ErrorCode = errno;
        }
        break;
    }
    ::close(FileDescriptor);
    if (ErrorCode)
    {
        errno = ErrorCode;
        return -1;
    }
    while (Length && (
        '\n' == Buffer[Length - 1] ||
        '\r' == Buffer[Length - 1] ||
        ' ' == Buffer[Length - 1] ||
        '\t' == Buffer[Length - 1]))
    {
        --Length;
    }
    Buffer[Length] = '\0';
    return 0;
}

/**
 * @brief Checks a device instance and its class against sysfs without opening
 *        the channel or changing its driver binding.
 * @param InstanceId The device instance GUID.
 * @param ClassId The expected device class GUID.
 * @return If the instance has the specified class, the function returns 0.
 *         Otherwise, it returns -1 and sets errno. ENODEV indicates an
 *         unexpected class attribute; other errors retain their cause.
*/
static int VmbusUioCheckDevice(
    _Mo_In_ MO_GUID InstanceId,
    _Mo_In_ MO_GUID ClassId)
{
    char Instance[37] = {};
    ::VmbusUioFormatGuid(InstanceId, Instance);
    char Attribute[128] = {};
    ::snprintf(
        Attribute,
        sizeof(Attribute),
        "/sys/bus/vmbus/devices/%s/class_id",
        Instance);
    char Text[64] = {};
    if (-1 == ::VmbusUioReadText(Attribute, Text, sizeof(Text)))
    {
        return -1;
    }
    if (38 != ::strlen(Text) || '{' != Text[0] || '}' != Text[37])
    {
        errno = ENODEV;
        return -1;
    }
    Text[37] = '\0';
    MO_GUID Class = {};
    if (-1 == ::VmbusUioParseGuid(Text + 1, &Class) ||
        0 != ::memcmp(&Class, &ClassId, sizeof(Class)))
    {
        errno = ENODEV;
        return -1;
    }
    return 0;
}

/**
 * @brief Writes one sysfs command, retrying interrupted writes but not
 *        partially written commands.
 * @param Path The null-terminated path of the writable attribute.
 * @param Value The null-terminated command text. The terminator is not written.
 * @return If all command bytes are written, the function returns 0.
 *         Otherwise, it returns -1 and sets errno. A short write sets EIO.
*/
static int VmbusUioWriteAttribute(
    _Mo_In_ MO_CONSTANT_STRING Path,
    _Mo_In_ MO_CONSTANT_STRING Value)
{
    int FileDescriptor = ::open(Path, O_WRONLY | O_CLOEXEC);
    if (-1 == FileDescriptor)
    {
        return -1;
    }
    size_t Length = ::strlen(Value);
    ssize_t Result = 0;
    do
    {
        Result = ::write(FileDescriptor, Value, Length);
    } while (-1 == Result && EINTR == errno);
    int ErrorCode = (-1 == Result)
        ? errno
        : (Length == static_cast<size_t>(Result) ? 0 : EIO);
    // Sysfs attributes are single commands; never retry a positive short write.
    ::close(FileDescriptor);
    if (ErrorCode)
    {
        errno = ErrorCode;
        return -1;
    }
    return 0;
}

/**
 * @brief Registers a class with the already loaded uio_hv_generic driver.
 *        This can affect current and future unbound offers of the class.
 * @param ClassId The class GUID to register.
 * @return 0 on success or an existing registration; otherwise -1 with errno
 *         set. Success does not establish that every offer was bound.
*/
static int VmbusUioRegisterClass(
    _Mo_In_ MO_GUID ClassId)
{
    char Identifier[37] = {};
    ::VmbusUioFormatGuid(ClassId, Identifier);
    if (-1 == ::VmbusUioWriteAttribute(
        "/sys/bus/vmbus/drivers/uio_hv_generic/new_id",
        Identifier) &&
        EEXIST != errno)
    {
        return -1;
    }
    return 0;
}

/**
 * @brief Inspects the driver link of a previously verified VMBus instance.
 * @param InstanceId The device instance GUID.
 * @return If bound to uio_hv_generic, the function returns 1. If the driver
 *         link is absent, it returns 0. Otherwise, it returns -1 and sets
 *         errno. EBUSY indicates another bound driver.
*/
static int VmbusUioDriverState(
    _Mo_In_ MO_GUID InstanceId)
{
    char Instance[37] = {};
    ::VmbusUioFormatGuid(InstanceId, Instance);
    char Attribute[128] = {};
    ::snprintf(
        Attribute,
        sizeof(Attribute),
        "/sys/bus/vmbus/devices/%s/driver",
        Instance);
    char Link[256] = {};
    ssize_t Length = ::readlink(Attribute, Link, sizeof(Link) - 1);
    if (-1 == Length)
    {
        return ENOENT == errno ? 0 : -1;
    }
    if (static_cast<size_t>(Length) == sizeof(Link) - 1)
    {
        errno = EOVERFLOW;
        return -1;
    }
    Link[Length] = '\0';
    MO_CONSTANT_STRING Name = ::strrchr(Link, '/');
    Name = Name ? Name + 1 : Link;
    if (0 != ::strcmp(Name, "uio_hv_generic"))
    {
        errno = EBUSY;
        return -1;
    }
    return 1;
}

/**
 * @brief Verifies the instance/class and ensures its UIO binding without
 *        unbinding another driver. The class must already be registered.
 * @param InstanceId The instance GUID to bind.
 * @param ClassId The expected class GUID.
 * @return 0 after verifying UIO ownership; otherwise -1 with errno set.
 *         EBUSY indicates another driver; ENODEV indicates no final binding.
*/
static int VmbusUioBind(
    _Mo_In_ MO_GUID InstanceId,
    _Mo_In_ MO_GUID ClassId)
{
    if (-1 == ::VmbusUioCheckDevice(InstanceId, ClassId))
    {
        return -1;
    }
    int State = ::VmbusUioDriverState(InstanceId);
    if (0 != State)
    {
        return State > 0 ? 0 : -1;
    }
    char Instance[37] = {};
    ::VmbusUioFormatGuid(InstanceId, Instance);
    int Result = ::VmbusUioWriteAttribute(
        "/sys/bus/vmbus/drivers/uio_hv_generic/bind",
        Instance);
    int ErrorCode = -1 == Result ? errno : 0;
    State = ::VmbusUioDriverState(InstanceId);
    if (1 == State)
    {
        return 0;
    }
    if (0 == State)
    {
        errno = ErrorCode ? ErrorCode : ENODEV;
    }
    return -1;
}

/**
 * @brief Reads an unsigned integer attribute using base autodetection.
 * @param Path The null-terminated path of the numeric attribute.
 * @param Value Receives the parsed integer on success.
 * @return If the function succeeds, it returns 0. Otherwise, it returns -1
 *         and sets errno. Malformed or out-of-range values set EPROTO.
*/
static int VmbusUioReadNumber(
    _Mo_In_ MO_CONSTANT_STRING Path,
    _Mo_Out_ unsigned long long* Value)
{
    char Buffer[64] = {};
    if (-1 == ::VmbusUioReadText(Path, Buffer, sizeof(Buffer)))
    {
        return -1;
    }
    MO_STRING End = nullptr;
    errno = 0;
    unsigned long long Number = ::strtoull(Buffer, &End, 0);
    if (0 != errno || Buffer[0] < '0' || Buffer[0] > '9' || '\0' != *End)
    {
        errno = EPROTO;
        return -1;
    }
    *Value = Number;
    return 0;
}

MO_EXTERN_C VMBUS_UIO_HANDLE MOAPI VmbusUioOpen(
    _Mo_In_ MO_GUID ClassId,
    _Mo_In_ MO_GUID InstanceId,
    _Mo_In_ MO_UINT32 Flags,
    _Mo_In_ MO_BOOL PipeMode)
{
    if (Flags > static_cast<MO_UINT32>(INT_MAX) ||
        O_RDWR != (Flags & O_ACCMODE) ||
        (Flags & O_CREAT) ||
        O_TMPFILE == (Flags & O_TMPFILE))
    {
        errno = EINVAL;
        return nullptr;
    }
    // Fail for missing/mismatched instances or another driver before changing
    // any registration. Do not wait for an offer or try another backend.
    if (-1 == ::VmbusUioCheckDevice(InstanceId, ClassId) ||
        -1 == ::VmbusUioDriverState(InstanceId))
    {
        return nullptr;
    }
    if (-1 == ::VmbusUioRegisterClass(ClassId) ||
        -1 == ::VmbusUioBind(InstanceId, ClassId))
    {
        return nullptr;
    }
    // One private mapping per handle, with no process-wide channel limit and
    // no manual malloc/new. The mapping is released even if opening fails.
    MO_POINTER Storage = ::mmap(
        nullptr,
        sizeof(VMBUS_UIO_CONTEXT),
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0);
    if (MAP_FAILED == Storage)
    {
        return nullptr;
    }
    if (!Storage)
    {
        ::munmap(Storage, sizeof(VMBUS_UIO_CONTEXT));
        errno = ENOMEM;
        return nullptr;
    }
    VMBUS_UIO_CONTEXT& Device = *static_cast<PVMBUS_UIO_CONTEXT>(Storage);
    Device = {};
    Device.FileDescriptor = -1;
    Device.NamedPipe = !!PipeMode;
    VMBUS_UIO_HANDLE Handle = reinterpret_cast<VMBUS_UIO_HANDLE>(Storage);
    int ErrorCode = 0;
    do
    {
        char Instance[37] = {};
        ::VmbusUioFormatGuid(InstanceId, Instance);
        char RootPath[128] = {};
        ::snprintf(
            RootPath,
            sizeof(RootPath),
            "/sys/bus/vmbus/devices/%s",
            Instance);
        char AttributePath[256] = {};
        char Text[64] = {};
        char DeviceName[16] = {};
        {
            ::snprintf(
                AttributePath,
                sizeof(AttributePath),
                "%s/uio",
                RootPath);
            DIR* Directory = ::opendir(AttributePath);
            if (!Directory)
            {
                ErrorCode = errno;
                break;
            }
            for (;;)
            {
                errno = 0;
                dirent* Entry = ::readdir(Directory);
                if (!Entry)
                {
                    ErrorCode = errno ? errno : ENOENT;
                    break;
                }
                size_t NameLength = ::strlen(Entry->d_name);
                if (NameLength < 4 ||
                    NameLength > 10 ||
                    0 != ::strncmp(Entry->d_name, "uio", 3))
                {
                    continue;
                }
                bool ValidName = true;
                for (size_t Index = 3; Index < NameLength; ++Index)
                {
                    if (Entry->d_name[Index] < '0' ||
                        Entry->d_name[Index] > '9')
                    {
                        ValidName = false;
                        break;
                    }
                }
                if (ValidName)
                {
                    ::memcpy(DeviceName, Entry->d_name, NameLength + 1);
                    break;
                }
            }
            ::closedir(Directory);
        }
        if (ErrorCode)
        {
            break;
        }
        char UioPath[160] = {};
        ::snprintf(UioPath, sizeof(UioPath), "%s/uio/%s", RootPath, DeviceName);
        ::snprintf(AttributePath, sizeof(AttributePath), "%s/name", UioPath);
        if (-1 == ::VmbusUioReadText(AttributePath, Text, sizeof(Text)))
        {
            ErrorCode = errno;
            break;
        }
        if (0 != ::strcmp(Text, "uio_hv_generic"))
        {
            ErrorCode = ENODEV;
            break;
        }
        unsigned long long Number = 0;
        ::snprintf(
            AttributePath,
            sizeof(AttributePath),
            "%s/monitor_id",
            RootPath);
        if (0 == ::VmbusUioReadNumber(AttributePath, &Number))
        {
            if (Number >= 128)
            {
                ErrorCode = EPROTO;
                break;
            }
            Device.UsesMonitor = true;
            Device.MonitorId = static_cast<MO_UINT32>(Number);
            ::snprintf(
                AttributePath,
                sizeof(AttributePath),
                "%s/id",
                RootPath);
            if (-1 == ::VmbusUioReadNumber(AttributePath, &Number))
            {
                ErrorCode = errno;
                break;
            }
            if (Number >= HV_EVENT_FLAGS_COUNT)
            {
                ErrorCode = EPROTO;
                break;
            }
            Device.ChannelId = static_cast<MO_UINT32>(Number);
        }
        else if (ENOENT != errno)
        {
            ErrorCode = errno;
            break;
        }
        long PageSize = ::sysconf(_SC_PAGESIZE);
        if (PageSize <= 0)
        {
            ErrorCode = EINVAL;
            break;
        }
        size_t MappingSizes[3] = {};
        MO_CONSTANT_STRING MappingNames[3] = {};
        MappingNames[0] = "txrx_rings";
        MappingNames[1] = "int_page";
        MappingNames[2] = "monitor_page";
        size_t MappingCount = Device.UsesMonitor ? 3 : 1;
        for (size_t Index = 0; Index < MappingCount; ++Index)
        {
            ::snprintf(
                AttributePath,
                sizeof(AttributePath),
                "%s/maps/map%zu/name",
                UioPath,
                Index);
            if (-1 == ::VmbusUioReadText(AttributePath, Text, sizeof(Text)))
            {
                ErrorCode = errno;
                break;
            }
            if (0 != ::strcmp(Text, MappingNames[Index]))
            {
                ErrorCode = EPROTO;
                break;
            }
            ::snprintf(
                AttributePath,
                sizeof(AttributePath),
                "%s/maps/map%zu/offset",
                UioPath,
                Index);
            if (-1 == ::VmbusUioReadNumber(AttributePath, &Number))
            {
                ErrorCode = errno;
                break;
            }
            if (0 != Number)
            {
                ErrorCode = EPROTO;
                break;
            }
            ::snprintf(
                AttributePath,
                sizeof(AttributePath),
                "%s/maps/map%zu/size",
                UioPath,
                Index);
            if (-1 == ::VmbusUioReadNumber(AttributePath, &Number))
            {
                ErrorCode = errno;
                break;
            }
            if (Number > SIZE_MAX || Number < HV_PAGE_SIZE)
            {
                ErrorCode = EPROTO;
                break;
            }
            MappingSizes[Index] = static_cast<size_t>(Number);
        }
        if (ErrorCode)
        {
            break;
        }
        size_t RingSize = MappingSizes[0] / 2;
        size_t MinimumDataSize =
            sizeof(VMPACKET_DESCRIPTOR) +
            sizeof(PREVIOUS_PACKET_OFFSET) +
            (Device.NamedPipe
                ? sizeof(VMPIPE_PROTOCOL_HEADER) +
                    VMPIPE_MAXIMUM_PIPE_PACKET_SIZE
                : 0);
        if (MappingSizes[0] % 2 ||
            RingSize % static_cast<size_t>(PageSize) ||
            RingSize <= static_cast<size_t>(PageSize) + MinimumDataSize ||
            RingSize - static_cast<size_t>(PageSize) > MO_UINT32_MAX)
        {
            ErrorCode = EPROTO;
            break;
        }
        Device.DataSize = static_cast<MO_UINT32>(
            RingSize - static_cast<size_t>(PageSize));
        char DevicePath[32] = {};
        ::snprintf(DevicePath, sizeof(DevicePath), "/dev/%s", DeviceName);
        Device.FileDescriptor = ::open(
            DevicePath,
            static_cast<int>(Flags) | O_NONBLOCK | O_CLOEXEC);
        if (-1 == Device.FileDescriptor)
        {
            ErrorCode = errno;
            break;
        }
        for (size_t Index = 0; Index < MappingCount; ++Index)
        {
            MO_POINTER Mapping = ::mmap(
                nullptr,
                MappingSizes[Index],
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                Device.FileDescriptor,
                static_cast<off_t>(Index) * PageSize);
            if (MAP_FAILED == Mapping)
            {
                ErrorCode = errno;
                break;
            }
            Device.Mappings[Index] = Mapping;
            Device.MappingSizes[Index] = MappingSizes[Index];
            if (!Mapping)
            {
                ErrorCode = ENOMEM;
                break;
            }
        }
        if (ErrorCode)
        {
            break;
        }
        PMO_UINT8 Ring = static_cast<PMO_UINT8>(Device.Mappings[0]);
        Device.OutgoingControl = reinterpret_cast<PVMRCB>(Ring);
        Device.OutgoingData = Ring + PageSize;
        Device.IncomingControl = reinterpret_cast<PVMRCB>(Ring + RingSize);
        Device.IncomingData = Ring + RingSize + PageSize;
        volatile VMRCB& IncomingControl = *Device.IncomingControl;
        IncomingControl.InterruptMask = 0;
        ::MoMileMemoryBarrier();
        return Handle;
    } while (false);
    ::VmbusUioClose(Handle);
    errno = ErrorCode;
    return nullptr;
}

/**
 * @brief Advances a ring offset, wrapping at the end of the data area.
 * @param Offset The starting offset, which must be less than Capacity.
 * @param Count The number of bytes to advance, at most Capacity.
 * @param Capacity The nonzero ring data capacity in bytes.
 * @return The wrapped offset, which is less than Capacity.
*/
static MO_UINT32 VmbusUioAdvance(
    _Mo_In_ MO_UINT32 Offset,
    _Mo_In_ MO_UINT32 Count,
    _Mo_In_ MO_UINT32 Capacity)
{
    MO_UINT32 Tail = Capacity - Offset;
    return Count >= Tail ? Count - Tail : Offset + Count;
}

/**
 * @brief Calculates readable ring bytes from validated producer and consumer
 *        offsets. Equal offsets represent an empty ring.
 * @param In The producer offset, which must be less than Capacity.
 * @param Out The consumer offset, which must be less than Capacity.
 * @param Capacity The nonzero ring data capacity in bytes.
 * @return The number of bytes between Out and In, accounting for wraparound.
*/
static MO_UINT32 VmbusUioAvailable(
    _Mo_In_ MO_UINT32 In,
    _Mo_In_ MO_UINT32 Out,
    _Mo_In_ MO_UINT32 Capacity)
{
    return In >= Out ? In - Out : Capacity - (Out - In);
}

/**
 * @brief Copies bytes from a ring across wraparound without publishing a new
 *        consumer offset.
 * @param Buffer The destination buffer with space for at least Count bytes.
 * @param Ring The ring data area to read.
 * @param Capacity The nonzero ring data capacity in bytes.
 * @param Offset The starting offset, which must be less than Capacity.
 * @param Count The number of bytes to copy, at most Capacity.
 * @return This function does not return a value.
*/
static void VmbusUioCopyFromRing(
    _Mo_Out_ MO_POINTER Buffer,
    _Mo_In_ const MO_UINT8* Ring,
    _Mo_In_ MO_UINT32 Capacity,
    _Mo_In_ MO_UINT32 Offset,
    _Mo_In_ MO_UINT32 Count)
{
    MO_UINT32 First = Capacity - Offset;
    if (First > Count)
    {
        First = Count;
    }
    ::memcpy(Buffer, Ring + Offset, First);
    if (Count > First)
    {
        ::memcpy(static_cast<MO_UINT8*>(Buffer) + First, Ring, Count - First);
    }
}

/**
 * @brief Copies bytes into a ring across wraparound without publishing a new
 *        producer offset.
 * @param Buffer The source buffer containing at least Count bytes.
 * @param Ring The ring data area to write.
 * @param Capacity The nonzero ring data capacity in bytes.
 * @param Offset The starting offset, which must be less than Capacity.
 * @param Count The number of bytes to copy, at most Capacity.
 * @return The wrapped offset immediately after the copied bytes.
*/
static MO_UINT32 VmbusUioCopyToRing(
    _Mo_In_ MO_CONSTANT_POINTER Buffer,
    _Mo_Out_ MO_UINT8* Ring,
    _Mo_In_ MO_UINT32 Capacity,
    _Mo_In_ MO_UINT32 Offset,
    _Mo_In_ MO_UINT32 Count)
{
    MO_UINT32 First = Capacity - Offset;
    if (First > Count)
    {
        First = Count;
    }
    ::memcpy(Ring + Offset, Buffer, First);
    if (Count > First)
    {
        ::memcpy(
            Ring,
            static_cast<const MO_UINT8*>(Buffer) + First,
            Count - First);
    }
    return ::VmbusUioAdvance(Offset, Count, Capacity);
}

/**
 * @brief Checks channel availability through a nonblocking UIO event read.
 *        Retries interrupted reads and records unexpected errors as terminal.
 * @param Device The open channel context whose event descriptor is checked.
 * @return If an event is read or no event is pending, the function returns 0.
 *         Otherwise, it returns -1 and sets errno to the terminal error.
 *         Success does not imply that an incoming packet is available.
*/
static int VmbusUioCheck(_Mo_InOut_ VMBUS_UIO_CONTEXT& Device)
{
    if (Device.TerminalError)
    {
        errno = Device.TerminalError;
        return -1;
    }
    MO_UINT32 Events = 0;
    ssize_t Result = 0;
    do
    {
        Result = ::read(Device.FileDescriptor, &Events, sizeof(Events));
    } while (-1 == Result && EINTR == errno);
    if (sizeof(Events) == static_cast<size_t>(Result) ||
        (-1 == Result && (EAGAIN == errno || EWOULDBLOCK == errno)))
    {
        return 0;
    }
    int ErrorCode = -1 == Result ? errno : EIO;
    Device.TerminalError = (EIO == ErrorCode || EINVAL == ErrorCode)
        ? ENODEV
        : ErrorCode;
    errno = Device.TerminalError;
    return -1;
}

/**
 * @brief Notifies the peer after a ring index is published, using monitor
 *        flags or the UIO interrupt-control operation as appropriate.
 * @param Device The open channel context containing notification resources.
 * @return If the notification succeeds, the function returns 0. Otherwise,
 *         it records a terminal error, returns -1, and sets errno. EAGAIN
 *         and EWOULDBLOCK become EIO to prevent replay of a committed packet.
*/
static int VmbusUioSignal(_Mo_InOut_ VMBUS_UIO_CONTEXT& Device)
{
    ::MoMileMemoryBarrier();
    if (Device.UsesMonitor)
    {
        volatile MO_UINT32* Interrupts = reinterpret_cast<volatile MO_UINT32*>(
            static_cast<MO_UINT8*>(Device.Mappings[1]) + HV_PAGE_SIZE / 2);
        ::MoMileInterlockedOr32(
            Interrupts + Device.ChannelId / 32,
            static_cast<MO_UINT32>(1) << (Device.ChannelId % 32));
        volatile HV_MONITOR_PAGE* Monitor =
            static_cast<volatile HV_MONITOR_PAGE*>(Device.Mappings[2]);
        ::MoMileInterlockedOr32(
            &Monitor->TriggerGroup[Device.MonitorId / 32].Pending,
            static_cast<MO_UINT32>(1) << (Device.MonitorId % 32));
        return 0;
    }
    MO_UINT32 Enable = 1;
    ssize_t Result = 0;
    do
    {
        Result = ::write(Device.FileDescriptor, &Enable, sizeof(Enable));
    } while (-1 == Result && EINTR == errno);
    if (sizeof(Enable) == static_cast<size_t>(Result))
    {
        return 0;
    }
    int ErrorCode = -1 == Result ? errno : EIO;
    // Publication cannot be undone; never invite a packet replay.
    if (EAGAIN == ErrorCode || EWOULDBLOCK == ErrorCode)
    {
        ErrorCode = EIO;
    }
    Device.TerminalError = ErrorCode;
    errno = ErrorCode;
    return -1;
}

MO_EXTERN_C MO_INTN MOAPI VmbusUioRead(
    _Mo_In_ VMBUS_UIO_HANDLE Handle,
    _Mo_Out_ MO_POINTER Buffer,
    _Mo_In_ MO_UINTN Size)
{
    if (!Handle)
    {
        errno = EBADF;
        return -1;
    }
    if (!Size)
    {
        return 0;
    }
    VMBUS_UIO_CONTEXT& Device = *reinterpret_cast<PVMBUS_UIO_CONTEXT>(Handle);
    if (-1 == ::VmbusUioCheck(Device))
    {
        return -1;
    }
    volatile VMRCB& Ring = *Device.IncomingControl;
    MO_UINT32 In = Ring.In;
    MO_UINT32 Out = Ring.Out;
    ::MoMileMemoryBarrier();
    if (In >= Device.DataSize ||
        Out >= Device.DataSize ||
        In % 8 ||
        Out % 8)
    {
        errno = EPROTO;
        return -1;
    }
    MO_UINT32 Available = ::VmbusUioAvailable(In, Out, Device.DataSize);
    if (Available < sizeof(VMPACKET_DESCRIPTOR))
    {
        errno = EAGAIN;
        return -1;
    }
    VMPACKET_DESCRIPTOR Descriptor = {};
    ::VmbusUioCopyFromRing(
        &Descriptor,
        Device.IncomingData,
        Device.DataSize,
        Out,
        sizeof(Descriptor));
    MO_UINT32 Length = static_cast<MO_UINT32>(Descriptor.Length8) << 3;
    MO_UINT32 Offset = static_cast<MO_UINT32>(Descriptor.DataOffset8) << 3;
    MO_UINT32 PacketSize = Length + sizeof(PREVIOUS_PACKET_OFFSET);
    if (Offset < sizeof(Descriptor) || Offset > Length ||
        PacketSize > Available)
    {
        errno = EPROTO;
        return -1;
    }
    MO_UINT32 PayloadSize = Length - Offset;
    if (Device.NamedPipe)
    {
        if (VmbusPacketTypeDataInBand != Descriptor.Type ||
            PayloadSize < sizeof(VMPIPE_PROTOCOL_HEADER))
        {
            errno = EPROTO;
            return -1;
        }
        VMPIPE_PROTOCOL_HEADER Header = {};
        MO_UINT32 HeaderOffset = ::VmbusUioAdvance(
            Out,
            Offset,
            Device.DataSize);
        ::VmbusUioCopyFromRing(
            &Header,
            Device.IncomingData,
            Device.DataSize,
            HeaderOffset,
            sizeof(Header));
        if (VmPipeMessageData != Header.PacketType ||
            Header.DataSize > VMPIPE_MAXIMUM_PIPE_PACKET_SIZE ||
            Header.DataSize > PayloadSize - sizeof(Header) ||
            PayloadSize > ((sizeof(Header) + Header.DataSize + 7) &
                ~static_cast<size_t>(7)))
        {
            errno = EPROTO;
            return -1;
        }
        Offset += sizeof(Header);
        PayloadSize = Header.DataSize;
    }
    if (PayloadSize > Size)
    {
        errno = ENOBUFS;
        return -1;
    }
    if (PayloadSize)
    {
        if (!Buffer)
        {
            errno = EFAULT;
            return -1;
        }
        MO_UINT32 PayloadOffset = ::VmbusUioAdvance(
            Out,
            Offset,
            Device.DataSize);
        ::VmbusUioCopyFromRing(
            Buffer,
            Device.IncomingData,
            Device.DataSize,
            PayloadOffset,
            PayloadSize);
    }
    MO_UINT32 Next = ::VmbusUioAdvance(Out, PacketSize, Device.DataSize);
    ::MoMileMemoryBarrier();
    Ring.Out = Next;
    ::MoMileMemoryBarrier();
    if (Ring.FeatureBits.SupportsPendingSendSize)
    {
        MO_UINT32 Pending = Ring.PendingSendSize;
        if (Pending)
        {
            ::MoMileMemoryBarrier();
            In = Ring.In;
            if (In >= Device.DataSize || In % 8)
            {
                Device.TerminalError = EPROTO;
                errno = EPROTO;
                return -1;
            }
            MO_UINT32 Free = Device.DataSize -
                ::VmbusUioAvailable(In, Next, Device.DataSize);
            if (Free >= PacketSize &&
                Free - PacketSize <= Pending &&
                Free > Pending &&
                -1 == ::VmbusUioSignal(Device))
            {
                return -1;
            }
        }
    }
    return static_cast<MO_INTN>(PayloadSize);
}

MO_EXTERN_C MO_INTN MOAPI VmbusUioWrite(
    _Mo_In_ VMBUS_UIO_HANDLE Handle,
    _Mo_In_ MO_CONSTANT_POINTER Buffer,
    _Mo_In_ MO_UINTN Size)
{
    if (!Handle)
    {
        errno = EBADF;
        return -1;
    }
    if (!Size)
    {
        return 0;
    }
    VMBUS_UIO_CONTEXT& Device = *reinterpret_cast<PVMBUS_UIO_CONTEXT>(Handle);
    size_t PrefixSize = sizeof(VMPACKET_DESCRIPTOR) +
        (Device.NamedPipe ? sizeof(VMPIPE_PROTOCOL_HEADER) : 0);
    size_t MaximumLength = Device.DataSize - 16;
    if (MaximumLength > static_cast<size_t>(MO_UINT16_MAX) * 8)
    {
        MaximumLength = static_cast<size_t>(MO_UINT16_MAX) * 8;
    }
    if (Size > MaximumLength - PrefixSize ||
        (Device.NamedPipe && Size > VMPIPE_MAXIMUM_PIPE_PACKET_SIZE))
    {
        errno = EMSGSIZE;
        return -1;
    }
    if (!Buffer)
    {
        errno = EFAULT;
        return -1;
    }
    if (-1 == ::VmbusUioCheck(Device))
    {
        return -1;
    }
    volatile VMRCB& Ring = *Device.OutgoingControl;
    MO_UINT32 In = Ring.In;
    MO_UINT32 Out = Ring.Out;
    ::MoMileMemoryBarrier();
    if (In >= Device.DataSize || Out >= Device.DataSize ||
        In % 8 || Out % 8)
    {
        errno = EPROTO;
        return -1;
    }
    MO_UINT32 Length = static_cast<MO_UINT32>(
        (PrefixSize + Size + 7) & ~static_cast<MO_UINTN>(7));
    MO_UINT32 PacketSize = Length + sizeof(PREVIOUS_PACKET_OFFSET);
    MO_UINT32 Available = Device.DataSize -
        ::VmbusUioAvailable(In, Out, Device.DataSize);
    if (Available <= PacketSize)
    {
        errno = EAGAIN;
        return -1;
    }
    VMPACKET_DESCRIPTOR Descriptor = {};
    Descriptor.Type = VmbusPacketTypeDataInBand;
    Descriptor.DataOffset8 = sizeof(Descriptor) / 8;
    Descriptor.Length8 = static_cast<MO_UINT16>(Length / 8);
    MO_UINT32 Next = ::VmbusUioCopyToRing(
        &Descriptor,
        Device.OutgoingData,
        Device.DataSize,
        In,
        sizeof(Descriptor));
    if (Device.NamedPipe)
    {
        VMPIPE_PROTOCOL_HEADER Header = {};
        Header.PacketType = VmPipeMessageData;
        Header.DataSize = static_cast<MO_UINT32>(Size);
        Next = ::VmbusUioCopyToRing(
            &Header,
            Device.OutgoingData,
            Device.DataSize,
            Next,
            sizeof(Header));
    }
    Next = ::VmbusUioCopyToRing(
        Buffer,
        Device.OutgoingData,
        Device.DataSize,
        Next,
        static_cast<MO_UINT32>(Size));
    MO_UINT64 Padding = 0;
    MO_UINT32 PaddingSize = static_cast<MO_UINT32>(
        Length - PrefixSize - Size);
    Next = ::VmbusUioCopyToRing(
        &Padding,
        Device.OutgoingData,
        Device.DataSize,
        Next,
        PaddingSize);
    PREVIOUS_PACKET_OFFSET Previous = {};
    Previous.Offset = In;
    Next = ::VmbusUioCopyToRing(
        &Previous,
        Device.OutgoingData,
        Device.DataSize,
        Next,
        sizeof(Previous));
    ::MoMileMemoryBarrier();
    Ring.In = Next;
    ::MoMileMemoryBarrier();
    if (!Ring.InterruptMask)
    {
        ::MoMileMemoryBarrier();
        // Re-read the peer index: it may have drained during the copy.
        Out = Ring.Out;
        if (Out >= Device.DataSize || Out % 8)
        {
            Device.TerminalError = EPROTO;
            errno = EPROTO;
            return -1;
        }
        if (In == Out && -1 == ::VmbusUioSignal(Device))
        {
            return -1;
        }
    }
    return static_cast<MO_INTN>(Size);
}
