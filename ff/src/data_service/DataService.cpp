/**
 * @file DataService.cpp
 * @auth DuckyLi
 * @date 2022-07-11 12:03:09
 * @description
 */
#include <ff/Bind.h>
#include <ff/CRC.h>
#include <ff/DataService.h>
#include <ff/String.h>
#include <ff/Variant.h>
#include <ff/thirdparty/json/json.h>

#include <fstream>
#include <iostream>

using namespace std;

NS_FF_BEG

namespace {

ff::Variant JsonValue(Json::Value& node) {
  if (node.isNull()) return Variant();
  if (node.isString()) return node.asString();
  if (node.isBool()) return node.asBool();
  if (node.isDouble()) return node.asDouble();
  if (node.isInt64()) return node.asInt64();
  if (node.isInt()) return node.asInt();
  if (node.isUInt64()) return node.asUInt64();
  if (node.isUInt()) return node.asUInt();

  return node.toStyledString();
}

}  // namespace

uint32_t ParseTypeLength(const std::string& typeName) {
  auto posStart = typeName.find_first_of("[");
  auto posEnd = typeName.find_last_of("]");
  if (string::npos == posStart || string::npos == posEnd) return 0;

  return Variant(typeName.substr(posStart + 1, posEnd - posStart - 1));
}

std::pair<DataValueType, uint32_t> Str2ValueType(const std::string& typeName) {
  auto name = TrimCopy(ToLowerCopy(typeName));

  return StartsWith(name, "string[")
             ? make_pair(DataValueType::String, ParseTypeLength(name))
         : StartsWith(name, "byte[")
             ? make_pair(DataValueType::ByteArray, ParseTypeLength(name))
         : ("bool" == name || "boolean" == name)
             ? make_pair(DataValueType::Boolean, 1u)
         : "int8" == name   ? make_pair(DataValueType::Int8, 1u)
         : "int16" == name  ? make_pair(DataValueType::Int16, 1u)
         : "int32" == name  ? make_pair(DataValueType::Int32, 1u)
         : "int64" == name  ? make_pair(DataValueType::Int64, 1u)
         : "uint8" == name  ? make_pair(DataValueType::Uint8, 1u)
         : "uint16" == name ? make_pair(DataValueType::Uint16, 1u)
         : "uint32" == name ? make_pair(DataValueType::Uint32, 1u)
         : "uint64" == name ? make_pair(DataValueType::Uint64, 1u)
                            : make_pair(DataValueType::Unknown, 0u);
}

static string DataMemberType(const DataMemberInfo& m) {
  return DataValueType::Int8 == m.valueType      ? "int8"
         : DataValueType::Int16 == m.valueType   ? "int16"
         : DataValueType::Int32 == m.valueType   ? "int32"
         : DataValueType::Int64 == m.valueType   ? "int64"
         : DataValueType::Uint8 == m.valueType   ? "uint8"
         : DataValueType::Uint16 == m.valueType  ? "uint16"
         : DataValueType::Uint32 == m.valueType  ? "uint32"
         : DataValueType::Uint64 == m.valueType  ? "uint64"
         : DataValueType::Boolean == m.valueType ? "bool"
         : DataValueType::String == m.valueType
             ? ("string[" + to_string(m.valueLength) + "]")
         : DataValueType::ByteArray == m.valueType
             ? ("byte[" + to_string(m.valueLength) + "]")
             : "";
}

std::string DataEntities2Json(const std::vector<DataEntityInfo>& entities) {
  Json::Value root;
  for (const DataEntityInfo& entity : entities) {
    Json::Value e;
    e["name"] = entity.name;
    e["isSingle"] = entity.isSingle;
    e["isTemp"] = entity.isTemp;

    Json::Value members;
    for (auto& m : entity.members) {
      Json::Value member;
      member["name"] = m.name;
      member["type"] = DataMemberType(m);
      member["isPK"] = m.isPK;
      member["defaultValue"] = m.defaultValue;
      member["validationRules"] = m.validationRules;
      members.insert(0, member);
    }

    if (!members.empty()) {
      e["members"] = members;
    }
    root.insert(0, e);
  }
  return root.toStyledString();
}

bool JsonDataEntityLoader::parse(Json::Value& root,
                                 std::vector<DataEntityInfo>& entities) {
  try {
    for (auto& e : root) {
      DataEntityInfo entity;
      entity.name = JsonValue(e["name"]).toString();
      entity.isSingle = JsonValue(e["isSingle"]);
      entity.isTemp = JsonValue(e["isTemp"]);

      auto members = e["members"];
      for (auto& m : members) {
        DataMemberInfo member;
        member.name = JsonValue(m["name"]).toString();
        auto type = Str2ValueType(m["type"].asString());
        member.valueType = type.first;
        member.valueLength = type.second;
        member.isPK = JsonValue(e["isPK"]);
        member.defaultValue = JsonValue(m["defaultValue"]).toString();
        member.validationRules = JsonValue(m["validationRules"]).toString();

        entity.members.push_back(member);
      }

      auto defaultRows = e["defaultRows"];
      for (auto& r : defaultRows) {
        std::vector<DataMember> row;
        for (auto& name : r.getMemberNames()) {
          row.push_back({name, r[name].asString()});
        }

        entity.defaultRows.push_back(row);
      }

      entities.push_back(entity);
    }
  } catch (...) {
    return false;
  }

  return true;
}

JsonFileDataEntityLoader::JsonFileDataEntityLoader(
    const std::string& cfgFilePath)
    : m_cfgFilePath(cfgFilePath) {}

bool JsonFileDataEntityLoader::load(std::vector<DataEntityInfo>& entities) {
  Json::CharReaderBuilder builder;
  ifstream in(this->m_cfgFilePath);
  Json::Value root;
  std::string errInfo;
  if (!Json::parseFromStream(builder, in, &root, &errInfo)) return false;

  return this->parse(root, entities);
}

JsonStrDataEntityLoader::JsonStrDataEntityLoader(const std::string& jsonStr)
    : m_jsonStr(jsonStr) {}

bool JsonStrDataEntityLoader::load(std::vector<DataEntityInfo>& entities) {
  Json::CharReaderBuilder builder;
  stringstream in;
  in << this->m_jsonStr;
  Json::Value root;
  std::string errInfo;
  if (!Json::parseFromStream(builder, in, &root, &errInfo)) return false;

  return this->parse(root, entities);
}

uint16_t DataServicePacket::HashData(const void* data, uint32_t size) {
  if (nullptr == data || 0 == size) return 0;

  CrcCalculator crc(CrcAlgorithm::Crc16Xmodem);
  return crc.calc(data, size);
}

bool DataServicePacket::empty() const { return this->m_buffer.isEmpty(); }

const DsPkgHeader* DataServicePacket::header() const {
  if (this->empty()) return nullptr;
  return (DsPkgHeader*)this->m_buffer.getData();
}

uint32_t DataServicePacket::dataSize() const {
  if (this->empty()) return 0;
  return this->header()->length() - DsPkgHeader::Size();
}

const uint8_t* DataServicePacket::data() const {
  if (this->empty()) return nullptr;
  if (0 == this->dataSize()) return nullptr;
  return (const uint8_t*)this->m_buffer.getData() + DsPkgHeader::Size();
}

const ff::Buffer& DataServicePacket::buffer() const { return this->m_buffer; }

ff::Buffer& DataServicePacket::buffer() { return this->m_buffer; }

bool DataServicePacket::parse(const void* buf, uint32_t size) {
  this->m_buffer.clear();
  if (nullptr == buf || size < DsPkgHeader::Size()) return false;

  DsPkgHeader* hdr = (DsPkgHeader*)buf;
  if (size < hdr->length()) return false;

  uint16_t hash = HashData((const char*)buf + DsPkgHeader::Size(),
                           hdr->length() - DsPkgHeader::Size());
  if (hdr->dataHash() != hash) return false;

  this->m_buffer.setData(buf, hdr->length());
  return true;
}

bool DataServicePacket::generate(uint32_t id, DsFrameType frameType,
                                 DsOpCode opCode, uint16_t option,
                                 const void* data, uint32_t dataLength) {
  this->m_buffer.alloc(DsPkgHeader::Size() + dataLength);
  DsPkgHeader* hdr = (DsPkgHeader*)this->m_buffer.getData();
  hdr->magic(DS_PKG_MAGIC);
  hdr->id(id);
  hdr->frameType(frameType);
  hdr->opCode(static_cast<uint16_t>(opCode));
  hdr->option(option);
  hdr->dataHash(HashData(data, dataLength));
  hdr->length(this->m_buffer.getSize());

  if (dataLength > 0) {
    uint8_t* p = (uint8_t*)this->m_buffer.getData() + DsPkgHeader::Size();
    memcpy(p, data, dataLength);
  }
  return true;
}

/**
 *
 * class DsPacketHelper
 *
 */

int32_t DsPacketHelper::FindNextMagic(const uint8_t* data, uint32_t len) {
  if (nullptr == data || 0 == len) return -1;

  for (uint32_t i = 0; i < len - 1; ++i) {
    DsPkgHeader* hdr = (DsPkgHeader*)(data + i);
    if (DS_PKG_MAGIC == hdr->magic()) return i;
  }
  return -1;
}

DsPacketHelper& DsPacketHelper::append(const void* data, uint32_t len) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_buffer.append(data, len);
  return *this;
}

DsPacketPtr DsPacketHelper::getPackage() {
  lock_guard<mutex> lk(this->m_mutex);
  if (this->m_buffer.getSize() < DsPkgHeader::Size()) return nullptr;

  auto pos = FindNextMagic(this->m_buffer.getData(), this->m_buffer.getSize());
  if (pos < 0) {
    this->m_buffer.clear();
    return nullptr;
  }

  const uint8_t* pData = (const uint8_t*)this->m_buffer.getData() + pos;
  const DsPkgHeader* hdr = (const DsPkgHeader*)pData;
  auto length = hdr->length();

  if ((this->m_buffer.getSize() - pos) < length) return nullptr;
  if (hdr->dataHash() !=
      DataServicePacket::HashData(pData + DsPkgHeader::Size(),
                                  length - DsPkgHeader::Size())) {
    this->m_buffer.setData(this->m_buffer.getData() + pos + 2,
                           this->m_buffer.getSize() - pos - 2);

    return nullptr;
  }

  DsPacketPtr pkg = make_shared<DataServicePacket>();
  pkg->buffer().setData(pData, length);

  this->m_buffer.setData(pData + length,
                         this->m_buffer.getSize() - length - pos);

  return pkg;
}
/**
 * @brief DsPkgPromise
 *
 */

DsPkgPromise::DsPkgPromise(uint32_t id, DsPkgPromiseRemoveFunc func)
    : m_id(id), m_func(func) {
  this->m_future = this->m_promise.get_future();
}

DsPkgPromise::~DsPkgPromise() {
  this->lock();
  this->m_func(m_id);
  this->unlock();
}

DsPacketPtr DsPkgPromise::get(uint32_t ms) {
  if (future_status::ready ==
      this->m_future.wait_for(chrono::milliseconds(ms))) {
    return this->m_future.get();
  }
  return nullptr;
}

void DsPkgPromise::set(DsPacketPtr pkg) {
  this->lock();
  this->m_promise.set_value(pkg);
  this->unlock();
}

/**
 * @brief class DataServiceServer
 *
 */
DataServiceServer::DataServiceServer(/* args */)
    : m_conn(TcpConnection::CreateInstance()) {}

DataServiceServer::~DataServiceServer() {}

bool DataServiceServer::start() {
  this->m_conn->listen(5300);

  this->m_conn->onAccept(Bind(&DataServiceServer::onAccept, this));
  return true;
}

void DataServiceServer::onAccept(const TcpConnectionPtr& client) {
  std::lock_guard<std::mutex> lk(this->m_mutexClients);
  this->m_clients.push_back(client);

  client->onClose([this](const TcpConnectionPtr& client) {
    std::lock_guard<std::mutex> lk(this->m_mutexClients);
    this->m_clients.remove(client);
  });

  client->onData(
      [this](const uint8_t* data, uint32_t size, const TcpConnectionPtr& conn) {
        this->m_pkgHelper.append(data, size);
        auto pkg = this->m_pkgHelper.getPackage();
        if (!pkg) return;

        this->handleReq(*pkg, conn);
      });
}

bool DataServiceServer::stop() { return true; }

void DataServiceServer::handleReq(const DataServicePacket& pkg,
                                  const TcpConnectionPtr& client) {
  auto hdr = pkg.header();
  cout << "hdr->id: " << hdr->id() << endl;

  switch ((DsOpCode)hdr->opCode()) {
    case DsOpCode::DataEntityInfo: {
      string data = DataEntities2Json(this->m_dataEntities);
      DataServicePacket rspPkg;
      rspPkg.generate(hdr->id(), DsFrameType::Rsp, (DsOpCode)hdr->opCode(), 0,
                      data.c_str(), data.length());
      client->send(rspPkg.buffer().getData(), rspPkg.buffer().getSize());
      break;
    }

    case DsOpCode::DataEntityNames: {
      string names;
      for (auto& e : this->m_dataEntities) {
        names += e.name + "\n";
      }
      Trim(names);
      DataServicePacket rspPkg;
      rspPkg.generate(hdr->id(), DsFrameType::Rsp, (DsOpCode)hdr->opCode(), 0,
                      names.c_str(), names.length());
      client->send(rspPkg.buffer().getData(), rspPkg.buffer().getSize());
      break;
    }
  }
}

/**
 * @brief class DataServiceClient
 *
 */
DataServiceClient::DataServiceClient(/* args */)
    : m_conn(TcpConnection::CreateInstance()) {}

DataServiceClient::~DataServiceClient() {}

bool DataServiceClient::start() {
  if (this->m_conn->connect(5300, "127.0.0.1")) {
    DataServicePacket pkg;
    pkg.generate(1, DsFrameType::Req, DsOpCode::DataEntityInfo, 0, nullptr, 0);
    this->m_conn->send(pkg.buffer().getData(), pkg.buffer().getSize());
  }

  this->m_conn->onData(
      [this](const uint8_t* data, uint32_t size, const TcpConnectionPtr& conn) {
        this->m_pkgHelper.append(data, size);
        DsPacketPtr pkg;

        while ((pkg = this->m_pkgHelper.getPackage()) != nullptr) {
          auto hdr = pkg->header();
          auto frameType = hdr->frameType();
          // if(DsFrameType::Req == DsFrameType(frameType)){
          //   this->handleReq(*pkg);
          // }

          if (DsFrameType::Rsp == DsFrameType(frameType)) {
            this->handleRsp(*pkg);
          }
        }
      });
  return true;
}

bool DataServiceClient::stop() { return true; }

std::vector<std::string> DataServiceClient::getDataEntityNames() {
  std::vector<std::string> names;

  DataServicePacket pkg;
  pkg.generate(2, DsFrameType::Req, DsOpCode::DataEntityNames, 0, nullptr, 0);
  auto promise = this->send(pkg);

  auto rspPkg = promise->get(1000);
  if (nullptr == rspPkg) return names;

  rspPkg->header();
  string dataStr((const char*)rspPkg->data(), rspPkg->dataSize());

  return names;
}

void DataServiceClient::removePromise(uint32_t pkgId) {
  lock_guard<mutex> lk(this->m_mutexPkgId2Promise);
  this->m_pkgId2Promise.erase(pkgId);
}

DsPkgPromisePtr DataServiceClient::send(const DataServicePacket& pkg) {
  lock_guard<mutex> lk(this->m_mutexPkgId2Promise);
  auto id = pkg.header()->id();
  DsPkgPromisePtr promise(make_shared<DsPkgPromise>(
      id, Bind(&DataServiceClient::removePromise, this)));
  this->m_pkgId2Promise.insert(make_pair(id, promise.get()));

  this->m_conn->send(pkg.buffer().getData(), pkg.buffer().getSize());
  return promise;
}

void DataServiceClient::handleRsp(const DataServicePacket& pkg) {
  auto remoteHdr = pkg.header();

  auto pkgId = remoteHdr->id();
  lock_guard<mutex> lk(this->m_mutexPkgId2Promise);
  auto it = this->m_pkgId2Promise.find(pkgId);
  if (it == this->m_pkgId2Promise.end()) return;

  (it->second)->set(make_shared<DataServicePacket>(pkg));
  this->m_pkgId2Promise.erase(pkgId);
}

NS_FF_END
