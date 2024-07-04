/**
 * @file WsaHelper.cpp
 * @author liyawu
 * @date 2024-07-04 09:47:58
 * @description
 */
#include <ff/windows/WsaHelper.h>
#include <mswsock.h>
#include <windows.h>

using namespace std;

NS_FF_BEG

namespace {

static GUID acceptex_guid = WSAID_ACCEPTEX;
static GUID getacceptexsockaddrs_guid = WSAID_GETACCEPTEXSOCKADDRS;
static GUID connectex_guid = WSAID_CONNECTEX;
static GUID disconnectex_guid = WSAID_DISCONNECTEX;

}  // namespace

WsaFuncs GetWsaFuncs() {
  WsaFuncs funcs;

  DWORD bytes;
  auto s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (s == -1) {
    return funcs;
  }

  WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &acceptex_guid, sizeof(GUID),
           &funcs.acceptEx, sizeof(LPFN_ACCEPTEX), &bytes, NULL, NULL);

  WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &getacceptexsockaddrs_guid,
           sizeof(GUID), &funcs.getAcceptExSockaddrs,
           sizeof(LPFN_GETACCEPTEXSOCKADDRS), &bytes, NULL, NULL);

  WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &connectex_guid, sizeof(GUID),
           &funcs.connectEx, sizeof(LPFN_CONNECTEX), &bytes, NULL, NULL);

  WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &disconnectex_guid,
           sizeof(GUID), &funcs.disconnectEx, sizeof(LPFN_DISCONNECTEX), &bytes,
           NULL, NULL);

  closesocket(s);

  return funcs;
}

NS_FF_END