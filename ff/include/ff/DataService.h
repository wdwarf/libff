/**
 * @file DataService.h
 * @auth DuckyLi
 * @date 2022-07-11 12:02:45
 * @description
 */

#ifndef _FF_DATASERVICE_H_
#define _FF_DATASERVICE_H_

#include <ff/Buffer.h>
#include <ff/TcpConnection.h>
#include <ff/Variant.h>
#include <ff/ff_config.h>
#include <ff/thirdparty/json/json.h>

#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

NS_FF_BEG

enum class DataValueType : int8_t {
  Unknown = -1,
  Boolean,
  Int8,
  Int16,
  Int32,
  Int64,
  Uint8,
  Uint16,
  Uint32,
  Uint64,
  String,
  ByteArray
};

std::pair<DataValueType, uint32_t> Str2ValueType(const std::string& typeName);

struct LIBFF_API DataMember {
  std::string name;
  ff::Variant value;
};

struct LIBFF_API DataEntity {
  std::string name;
  bool isSingle;
  bool isTemp;
  std::vector<DataMember> members;
};

struct LIBFF_API DataMemberInfo {
  std::string name;
  DataValueType valueType;
  uint32_t valueLength;
  bool isPK;
  std::string defaultValue;
  std::string validationRules;
};

struct LIBFF_API DataEntityInfo {
  std::string name;
  bool isSingle;
  bool isTemp;
  std::vector<DataMemberInfo> members;
  std::vector<std::vector<DataMember>> defaultRows;
};

class LIBFF_API IDataEntityInfoLoader {
 public:
  virtual bool load(std::vector<DataEntityInfo>& entities) = 0;
};

class LIBFF_API JsonDataEntityLoader : public IDataEntityInfoLoader {
 protected:
  bool parse(Json::Value& root, std::vector<DataEntityInfo>& entities);
};

class LIBFF_API JsonFileDataEntityLoader : public JsonDataEntityLoader {
 public:
  JsonFileDataEntityLoader(const std::string& cfgFilePath);
  bool load(std::vector<DataEntityInfo>& entities) override;

 private:
  std::string m_cfgFilePath;
};

class LIBFF_API JsonStrDataEntityLoader : public JsonDataEntityLoader {
 public:
  JsonStrDataEntityLoader(const std::string& jsonStr);
  bool load(std::vector<DataEntityInfo>& entities) override;

 private:
  std::string m_jsonStr;
};

#define DS_PKG_MAGIC 0x6666

#pragma pack(push, 1)

STRUCT_DEF_BEGIN(DsPkgHeader)
MEMBER_DEF_U16(magic);
MEMBER_DEF_U32(length);
MEMBER_DEF_U32(id);
MEMBER_DEF_U16(frameType);
MEMBER_DEF_U16(opCode);
MEMBER_DEF_U16(option);
MEMBER_DEF_U16(dataHash);
STRUCT_DEF_END

#pragma pack(pop)

enum DsFrameType : uint16_t { Req = 0x0000, Rsp = 0x0001 };

class LIBFF_API DataServicePacket {
 public:
  static uint16_t HashData(const void* data, uint32_t size);

  const DsPkgHeader* header() const;

  bool empty() const;
  bool parse(const void* buf, uint32_t size);
  bool generate(uint32_t id, uint16_t frameType, uint16_t opCode,
                uint16_t option, const void* data, uint32_t dataLength);
  const ff::Buffer& buffer() const;
  ff::Buffer& buffer();
  const uint8_t* data() const;
  uint32_t dataSize() const;

 private:
  ff::Buffer m_buffer;
};

using DsPacketPtr = std::shared_ptr<DataServicePacket>;

class LIBFF_API DsPacketHelper {
 public:
  DsPacketHelper& append(const void* data, uint32_t len);
  DsPacketPtr getPackage();

 private:
  ff::Buffer m_buffer;
  std::mutex m_mutex;

  static int32_t FindNextMagic(const uint8_t* data, uint32_t len);
};

class LIBFF_API DataServiceServer {
 public:
  DataServiceServer();
  ~DataServiceServer();

  bool start();

 private:
  ff::TcpConnectionPtr m_conn;
};

class DataServiceClient {
 public:
  DataServiceClient();
  ~DataServiceClient();

  bool start();

 private:
  ff::TcpConnectionPtr m_conn;
};

NS_FF_END

#endif /** _FF_DATASERVICE_H_ */
