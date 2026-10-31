/*
 * PROJECT:    MinLin
 * FILE:       SynthRdp.cpp
 * PURPOSE:    Implementation for Hyper-V Enhanced Session Proxy Service
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include <Mile.Mobility.Portable.Types.h>
#include <Mile.HyperV.VMBus.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "VmbusPipe.h"
#include "VmbusUio.h"

// Backend selection belongs here; UIO Open owns its registration and binding.
enum class SynthRdpBackend
{
    Unselected,
    Kernel,
    Uio
};

static SynthRdpBackend g_Backend = SynthRdpBackend::Unselected;

// Each local channel owns either a real Pipe fd or an opaque UIO handle.
// Do not copy an open channel. The worker borrows it; main closes it.
struct SynthRdpChannel
{
    int FileDescriptor = -1;
    VMBUS_UIO_HANDLE UioHandle = nullptr;
};

/**
 * @brief Formats a GUID for diagnostics, without accessing a device.
 * @param Identifier The GUID value to format.
 * @param Text Receives 36 lowercase characters and a terminating null.
 * @return None. The destination must have space for 37 characters.
*/
static void SynthRdpFormatGuid(
    _Mo_In_ MO_GUID Identifier,
    _Mo_Out_ MO_STRING Text)
{
    ::snprintf(Text, 37,
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
 * @brief Selects the requested backend once and opens a SynthRdp Pipe channel.
 *        UIO Open performs registration and binding for the requested pair.
 *        Never falls back to another backend or loads a driver.
 * @param ClassId The protocol class GUID.
 * @param InstanceId The channel instance GUID.
 * @param Channel An empty channel that receives ownership on success.
 * @param ReportMissing Whether to report ENOENT as well as other errors.
 * @return 0 on success; otherwise -1 with errno set. Preparation is not
 *         rolled back. An already open Channel is rejected with EBUSY.
*/
static int SynthRdpOpen(
    _Mo_In_ MO_GUID ClassId,
    _Mo_In_ MO_GUID InstanceId,
    _Mo_InOut_ SynthRdpChannel& Channel,
    _Mo_In_ bool ReportMissing = true)
{
    if (Channel.UioHandle || -1 != Channel.FileDescriptor)
    {
        errno = EBUSY;
        return -1;
    }
    if (SynthRdpBackend::Unselected == g_Backend)
    {
        MO_CONSTANT_STRING Backend = ::getenv("SYNTHRDP_BACKEND");
        if (!Backend || 0 == ::strcmp(Backend, "hv_vmbus_pipe"))
        {
            g_Backend = SynthRdpBackend::Kernel;
        }
        else if (0 == ::strcmp(Backend, "uio_hv_generic"))
        {
            g_Backend = SynthRdpBackend::Uio;
        }
        else
        {
            ::printf("[Error] SYNTHRDP_BACKEND Invalid.\n");
            errno = EINVAL;
            return -1;
        }
    }
    const int Flags = O_RDWR | O_NONBLOCK | O_CLOEXEC;
    if (SynthRdpBackend::Kernel == g_Backend)
    {
        Channel.FileDescriptor = ::VmbusPipeOpen(
            InstanceId,
            ClassId,
            Flags,
            MO_TRUE);
        if (-1 != Channel.FileDescriptor)
        {
            return 0;
        }
    }
    else
    {
        Channel.UioHandle = ::VmbusUioOpen(
            ClassId,
            InstanceId,
            static_cast<MO_UINT32>(Flags),
            MO_TRUE);
        if (Channel.UioHandle)
        {
            return 0;
        }
    }
    int ErrorCode = errno;
    if (ReportMissing || ENOENT != ErrorCode)
    {
        char ClassText[37] = {};
        char InstanceText[37] = {};
        ::SynthRdpFormatGuid(ClassId, ClassText);
        ::SynthRdpFormatGuid(InstanceId, InstanceText);
        ::printf(
            "[Error] %s(class=%s, instance=%s) failed (%d).\n",
            SynthRdpBackend::Uio == g_Backend
                ? "VmbusUioOpen"
                : "VmbusPipeOpen",
            ClassText,
            InstanceText,
            ErrorCode);
    }
    errno = ErrorCode;
    return -1;
}

/**
 * @brief Reads one packet through the channel's backend.
 * @param Channel The open channel to read.
 * @param Buffer The writable destination buffer.
 * @param BufferSize The destination capacity in bytes.
 * @return The backend result, preserving its packet and errno semantics.
*/
static ssize_t SynthRdpRead(
    _Mo_In_ const SynthRdpChannel& Channel,
    _Mo_Out_ MO_POINTER Buffer,
    _Mo_In_ size_t BufferSize)
{
    return Channel.UioHandle
        ? static_cast<ssize_t>(
            ::VmbusUioRead(Channel.UioHandle, Buffer, BufferSize))
        : ::VmbusPipeRead(Channel.FileDescriptor, Buffer, BufferSize);
}

/**
 * @brief Writes one packet through the channel's backend.
 * @param Channel The open channel to write.
 * @param Buffer The read-only source buffer.
 * @param BufferSize The source size in bytes.
 * @return The backend result without adding waits, retries or stream framing.
*/
static ssize_t SynthRdpWrite(
    _Mo_In_ const SynthRdpChannel& Channel,
    _Mo_In_ MO_CONSTANT_POINTER Buffer,
    _Mo_In_ size_t BufferSize)
{
    return Channel.UioHandle
        ? static_cast<ssize_t>(
            ::VmbusUioWrite(Channel.UioHandle, Buffer, BufferSize))
        : ::VmbusPipeWrite(Channel.FileDescriptor, Buffer, BufferSize);
}

/**
 * @brief Closes a channel and clears its owned resource, even on failure.
 * @param Channel The channel to close. Do not retry a failed close.
 * @return 0 on success or -1 with errno set on failure. Closing does not
 *         undo UIO registration or binding.
*/
static int SynthRdpClose(
    _Mo_InOut_ SynthRdpChannel& Channel)
{
    if (Channel.UioHandle)
    {
        VMBUS_UIO_HANDLE Handle = Channel.UioHandle;
        Channel.UioHandle = nullptr;
        return ::VmbusUioClose(Handle) ? 0 : -1;
    }
    int FileDescriptor = Channel.FileDescriptor;
    Channel.FileDescriptor = -1;
    return ::VmbusPipeClose(FileDescriptor);
}

/**
 * @brief Reports the selected backend and instance, not a device file path.
 * @param InstanceId The GUID of a successfully opened data channel.
 * @return None. Preserves errno and does not query or open any device.
*/
static void SynthRdpReportDataChannel(
    _Mo_In_ MO_GUID InstanceId)
{
    int ErrorCode = errno;
    char Text[37] = {};
    ::SynthRdpFormatGuid(InstanceId, Text);
    ::printf(
        "[SynthRdp] Data channel: backend=%s, instance=%s\n",
        SynthRdpBackend::Uio == g_Backend
            ? "uio_hv_generic"
            : "hv_vmbus_pipe",
        Text);
    errno = ErrorCode;
}

/**
 * @brief Sends the whole TCP buffer, waiting as needed.
 * @return BufferSize on success, or -1 with errno set on failure.
 * @note Failure may follow a partial send; do not retry the whole buffer.
 */
ssize_t TcpSend(
    _Mo_In_ int SocketFileDescriptor,
    _Mo_In_ const void* Buffer,
    _Mo_In_ size_t BufferSize,
    _Mo_In_ int Flags)
{
    if (BufferSize > static_cast<size_t>(SSIZE_MAX))
    {
        errno = EMSGSIZE;
        return -1;
    }

    const MO_UINT8* CurrentBuffer = static_cast<const MO_UINT8*>(Buffer);
    size_t NumberOfBytesSent = 0;
    do
    {
        ssize_t NumberOfBytesWritten = ::send(
            SocketFileDescriptor,
            CurrentBuffer,
            BufferSize - NumberOfBytesSent,
            Flags);
        if (NumberOfBytesWritten > 0)
        {
            CurrentBuffer += NumberOfBytesWritten;
            NumberOfBytesSent += static_cast<size_t>(NumberOfBytesWritten);
            if (NumberOfBytesSent == BufferSize)
            {
                return static_cast<ssize_t>(NumberOfBytesSent);
            }
            continue;
        }
        if (0 == NumberOfBytesWritten)
        {
            if (NumberOfBytesSent == BufferSize)
            {
                return static_cast<ssize_t>(NumberOfBytesSent);
            }
            errno = EPIPE;
            return -1;
        }
        if (EINTR == errno)
        {
            continue;
        }
        if (EAGAIN != errno && EWOULDBLOCK != errno)
        {
            return -1;
        }

        ::usleep(5000);
    } while (true);
}

/**
 * @brief Redirects one SynthRdp data connection to the target RDP server.
 * @param Channel An open data channel owned and closed by the caller.
 * @param ServerHost An IPv4 address or hostname.
 * @param ServerPort The numeric TCP port, in decimal notation.
 */
void SynthRdpRedirectionWorker(
    _Mo_In_ const SynthRdpChannel& Channel,
    _Mo_In_ MO_CONSTANT_STRING ServerHost,
    _Mo_In_ MO_CONSTANT_STRING ServerPort)
{
    int SocketFileDescriptor = -1;

    do
    {
        // Resolve and connect to the target server.
        {
            addrinfo AddressHints = {};
            AddressHints.ai_family = AF_INET;
            AddressHints.ai_socktype = SOCK_STREAM;
            AddressHints.ai_protocol = IPPROTO_TCP;
            AddressHints.ai_flags = AI_NUMERICSERV;
            addrinfo* AddressInfo = nullptr;
            int Result = ::getaddrinfo(
                ServerHost,
                ServerPort,
                &AddressHints,
                &AddressInfo);
            if (0 != Result)
            {
                if (EAI_SYSTEM == Result)
                {
                    int ErrorCode = errno;
                    ::printf(
                        "[Error] getaddrinfo failed (%d), errno (%d).\n",
                        Result,
                        ErrorCode);
                }
                else
                {
                    ::printf("[Error] getaddrinfo failed (%d).\n", Result);
                }
                break;
            }

            for (addrinfo* CurrentAddressInfo = AddressInfo;
                CurrentAddressInfo;
                CurrentAddressInfo = CurrentAddressInfo->ai_next)
            {
                SocketFileDescriptor = ::socket(
                    CurrentAddressInfo->ai_family,
                    CurrentAddressInfo->ai_socktype | SOCK_CLOEXEC,
                    CurrentAddressInfo->ai_protocol);
                if (-1 == SocketFileDescriptor)
                {
                    int ErrorCode = errno;
                    ::printf("[Error] socket failed (%d).\n", ErrorCode);
                    continue;
                }
                if (0 == ::connect(
                    SocketFileDescriptor,
                    CurrentAddressInfo->ai_addr,
                    CurrentAddressInfo->ai_addrlen))
                {
                    break;
                }
                int ErrorCode = errno;
                ::printf("[Error] connect failed (%d).\n", ErrorCode);
                ::close(SocketFileDescriptor);
                SocketFileDescriptor = -1;
            }
            ::freeaddrinfo(AddressInfo);
            if (-1 == SocketFileDescriptor)
            {
                break;
            }
        }

        ::printf("[SynthRdp] Connected to %s:%s\n", ServerHost, ServerPort);

        MO_UINT8 ReceiveBuffer[VMPIPE_MAXIMUM_PIPE_PACKET_SIZE] = {};
        size_t ReceiveLength = 0;
        size_t ReceiveOffset = 0;

        // X.224 Connection Request PDU (Patched)
        {
            MO_UINT8 ConnectionRequest[260] = {};
            size_t ConnectionRequestSize = 0;
            size_t RequiredSize = 4;
            int ErrorCode = 0;
            while (ConnectionRequestSize < RequiredSize)
            {
                if (ReceiveOffset == ReceiveLength)
                {
                    ssize_t NumberOfBytesRead = ::SynthRdpRead(
                        Channel,
                        ReceiveBuffer,
                        sizeof(ReceiveBuffer));
                    if (-1 == NumberOfBytesRead)
                    {
                        if (EAGAIN == errno ||
                            EWOULDBLOCK == errno ||
                            EINTR == errno)
                        {
                            ::usleep(5000);
                            continue;
                        }
                        ErrorCode = errno;
                        ::printf("[Error] read failed (%d).\n", ErrorCode);
                        break;
                    }
                    ReceiveLength = static_cast<size_t>(NumberOfBytesRead);
                    ReceiveOffset = 0;
                    if (0 == ReceiveLength)
                    {
                        ::usleep(5000);
                        continue;
                    }
                }
                size_t CopyLength = MO_MIN(
                    RequiredSize - ConnectionRequestSize,
                    ReceiveLength - ReceiveOffset);
                ::memcpy(
                    ConnectionRequest + ConnectionRequestSize,
                    ReceiveBuffer + ReceiveOffset,
                    CopyLength);
                ConnectionRequestSize += CopyLength;
                ReceiveOffset += CopyLength;
                if (4 == ConnectionRequestSize)
                {
                    RequiredSize =
                        (static_cast<size_t>(ConnectionRequest[2]) << 8) |
                        ConnectionRequest[3];
                    if (3 != ConnectionRequest[0] ||
                        0 != ConnectionRequest[1] ||
                        RequiredSize < 11 ||
                        RequiredSize > sizeof(ConnectionRequest))
                    {
                        ErrorCode = EPROTO;
                        ::printf(
                            "[Error] X.224 Connection Request PDU Invalid.\n");
                        break;
                    }
                }
            }
            if (0 != ErrorCode)
            {
                break;
            }
            if (0xe0 != ConnectionRequest[5] ||
                static_cast<size_t>(ConnectionRequest[4]) + 5 !=
                    ConnectionRequestSize)
            {
                ::printf("[Error] X.224 Connection Request PDU Invalid.\n");
                break;
            }
            size_t NegotiationOffset = 11;
            if (NegotiationOffset < ConnectionRequestSize &&
                1 != ConnectionRequest[NegotiationOffset])
            {
                while (NegotiationOffset + 1 < ConnectionRequestSize &&
                    !(13 == ConnectionRequest[NegotiationOffset] &&
                      10 == ConnectionRequest[NegotiationOffset + 1]))
                {
                    ++NegotiationOffset;
                }
                NegotiationOffset += 2;
            }
            if (NegotiationOffset > ConnectionRequestSize ||
                (NegotiationOffset < ConnectionRequestSize &&
                 (ConnectionRequestSize - NegotiationOffset < 8 ||
                  1 != ConnectionRequest[NegotiationOffset] ||
                  8 != ConnectionRequest[NegotiationOffset + 2] ||
                  0 != ConnectionRequest[NegotiationOffset + 3])))
            {
                ::printf("[Error] X.224 Connection Request PDU Invalid.\n");
                break;
            }
            if (NegotiationOffset < ConnectionRequestSize)
            {
                ::memset(ConnectionRequest + NegotiationOffset + 4, 0, 4);
            }

            if (-1 == ::TcpSend(
                SocketFileDescriptor,
                ConnectionRequest,
                ConnectionRequestSize,
                MSG_NOSIGNAL))
            {
                ErrorCode = errno;
                ::printf("[Error] TcpSend failed (%d).\n", ErrorCode);
                break;
            }
        }

        MO_UINT8 SendBuffer[VMPIPE_MAXIMUM_PIPE_PACKET_SIZE] = {};
        size_t SendLength = 0;
        bool SocketClosed = false;
        while (!SocketClosed || 0 != SendLength)
        {
            bool Progress = false;
            if (!SocketClosed && ReceiveOffset == ReceiveLength)
            {
                ssize_t NumberOfBytesRead = ::SynthRdpRead(
                    Channel,
                    ReceiveBuffer,
                    sizeof(ReceiveBuffer));
                if (NumberOfBytesRead >= 0)
                {
                    ReceiveLength = static_cast<size_t>(NumberOfBytesRead);
                    ReceiveOffset = 0;
                    Progress = 0 != ReceiveLength;
                }
                else if (
                    EAGAIN != errno &&
                    EWOULDBLOCK != errno &&
                    EINTR != errno)
                {
                    int ErrorCode = errno;
                    ::printf("[Error] read failed (%d).\n", ErrorCode);
                    break;
                }
            }
            if (!SocketClosed && ReceiveOffset < ReceiveLength)
            {
                if (-1 == ::TcpSend(
                    SocketFileDescriptor,
                    ReceiveBuffer + ReceiveOffset,
                    ReceiveLength - ReceiveOffset,
                    MSG_NOSIGNAL))
                {
                    int ErrorCode = errno;
                    ::printf("[Error] TcpSend failed (%d).\n", ErrorCode);
                    break;
                }
                ReceiveOffset = ReceiveLength;
                Progress = true;
            }
            if (!SocketClosed && 0 == SendLength)
            {
                ssize_t NumberOfBytesReceived = ::recv(
                    SocketFileDescriptor,
                    SendBuffer,
                    sizeof(SendBuffer),
                    MSG_DONTWAIT);
                if (NumberOfBytesReceived > 0)
                {
                    SendLength = static_cast<size_t>(NumberOfBytesReceived);
                    Progress = true;
                }
                else if (0 == NumberOfBytesReceived)
                {
                    SocketClosed = true;
                }
                else if (
                    EAGAIN != errno &&
                    EWOULDBLOCK != errno &&
                    EINTR != errno)
                {
                    int ErrorCode = errno;
                    ::printf("[Error] recv failed (%d).\n", ErrorCode);
                    break;
                }
            }
            if (0 != SendLength)
            {
                ssize_t NumberOfBytesWritten = ::SynthRdpWrite(
                    Channel,
                    SendBuffer,
                    SendLength);
                if (NumberOfBytesWritten == static_cast<ssize_t>(SendLength))
                {
                    SendLength = 0;
                    Progress = true;
                }
                else if (
                    NumberOfBytesWritten >= 0 ||
                    (EAGAIN != errno && EWOULDBLOCK != errno && EINTR != errno))
                {
                    int ErrorCode = NumberOfBytesWritten >= 0 ? EIO : errno;
                    ::printf("[Error] write failed (%d).\n", ErrorCode);
                    break;
                }
            }
            if (!Progress && !SocketClosed)
            {
                ::usleep(5000);
            }
        }
    } while (false);

    if (-1 != SocketFileDescriptor)
    {
        ::close(SocketFileDescriptor);
    }
}

int main(int argc, char* argv[])
{
    ::setbuf(stdout, nullptr);

    MO_CONSTANT_STRING ServerHost = argc > 1
        ? argv[1]
        : "127.0.0.1";
    MO_CONSTANT_STRING ServerPort = argc > 2
        ? argv[2]
        : "3389";
    // Validate configuration before opening any VMBus channel.
    {
        bool ShowHelp =
            2 == argc &&
            (0 == ::strcmp(ServerHost, "--help") ||
            0 == ::strcmp(ServerHost, "-h"));
        if (ShowHelp || argc > 3)
        {
            if (!ShowHelp)
            {
                ::printf("[Error] argv Invalid.\n");
            }
            ::printf(
                "Usage: SynthRdp [ServerHost [ServerPort]]\n"
                "ServerHost: IPv4 address or hostname (default: 127.0.0.1)\n"
                "ServerPort: decimal port 1-65535 (default: 3389)\n");
            return ShowHelp ? 0 : EINVAL;
        }
        if (!ServerHost[0] || '-' == ServerHost[0])
        {
            ::printf("[Error] ServerHost Invalid.\n");
            return EINVAL;
        }
        MO_STRING PortEnd = nullptr;
        errno = 0;
        unsigned long PortNumber = ::strtoul(ServerPort, &PortEnd, 10);
        if (0 != errno ||
            ServerPort[0] < '0' ||
            ServerPort[0] > '9' ||
            '\0' != *PortEnd ||
            0 == PortNumber ||
            PortNumber > 65535)
        {
            ::printf("[Error] ServerPort Invalid.\n");
            return EINVAL;
        }
    }

    int ErrorCode = 0;
    SynthRdpChannel ControlChannel = {};

    do
    {
        if (-1 == ::SynthRdpOpen(
            SYNTHRDP_CONTROL_CLASS_ID,
            SYNTHRDP_CONTROL_INSTANCE_ID,
            ControlChannel))
        {
            ErrorCode = errno;
            break;
        }

        SYNTHRDP_VERSION_REQUEST_MESSAGE Request = {};
        Request.Header.Type = SynthrdpVersionRequest;
        Request.Header.Size = sizeof(Request);
        Request.Version.AsDWORD = SYNTHRDP_VERSION_WINBLUE;
        Request.Reserved = 0;
        ssize_t NumberOfBytesWritten = 0;
        do
        {
            NumberOfBytesWritten = ::SynthRdpWrite(
                ControlChannel,
                &Request,
                sizeof(Request));
            if (NumberOfBytesWritten >= 0 ||
                (EAGAIN != errno && EWOULDBLOCK != errno && EINTR != errno))
            {
                break;
            }
            ::usleep(5000);
        } while (true);
        if (-1 == NumberOfBytesWritten)
        {
            ErrorCode = errno;
            ::printf("[Error] write failed (%d).\n", ErrorCode);
            break;
        }
        if (sizeof(Request) != static_cast<size_t>(NumberOfBytesWritten))
        {
            ErrorCode = EIO;
            ::printf("[Error] SYNTHRDP_VERSION_REQUEST_MESSAGE Invalid.\n");
            break;
        }

        SYNTHRDP_VERSION_RESPONSE_MESSAGE Response = {};
        ssize_t NumberOfBytesRead = 0;
        do
        {
            NumberOfBytesRead = ::SynthRdpRead(
                ControlChannel,
                &Response,
                sizeof(Response));
            if (NumberOfBytesRead >= 0 ||
                (EAGAIN != errno && EWOULDBLOCK != errno && EINTR != errno))
            {
                break;
            }
            ::usleep(5000);
        } while (true);
        if (-1 == NumberOfBytesRead)
        {
            ErrorCode = errno;
            ::printf("[Error] read failed (%d).\n", ErrorCode);
            break;
        }
        if (sizeof(Response) != static_cast<size_t>(NumberOfBytesRead) ||
            SYNTHRDP_TRUE_WITH_VERSION_EXCHANGE != Response.IsAccepted)
        {
            ErrorCode = EPROTO;
            ::printf("[Error] SYNTHRDP_VERSION_RESPONSE_MESSAGE Invalid.\n");
            break;
        }

        ::printf("[SynthRdp] Service initialized.\n");

        const MO_GUID* Instances[] =
        {
            &SYNTHRDP_DATA_INSTANCE_ID_1,
            &SYNTHRDP_DATA_INSTANCE_ID_2,
            &SYNTHRDP_DATA_INSTANCE_ID_3,
            &SYNTHRDP_DATA_INSTANCE_ID_4,
            &SYNTHRDP_DATA_INSTANCE_ID_5
        };
        for (size_t InstanceIndex = 0; ; ++InstanceIndex)
        {
            MO_GUID InstanceId =
                *Instances[InstanceIndex % MO_ARRAY_SIZE(Instances)];
            SynthRdpChannel DataChannel = {};
            if (-1 == ::SynthRdpOpen(
                SYNTHRDP_DATA_CLASS_ID,
                InstanceId,
                DataChannel,
                false))
            {
                ::usleep(50000);
                continue;
            }

            ::SynthRdpReportDataChannel(InstanceId);
            ::SynthRdpRedirectionWorker(
                DataChannel,
                ServerHost,
                ServerPort);
            ::SynthRdpClose(DataChannel);
        }
    } while (false);

    if (ControlChannel.UioHandle || -1 != ControlChannel.FileDescriptor)
    {
        ::SynthRdpClose(ControlChannel);
    }
    return ErrorCode;
}
