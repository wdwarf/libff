/**
 * @file WsaHelper.h
 * @author liyawu
 * @date 2024-07-04 09:24:09
 * @description
 */

#ifndef _WSAHELPER_H
#define _WSAHELPER_H

#include <ff/ff_config.h>
#include <windows.h>

#include <string>

NS_FF_BEG

// AcceptEx
#ifndef WSAID_ACCEPTEX

#define WSAID_ACCEPTEX                               \
  {                                                  \
    0xb5367df1, 0xcbac, 0x11cf, {                    \
      0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92 \
    }                                                \
  }

typedef BOOL(PASCAL FAR *LPFN_ACCEPTEX)(
    IN SOCKET sListenSocket, IN SOCKET sAcceptSocket, IN PVOID lpOutputBuffer,
    IN DWORD dwReceiveDataLength, IN DWORD dwLocalAddressLength,
    IN DWORD dwRemoteAddressLength, OUT LPDWORD lpdwBytesReceived,
    IN LPOVERLAPPED lpOverlapped);

#endif

// GetAcceptExSockaddrs
#ifndef WSAID_GETACCEPTEXSOCKADDRS

#define WSAID_GETACCEPTEXSOCKADDRS                   \
  {                                                  \
    0xb5367df2, 0xcbac, 0x11cf, {                    \
      0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92 \
    }                                                \
  }

typedef VOID(PASCAL FAR *LPFN_GETACCEPTEXSOCKADDRS)(
    IN PVOID lpOutputBuffer, IN DWORD dwReceiveDataLength,
    IN DWORD dwLocalAddressLength, IN DWORD dwRemoteAddressLength,
    OUT struct sockaddr **LocalSockaddr, OUT LPINT LocalSockaddrLength,
    OUT struct sockaddr **RemoteSockaddr, OUT LPINT RemoteSockaddrLength);

#endif

// ConnectEx
#ifndef WSAID_CONNECTEX

#define WSAID_CONNECTEX                              \
  {                                                  \
    0x25a207b9, 0xddf3, 0x4660, {                    \
      0x8e, 0xe9, 0x76, 0xe5, 0x8c, 0x74, 0x06, 0x3e \
    }                                                \
  }

typedef BOOL(PASCAL FAR *LPFN_CONNECTEX)(
    IN SOCKET s, IN const struct sockaddr FAR *name, IN int namelen,
    IN PVOID lpSendBuffer OPTIONAL, IN DWORD dwSendDataLength,
    OUT LPDWORD lpdwBytesSent, IN LPOVERLAPPED lpOverlapped);

#endif

// DisconnectEx
#ifndef WSAID_DISCONNECTEX

#define WSAID_DISCONNECTEX                           \
  {                                                  \
    0x7fda2e11, 0x8630, 0x436f, {                    \
      0xa0, 0x31, 0xf5, 0x36, 0xa6, 0xee, 0xc1, 0x57 \
    }                                                \
  }

typedef BOOL(PASCAL FAR *LPFN_DISCONNECTEX)(IN SOCKET s,
                                            IN LPOVERLAPPED lpOverlapped,
                                            IN DWORD dwFlags,
                                            IN DWORD dwReserved);

#endif

struct WsaFuncs {
  LPFN_ACCEPTEX acceptEx = nullptr;
  LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockaddrs = nullptr;
  LPFN_CONNECTEX connectEx = nullptr;
  LPFN_DISCONNECTEX disconnectEx = nullptr;
};

LIBFF_API WsaFuncs GetWsaFuncs();

NS_FF_END

#endif  // _WSAHELPER_H
