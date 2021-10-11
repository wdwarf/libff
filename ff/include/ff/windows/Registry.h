/**
 * @file Registry.h
 * @auth DuckyLi
 * @date 2021-10-11 14:56:09
 * @description
 */

#ifndef _FF_WINDOWS_REGISTER_H_
#define _FF_WINDOWS_REGISTER_H_

#include <winsock2.h>
#include <Windows.h>
#include <ff/Buffer.h>
#include <ff/ff_config.h>

#include <list>
#include <memory>
#include <string>
#include <vector>

NS_FF_BEG

class LIBFF_API RegistryValue {
 public:
  RegistryValue(const std::string& name, const char* data, uint32_t dataLen,
                uint32_t dataType);
  RegistryValue(const RegistryValue& rv);
  ~RegistryValue();

  void operator=(const RegistryValue& rv);
  const std::string& getName();
  const char* getData();
  uint32_t getDataLen();
  uint32_t getDataType();

  std::string asString();
  uint32_t asLong();
  operator bool();
  bool operator<(RegistryValue& rv);

 private:
  std::string _name;
  ff::Buffer _data;
  uint32_t _dataType;

  void clear();
  void setValue(const std::string& name, const char* data, uint32_t dataLen,
                uint32_t dataType);
};

class LIBFF_API Registry {
 public:
  Registry(HKEY key = 0);
  ~Registry(void);

  bool open(HKEY hKey, const std::string& subKey = "",
            REGSAM samDesired = KEY_ALL_ACCESS);
  bool isOpen();
  bool close();
  std::list<std::string> enumKeys();
  std::list<RegistryValue> enumValues();
  std::list<RegistryValue> enumStringValues();
  bool getValue(const std::string& subKey, const std::string& valueName,
                LPDWORD pType, void* pDataBuf, LPDWORD pBufLen,
                uint32_t flags = RRF_RT_REG_SZ);

  bool setKeyValue(const std::string& keyName, const char* data,
                   uint32_t dataLen, uint32_t dataType = REG_SZ,
                   const std::string& subKey = "");

  bool setValue(const std::string& keyName, const char* data, uint32_t dataLen,
                uint32_t dataType = REG_SZ);

  bool deleteValue(const std::string& keyName,
                   uint32_t dataType = KEY_WOW64_32KEY);

  std::shared_ptr<Registry> createKey(
      const std::string& subKey, uint32_t reserved = 0,
      const std::string& strClass = "",
      uint32_t options = REG_OPTION_NON_VOLATILE,
      REGSAM samDesired = KEY_ALL_ACCESS,
      _In_opt_ CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL,
      _Out_opt_ LPDWORD lpdwDisposition = NULL);

 protected:
  HKEY key;
};

NS_FF_END

#endif
