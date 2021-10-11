/**
 * @file Registry.cpp
 * @auth DuckyLi
 * @date 2021-10-11 14:56:54
 * @description
 */
#include <ff/windows/Registry.h>

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// #pragma comment(lib, "Advapi32.lib")

NS_FF_BEG

//-------------------------------------------------------------

RegistryValue::RegistryValue(const std::string& name, const char* data,
                             uint32_t dataLen, uint32_t dataType) {
  this->setValue(name, data, dataLen, dataType);
}

//-------------------------------------------------------------

RegistryValue::~RegistryValue() {}

//-------------------------------------------------------------

RegistryValue::RegistryValue(const RegistryValue& rv) {
  this->_data.clear();
  this->setValue(rv._name, (const char*)rv._data.getData(), rv._data.getSize(),
                 rv._dataType);
}

//-------------------------------------------------------------

void RegistryValue::operator=(const RegistryValue& rv) {
  this->setValue(rv._name, (const char*)rv._data.getData(), rv._data.getSize(),
                 rv._dataType);
}

//-------------------------------------------------------------

bool RegistryValue::operator<(RegistryValue& rv) {
  return this->_name < rv._name;
}

//-------------------------------------------------------------

void RegistryValue::clear() {
  this->_name = "";
  this->_dataType = 0;
  this->_data.clear();
}

//-------------------------------------------------------------

void RegistryValue::setValue(const std::string& name, const char* data,
                             uint32_t dataLen, uint32_t dataType) {
  this->clear();
  this->_name = name;
  this->_dataType = dataType;
  this->_data.setData(data, dataLen);
}

//-------------------------------------------------------------

const std::string& RegistryValue::getName() { return this->_name; }

//-------------------------------------------------------------

const char* RegistryValue::getData() {
  return (const char*)this->_data.getData();
}

//-------------------------------------------------------------

uint32_t RegistryValue::getDataLen() { return this->_data.getSize(); }

//-------------------------------------------------------------

uint32_t RegistryValue::getDataType() { return this->_dataType; }

//-------------------------------------------------------------

RegistryValue::operator bool() { return !this->_data.isEmpty(); }

//-------------------------------------------------------------

std::string RegistryValue::asString() {
  std::string re;
  if (!this->_data.isEmpty()) {
    switch (this->_dataType) {
      case REG_SZ:
      case REG_EXPAND_SZ: {
        re = (const char*)this->_data.getData();
      } break;
      case REG_DWORD:
      case REG_DWORD_BIG_ENDIAN: {
        uint32_t dRe = 0;
        if (sizeof(uint32_t) >= this->_data.getSize()) {
          memcpy(&dRe, this->_data.getData(), this->_data.getSize());
        }
        stringstream str;
        str << dRe;
        re = str.str();
      } break;
      case REG_BINARY: {
        stringstream str;
        for (int i = 0; i < this->_data.getSize(); ++i) {
          unsigned short n = 0;
          memcpy(&n, this->_data.getData() + i, 1);
          str.width(2);
          str.fill('0');
          str << hex;
          str << n << " ";
        }
        re = str.str();
      } break;
    }
  }
  return re;
}

//-------------------------------------------------------------

uint32_t RegistryValue::asLong() {
  uint32_t re = 0;
  if (!this->_data.isEmpty()) {
    switch (this->_dataType) {
      case REG_SZ:
      case REG_EXPAND_SZ: {
        stringstream str;
        str << this->_data.getData();
        str >> re;
      } break;
      case REG_DWORD:
      case REG_DWORD_BIG_ENDIAN: {
        if (sizeof(uint32_t) >= this->_data.getSize()) {
          memcpy(&re, this->_data.getData(),
                 this->_data.getSize() * sizeof(char));
        }
      } break;
      case REG_BINARY: {
        if (this->_data.getSize() <= sizeof(uint32_t)) {
          memcpy(&re, this->_data.getData(),
                 this->_data.getSize() * sizeof(char));
        }
      } break;
    }
  }
  return re;
}

//-------------------------------------------------------------

Registry::Registry(HKEY key) { this->key = key; }

//-------------------------------------------------------------

Registry::~Registry(void) { this->close(); }

//-------------------------------------------------------------

bool Registry::open(HKEY hKey, const std::string& subKey, REGSAM samDesired) {
  this->close();
  return (ERROR_SUCCESS ==
          ::RegOpenKeyEx(hKey, subKey.c_str(), 0, samDesired, &this->key));
}

//-------------------------------------------------------------

bool Registry::isOpen() { return (0 != this->key); }

//-------------------------------------------------------------

bool Registry::close() {
  if (ERROR_SUCCESS == ::RegCloseKey(this->key)) {
    this->key = 0;
    return true;
  }
  return false;
}

//-------------------------------------------------------------

bool Registry::getValue(const std::string& subKey, const std::string& valueName,
                        LPDWORD pType, void* pDataBuf, LPDWORD pBufLen,
                        uint32_t flags) {
  return (ERROR_SUCCESS == RegGetValue(this->key, subKey.c_str(),
                                       valueName.c_str(), flags, pType,
                                       pDataBuf, pBufLen));
}

//-------------------------------------------------------------

list<std::string> Registry::enumKeys() {
  list<std::string> re;
  if (this->isOpen()) {
    uint32_t index = 0;
    uint32_t bufLen = 320;
    Buffer bufName(bufLen);
    while (ERROR_SUCCESS == ::RegEnumKeyA(this->key, index++,
                                          (char*)bufName.getData(), bufLen)) {
      re.push_back((const char*)bufName.getData());
      bufName.zero();
    }
  }
  re.sort();
  return re;
}

//-------------------------------------------------------------

list<RegistryValue> Registry::enumValues() {
  list<RegistryValue> result;
  if (this->isOpen()) {
    uint32_t index = 0;
    uint32_t bufLen = 320;
    uint32_t dataBufLen = 1024;
    Buffer bufName(bufLen);
    Buffer bufData(dataBufLen);
    DWORD nameLen = bufLen;
    DWORD dataLen = dataBufLen;
    DWORD type = 0;
    bool isHasData = true;
    while (isHasData) {
      nameLen = bufLen;
      dataLen = dataBufLen;
      bufName.zero();
      bufData.zero();

      uint32_t re =
          ::RegEnumValueA(this->key, index, (char*)bufName.getData(), &nameLen,
                          0, &type, bufData.getData(), &dataLen);

      switch (re) {
        case ERROR_SUCCESS: {
          RegistryValue rv((const char*)bufName.getData(),
                           (const char*)bufData.getData(), dataLen, type);
          result.push_back(rv);
          ++index;
        } break;
        case ERROR_MORE_DATA: {
          dataBufLen = dataLen + 1;
          bufData.alloc(dataBufLen);
        } break;
        case ERROR_NO_DATA:
        case ERROR_NO_MORE_ITEMS: {
          isHasData = false;
        } break;
        default:
          ++index;
          break;
      }
    }
  }
  result.sort();
  return result;
}

//-------------------------------------------------------------

list<RegistryValue> Registry::enumStringValues() {
  list<RegistryValue> result;
  list<RegistryValue> valueList = this->enumValues();
  for (list<RegistryValue>::iterator it = valueList.begin();
       it != valueList.end(); ++it) {
    if (REG_SZ == it->getDataType() || REG_EXPAND_SZ == it->getDataType()) {
      result.push_back(*it);
    }
  }
  return result;
}

//-------------------------------------------------------------

bool Registry::setKeyValue(const std::string& keyName, const char* data,
                           uint32_t dataLen, uint32_t dataType,
                           const std::string& subKey) {
  return (ERROR_SUCCESS == RegSetKeyValue(this->key, subKey.c_str(),
                                          keyName.c_str(), dataType, data,
                                          dataLen));
}

bool Registry::setValue(const std::string& keyName, const char* data,
                        uint32_t dataLen, uint32_t dataType) {
  return (ERROR_SUCCESS == RegSetValueEx(this->key, keyName.c_str(), 0,
                                         dataType, (BYTE*)data, dataLen));
  // return RegSetValue(this->key, keyName.c_str(), dataType, data, dataLen);
}

bool Registry::deleteValue(const std::string& keyName, uint32_t dataType) {
  return (ERROR_SUCCESS == RegDeleteValue(this->key, keyName.c_str()));
  // return RegDeleteKeyEx(this->key, keyName.c_str(), dataType, 0);
}

shared_ptr<Registry> Registry::createKey(
    const std::string& subKey, uint32_t reserved, const std::string& strClass,
    uint32_t options, REGSAM samDesired,
    _In_opt_ CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _Out_opt_ LPDWORD lpdwDisposition) {
  shared_ptr<Registry> re = make_shared<Registry>((HKEY)NULL);
  HKEY newKey = 0;
  DWORD disposition = 0;
  long ec = RegCreateKeyExA(this->key, subKey.c_str(), reserved,
                            (char*)strClass.c_str(), options, samDesired,
                            lpSecurityAttributes, &newKey, &disposition);
  if (ERROR_SUCCESS == ec) {
    re = make_shared<Registry>(newKey);
  }
  return re;
}

NS_FF_END
