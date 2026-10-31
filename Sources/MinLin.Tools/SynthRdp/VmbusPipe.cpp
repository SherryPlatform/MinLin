/*
 * PROJECT:    MinLin
 * FILE:       VmbusPipe.cpp
 * PURPOSE:    Implementation for VMBus Kernel Pipe Backend
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "VmbusPipe.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

MO_EXTERN_C int MOAPI VmbusPipeClose(
    _Mo_In_ int FileDescriptor)
{
    return ::close(FileDescriptor);
}

/**
 * @brief Formats a GUID for a kernel device path.
 * @param Identifier The GUID value to format.
 * @param Buffer Receives lowercase canonical text without braces. The buffer
 *               must provide at least 37 bytes, including the null terminator.
 * @return This function does not return a value.
*/
static void VmbusPipeFormatGuid(
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
 * @brief Builds the kernel device path from structured GUIDs.
 * @param InstanceId The device instance GUID.
 * @param ClassId The device class GUID.
 * @param Path Receives the null-terminated /dev/vmbus path. The buffer must
 *             provide at least 85 bytes.
 * @return This function does not return a value.
*/
static void VmbusPipeFormatPath(
    _Mo_In_ MO_GUID InstanceId,
    _Mo_In_ MO_GUID ClassId,
    _Mo_Out_ MO_STRING Path)
{
    ::memcpy(Path, "/dev/vmbus/", 11);
    ::VmbusPipeFormatGuid(InstanceId, Path + 11);
    Path[47] = '.';
    ::VmbusPipeFormatGuid(ClassId, Path + 48);
}

MO_EXTERN_C int MOAPI VmbusPipeOpen(
    _Mo_In_ MO_GUID InstanceId,
    _Mo_In_ MO_GUID ClassId,
    _Mo_In_ int Flags,
    _Mo_In_ MO_BOOL PipeMode)
{
    MO_UNREFERENCED_PARAMETER(PipeMode);
    if ((Flags & O_CREAT) ||
        O_TMPFILE == (Flags & O_TMPFILE))
    {
        errno = EINVAL;
        return -1;
    }
    char Path[85] = {};
    ::VmbusPipeFormatPath(InstanceId, ClassId, Path);
    return ::open(Path, Flags);
}

MO_EXTERN_C ssize_t MOAPI VmbusPipeRead(
    _Mo_In_ int FileDescriptor,
    _Mo_Out_ MO_POINTER Buffer,
    _Mo_In_ size_t Size)
{
    return ::read(FileDescriptor, Buffer, Size);
}

MO_EXTERN_C ssize_t MOAPI VmbusPipeWrite(
    _Mo_In_ int FileDescriptor,
    _Mo_In_ MO_CONSTANT_POINTER Buffer,
    _Mo_In_ size_t Size)
{
    return ::write(FileDescriptor, Buffer, Size);
}
