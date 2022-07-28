/**
 * @file DataService.h
 * @auth DuckyLi
 * @date 2022-07-11 12:02:45
 * @description
 */

#ifndef _FF_DATASERVICE_H_
#define _FF_DATASERVICE_H_

#include <ff/Buffer.h>
#include <ff/Synchronizable.h>
#include <ff/TcpConnection.h>
#include <ff/Variant.h>
#include <ff/ff_config.h>
#include <ff/thirdparty/json/json.h>

#include <cstdint>
#include <future>
#include <list>
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

enum class DsOpCode : uint16_t {
  Heartbeat = 0,
  DataEntityInfo,
  DataEntityNames,
  DataQuery,
  DataAdd,
  DataMd,
  DataDel
};

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

std::pair<DataValueType, uint32_t> Str2ValueType(const std::string& typeName);
std::string DataEntities2Json(const std::vector<DataEntityInfo>& entities);

class LIBFF_API IDataEntityInfoLoader {
 public:
  virtual bool load(std::vector<DataEntityInfo>& entities) = 0;
};

using DataEntityInfoLoaderPtr = std::shared_ptr<IDataEntityInfoLoader>;

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

/**
 * @brief DsPkgHeader
 *
 */
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

/**
 * @brief DataServicePacket
 *
 */
class LIBFF_API DataServicePacket {
 public:
  static uint16_t HashData(const void* data, uint32_t size);

  DataServicePacket() = default;
  DataServicePacket(const DataServicePacket& pkg) = default;
  DataServicePacket(DataServicePacket&& pkg) {
    this->m_buffer = std::move(pkg.m_buffer);
  }

  const DsPkgHeader* header() const;

  bool empty() const;
  bool parse(const void* buf, uint32_t size);
  bool generate(uint32_t id, DsFrameType frameType, DsOpCode opCode,
                uint16_t option, const void* data, uint32_t dataLength);
  const ff::Buffer& buffer() const;
  ff::Buffer& buffer();
  const uint8_t* data() const;
  uint32_t dataSize() const;

 private:
  ff::Buffer m_buffer;
};

using DsPacketPtr = std::shared_ptr<DataServicePacket>;

/**
 * @brief DsPacketHelper
 *
 */
class LIBFF_API DsPacketHelper {
 public:
  DsPacketHelper& append(const void* data, uint32_t len);
  DsPacketPtr getPackage();

 private:
  ff::Buffer m_buffer;
  std::mutex m_mutex;

  static int32_t FindNextMagic(const uint8_t* data, uint32_t len);
};

class IDataServiceSource {
  virtual DataEntity get(const std::string& entityName) = 0;
};

using IDataServiceSourcePtr = std::shared_ptr<IDataServiceSource>;

using DsPkgPromiseRemoveFunc = std::function<void(uint32_t msgId)>;
class LIBFF_API DsPkgPromise : public Synchronizable {
 public:
  DsPkgPromise(uint32_t msgId, DsPkgPromiseRemoveFunc func);
  ~DsPkgPromise();

  DsPacketPtr get(uint32_t ms);
  void set(DsPacketPtr pkg);

 private:
  uint32_t m_id;
  DsPkgPromiseRemoveFunc m_func;
  std::promise<DsPacketPtr> m_promise;
  std::future<DsPacketPtr> m_future;
};

using DsPkgPromisePtr = std::shared_ptr<DsPkgPromise>;

/**
 * @brief DataServiceServer
 *
 */
class LIBFF_API DataServiceServer {
 public:
  DataServiceServer();
  ~DataServiceServer();

  bool start();
  bool stop();
  void setDataEntityInfoLoader(DataEntityInfoLoaderPtr loader) {
    this->m_dataEntityInfoLoader = loader;
    this->m_dataEntityInfoLoader->load(this->m_dataEntities);
  }

 private:
  DataEntityInfoLoaderPtr m_dataEntityInfoLoader;
  std::vector<DataEntityInfo> m_dataEntities;
  ff::TcpConnectionPtr m_conn;
  std::list<ff::TcpConnectionPtr> m_clients;
  DsPacketHelper m_pkgHelper;
  std::mutex m_mutexClients;
  IDataServiceSourcePtr m_dataSource;

  void onAccept(const TcpConnectionPtr& client);
  void handleReq(const DataServicePacket& pkg, const TcpConnectionPtr& client);
};

/**
 * @brief DataServiceClient
 *
 */
class LIBFF_API DataServiceClient {
 public:
  DataServiceClient();
  ~DataServiceClient();

  bool start();
  bool stop();

  std::vector<std::string> getDataEntityNames();
  DataEntity get(const std::string& entityName);

 private:
  ff::TcpConnectionPtr m_conn;
  DsPacketHelper m_pkgHelper;
  std::vector<DataEntityInfo> m_rdataEntities;
  std::map<uint32_t, DsPkgPromise*> m_pkgId2Promise;
  std::mutex m_mutexPkgId2Promise;

  void removePromise(uint32_t pkgId);

  DsPkgPromisePtr send(const DataServicePacket& pkg);
  void handleRsp(const DataServicePacket& pkg);
};

NS_FF_END

#endif /** _FF_DATASERVICE_H_ */
