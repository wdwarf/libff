/**
 * @file NetworkDevices.cpp
 * @auth DuckyLi
 * @date 2022-09-22 16:49:33
 * @description
 */
#include <ff/NetworkDevices.h>
#include <ff/String.h>

#include <sstream>

#ifdef _WIN32
#include <IPHlpApi.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "IPHlpApi.lib")
#pragma comment(lib, "ws2_32.lib")

#else

#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

using namespace std;

NS_FF_BEG

#ifdef _WIN32

static std::string GetMacStr(const uint8_t* addr, uint32_t addrLen) {
  stringstream str;
  for (uint32_t i = 0; i < addrLen; ++i) {
    str.width(2);
    str.fill('0');
    str << hex << uppercase << (uint16_t)addr[i];
    if (i + 1 < addrLen) str << ":";
  }
  return str.str();
}

static void GetGwMask(const NetworkDevice& dev, NetworkDeviceIP& ip) {
  PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO;

  ULONG ulSize = sizeof(IP_ADAPTER_INFO);

  ULONG ulRet = GetAdaptersInfo(pIpAdapterInfo, &ulSize);

  if (ERROR_BUFFER_OVERFLOW == ulRet) {
    delete pIpAdapterInfo;

    pIpAdapterInfo = (PIP_ADAPTER_INFO) new BYTE[ulSize];

    ulRet = GetAdaptersInfo(pIpAdapterInfo, &ulSize);

    if (ERROR_SUCCESS != ulRet) {
      delete pIpAdapterInfo;
      return;
    }
  }

  for (PIP_ADAPTER_INFO pIt = pIpAdapterInfo; NULL != pIt; pIt = pIt->Next) {
    if (dev.name != pIt->AdapterName) continue;

    for (PIP_ADDR_STRING pIpAddr = &pIt->IpAddressList; NULL != pIpAddr;
         pIpAddr = pIpAddr->Next) {
      ip.mask = pIpAddr->IpMask.String;

      for (PIP_ADDR_STRING pGateAwayList = &pIt->GatewayList;
           NULL != pGateAwayList; pGateAwayList = pGateAwayList->Next) {
        ip.gateways.push_back(pGateAwayList->IpAddress.String);
      }
    }
  }

  if (pIpAdapterInfo) {
    delete pIpAdapterInfo;
  }
}

std::vector<NetworkDevice> NetworkDevices::ListDevices() {
  std::vector<NetworkDevice> devices;

  ULONG flags = GAA_FLAG_INCLUDE_PREFIX |
                GAA_FLAG_INCLUDE_GATEWAYS; /** 包括 IPV4 ，IPV6 网关 */
  ULONG family = AF_UNSPEC; /** 返回包括 IPV4 和 IPV6 地址 */
  PIP_ADAPTER_ADDRESSES pAddresses = NULL;
  ULONG bufLen = sizeof(IP_ADAPTER_ADDRESSES);
  ULONG ret = 0;
  do {
    pAddresses = (IP_ADAPTER_ADDRESSES*)new uint8_t[bufLen];
    if (pAddresses == NULL) return devices;
    ret = GetAdaptersAddresses(family, flags, NULL, pAddresses, &bufLen);
    if (ERROR_BUFFER_OVERFLOW == ret) {
      delete[] pAddresses;
      pAddresses = NULL;
    }
  } while (ERROR_BUFFER_OVERFLOW == ret);

  if (NO_ERROR != ret) return devices;

  for (PIP_ADAPTER_ADDRESSES pCurrAddrs = pAddresses; NULL != pCurrAddrs;
       pCurrAddrs = pCurrAddrs->Next) {
    if (1 != pCurrAddrs->OperStatus) continue;

    // Currently not supported interfaces that not support multicast.
    if (pCurrAddrs->Flags & 0x0010) {
      continue;
    }

    NetworkDevice dev;
    dev.name = pCurrAddrs->AdapterName;
    dev.desc = ToMbs(pCurrAddrs->Description);
    dev.mac = GetMacStr(pCurrAddrs->PhysicalAddress,
                        pCurrAddrs->PhysicalAddressLength);

    char buf[BUFSIZ];
    for (auto ua = pCurrAddrs->FirstUnicastAddress; NULL != ua; ua = ua->Next) {
      int family = ua->Address.lpSockaddr->sa_family;
      if (AF_INET != family && AF_INET6 != family) continue;

      memset(buf, 0, BUFSIZ);
      getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf,
                  sizeof(buf), NULL, 0, NI_NUMERICHOST);
      NetworkDeviceIP ip;
      ip.ip = buf;
      ip.type = AF_INET == family ? IpType::V4 : IpType::V6;
      if (AF_INET == family) GetGwMask(dev, ip);

      dev.ips.push_back(ip);
    }

    devices.push_back(dev);
  }

  delete[] pAddresses;
  return devices;
}

#else

static string _getMAC(const char *ifname) {
  struct ifreq ifreq; /* interface request */
  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sfd) return "";
  strncpy(ifreq.ifr_name, ifname, sizeof(ifreq.ifr_name));
  /* Get hardware address	*/
  if (-1 == ioctl(sfd, SIOCGIFHWADDR, &ifreq)) {
    close(sfd);
    return "";
  }
  uint8_t *mac = (uint8_t *)ifreq.ifr_hwaddr.sa_data;
  char macStr[32] = {0};
  sprintf(macStr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2],
          mac[3], mac[4], mac[5]);
  return macStr;
}

std::vector<NetworkDevice> NetworkDevices::ListDevices() {
  std::vector<NetworkDevice> devices;

  struct ifaddrs *ifaddr, *ifa;
  int family, s;
  char host[NI_MAXHOST];

  if (getifaddrs(&ifaddr) == -1) {
    return devices;
  }

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL || (ifa->ifa_flags & IFF_RUNNING) == 0) {
      continue;
    }

    NetworkDeviceIP devIp;

    family = ifa->ifa_addr->sa_family;
    devIp.type = AF_INET == family ? IpType::V4 : IpType::V6;

    NetworkDevice *pDev = nullptr;
    for (auto &d : devices) {
      if (d.name == ifa->ifa_name) {
        pDev = &d;
        break;
      }
    }

    if (nullptr == pDev) {
      NetworkDevice dev;
      dev.name = ifa->ifa_name;
      dev.desc = ifa->ifa_name;
      dev.mac = _getMAC(ifa->ifa_name);
      devices.push_back(dev);
    }

    for (auto &d : devices) {
      if (d.name == ifa->ifa_name) {
        pDev = &d;
        break;
      }
    }

    if (family == AF_INET) {
      s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
                      NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if (s != 0) {
        continue;
      }
      devIp.ip = host;
      s = getnameinfo(ifa->ifa_netmask, sizeof(struct sockaddr_in), host,
                      NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if (s != 0) {
        continue;
      }
      devIp.mask = host;

      pDev->ips.push_back(devIp);

    } else if (family == AF_INET6) {
      s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6), host,
                      NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if (s != 0) {
        continue;
      }
      
      devIp.ip = host;
      s = getnameinfo(ifa->ifa_netmask, sizeof(struct sockaddr_in6), host,
                      NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if (s != 0) {
        continue;
      }
      devIp.mask = host;

      pDev->ips.push_back(devIp);
    }
  }

  freeifaddrs(ifaddr);
  return devices;
}
#endif

NS_FF_END
