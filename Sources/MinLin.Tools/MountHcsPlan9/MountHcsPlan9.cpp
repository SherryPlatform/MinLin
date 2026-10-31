/*
 * PROJECT:    MinLin
 * FILE:       MountHcsPlan9.cpp
 * PURPOSE:    Implementation for Hyper-V HCS VM Plan 9 Share Mount Utility
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <Mile.Mobility.Portable.Types.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <unistd.h>

#include <linux/vm_sockets.h>

static constexpr int g_BufferSize = 65536;
// Leave room in a 4096-byte mount option buffer for two decimal int fds.
static constexpr size_t g_MaximumAccessNameLength = 4000;

static void PrintUsage(
    _Mo_In_ FILE* Stream)
{
    ::fprintf(
        Stream,
        "Usage: mounthcs9p [-r|--read-only] [--] Port AccessName MountPoint\n"
        "       mounthcs9p -h|--help\n"
        "\n"
        "Mount a user-specified Hyper-V HCS VM Plan 9 (9p) share.\n"
        "Options must precede the three positional arguments.\n"
        "  -r, --read-only  Mount read-only (default: read-write).\n"
        "  -h, --help       Show this help as the only argument.\n"
        "\n"
        "These settings are based on the HCS Plan9 related settings:\n"
        "Port: Hyper-V Socket port number, decimal 0-4294967294.\n"
        "AccessName: The access name string, 1-4000 bytes with no comma.\n"
        "\n"
        "MountPoint: Mount point target directory, which need to be existed.\n"
        "\n"
        "Exit status:\n"
        "  0 = Success\n"
        "  1 = Failed\n"
        "  2 = Invalid Arguments\n");
}

/**
 * @brief Reports one operation failure without losing its errno to printing.
 * @return The operation-failure exit status, 1.
 */
static int ReportError(
    _Mo_In_ MO_CONSTANT_STRING Operation,
    _Mo_In_ int ErrorCode)
{
    ::fprintf(
        stderr,
        "mounthcs9p: %s failed: %s (errno=%d).\n",
        Operation,
        ::strerror(ErrorCode),
        ErrorCode);
    errno = ErrorCode;
    return 1;
}

/**
 * @brief Mounts one host share using already validated command-line values.
 * @param Port A concrete VSOCK port, not VMADDR_PORT_ANY.
 * @param AccessName A nonempty, comma-free name of at most 4000 bytes.
 * @param MountPoint A nonempty path; the kernel validates the mount target.
 * @param ReadOnly Whether to request MS_RDONLY.
 * @return 0 after a successful mount, or 1 with an operation diagnostic.
 * @remarks No timeout, retry, directory creation, unmount or module command.
 *          A successful 9p fd mount keeps its own socket file references.
 */
static int MountHcsPlan9(
    _Mo_In_ MO_UINT32 Port,
    _Mo_In_ MO_CONSTANT_STRING AccessName,
    _Mo_In_ MO_CONSTANT_STRING MountPoint,
    _Mo_In_ bool ReadOnly)
{
    int SocketFileDescriptor = ::socket(
        AF_VSOCK,
        SOCK_STREAM | SOCK_CLOEXEC,
        0);
    if (-1 == SocketFileDescriptor)
    {
        return ::ReportError("socket(AF_VSOCK)", errno);
    }

    int ErrorCode = 0;
    MO_CONSTANT_STRING Operation = nullptr;
    do
    {
        sockaddr_vm SocketAddress = {};
        SocketAddress.svm_family = AF_VSOCK;
        SocketAddress.svm_port = Port;
        SocketAddress.svm_cid = VMADDR_CID_HOST;
        if (-1 == ::connect(
            SocketFileDescriptor,
            reinterpret_cast<const sockaddr*>(&SocketAddress),
            sizeof(SocketAddress)))
        {
            ErrorCode = errno;
            Operation = "connect";
            break;
        }

        int SocketBufferSize = g_BufferSize;
        if (-1 == ::setsockopt(
            SocketFileDescriptor,
            SOL_SOCKET,
            SO_SNDBUF,
            &SocketBufferSize,
            sizeof(SocketBufferSize)))
        {
            ErrorCode = errno;
            Operation = "setsockopt(SO_SNDBUF)";
            break;
        }

        char Options[4096] = {};
        errno = 0;
        // Keep the original option string. In upstream Linux 6.12, noload
        // is ignored by 9p; it does not suppress kernel module autoloading.
        int Length = ::snprintf(
            Options,
            sizeof(Options),
            "trans=fd,rfdno=%d,wfdno=%d,msize=%d,noload,aname=%s",
            SocketFileDescriptor,
            SocketFileDescriptor,
            g_BufferSize,
            AccessName);
        if (Length < 0 || static_cast<size_t>(Length) >= sizeof(Options))
        {
            ErrorCode = Length < 0 ? (errno ? errno : EIO) : EOVERFLOW;
            Operation = "format mount options";
            break;
        }

        if (-1 == ::mount(
            AccessName,
            MountPoint,
            "9p",
            ReadOnly ? MS_RDONLY : 0,
            Options))
        {
            ErrorCode = errno;
            Operation = "mount(9p)";
            break;
        }
    } while (false);

    // Never retry close or let it replace the mount result. In particular,
    // a successful mount must not be reported as a failed mount to scripts.
    ::close(SocketFileDescriptor);
    return ErrorCode ? ::ReportError(Operation, ErrorCode) : 0;
}

int main(int argc, char* argv[])
{
    if (2 == argc &&
        (0 == ::strcmp(argv[1], "-h") ||
        0 == ::strcmp(argv[1], "--help")))
    {
        ::PrintUsage(stdout);
        return 0;
    }

    int FirstArgument = 1;
    bool ReadOnly = false;
    if (FirstArgument < argc &&
        (0 == ::strcmp(argv[FirstArgument], "-r") ||
        0 == ::strcmp(argv[FirstArgument], "--read-only")))
    {
        ReadOnly = true;
        ++FirstArgument;
    }
    if (FirstArgument < argc &&
        0 == ::strcmp(argv[FirstArgument], "--"))
    {
        ++FirstArgument;
    }
    if (3 != argc - FirstArgument)
    {
        ::PrintUsage(stderr);
        return 2;
    }

    MO_CONSTANT_STRING PortText = argv[FirstArgument];
    MO_STRING PortEnd = nullptr;
    errno = 0;
    unsigned long PortNumber = ::strtoul(PortText, &PortEnd, 10);
    if (errno ||
        PortText[0] < '0' ||
        PortText[0] > '9' ||
        '\0' != *PortEnd ||
        PortNumber >= static_cast<unsigned long>(VMADDR_PORT_ANY))
    {
        ::fprintf(stderr,
            "mounthcs9p: Port must be decimal 0-4294967294.\n");
        return 2;
    }

    MO_CONSTANT_STRING AccessName = argv[FirstArgument + 1];
    size_t AccessNameLength = ::strnlen(
        AccessName,
        g_MaximumAccessNameLength + 1);
    if (!AccessNameLength ||
        AccessNameLength > g_MaximumAccessNameLength ||
        ::strchr(AccessName, ','))
    {
        ::fprintf(
            stderr,
            "mounthcs9p: AccessName must be 1-4000 bytes with no comma.\n");
        return 2;
    }

    MO_CONSTANT_STRING MountPoint = argv[FirstArgument + 2];
    if (!MountPoint[0])
    {
        ::fprintf(stderr, "mounthcs9p: MountPoint must not be empty.\n");
        return 2;
    }

    return ::MountHcsPlan9(
        static_cast<MO_UINT32>(PortNumber),
        AccessName,
        MountPoint,
        ReadOnly);
}
