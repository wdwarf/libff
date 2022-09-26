/**
 * @file NetworkDevices.h
 * @auth DuckyLi
 * @date 2022-09-22 16:48:16
 * @description 
 */

#ifndef _FF_NETWORKDEVICES_H_
#define _FF_NETWORKDEVICES_H_

#include <vector>
#include <string>

#include <ff/ff_config.h>

NS_FF_BEG

enum class IpType : uint8_t{
  V4,
  V6
};

struct NetworkDeviceIP{
  std::string ip;
  IpType type;
  std::string mask;
  std::vector<std::string> gateways;
};

struct NetworkDevice{
  std::string name;
  std::string desc;
  std::string mac;
  std::vector<NetworkDeviceIP> ips;
};

class LIBFF_API NetworkDevices
{
public:
  static std::vector<NetworkDevice> ListDevices();

private:
  NetworkDevices();
  ~NetworkDevices();
};

NS_FF_END

#endif /** _FF_NETWORKDEVICES_H_ */
