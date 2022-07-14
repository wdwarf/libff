/**
 * @file DataService.cpp
 * @auth DuckyLi
 * @date 2022-07-11 12:03:09
 * @description
 */
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
             ? make_pair(DataValueType::Boolean, 1)
         : "int8" == name   ? make_pair(DataValueType::Int8, 1)
         : "int16" == name  ? make_pair(DataValueType::Int16, 1)
         : "int32" == name  ? make_pair(DataValueType::Int32, 1)
         : "int64" == name  ? make_pair(DataValueType::Int64, 1)
         : "uint8" == name  ? make_pair(DataValueType::Uint8, 1)
         : "uint16" == name ? make_pair(DataValueType::Uint16, 1)
         : "uint32" == name ? make_pair(DataValueType::Uint32, 1)
         : "uint64" == name ? make_pair(DataValueType::Uint64, 1)
                            : make_pair(DataValueType::Unknown, 0);
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

bool DataServicePacket::generate(uint32_t id, uint16_t frameType,
                                 uint16_t opCode, uint16_t option,
                                 const void* data, uint32_t dataLength) {
  this->m_buffer.alloc(DsPkgHeader::Size() + dataLength);
  DsPkgHeader* hdr = (DsPkgHeader*)this->m_buffer.getData();
  hdr->magic(DS_PKG_MAGIC);
  hdr->id(id);
  hdr->frameType(frameType);
  hdr->opCode(opCode);
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
 * @brief class DataServiceServer
 *
 */
DataServiceServer::DataServiceServer(/* args */)
    : m_conn(TcpConnection::CreateInstance()) {}

DataServiceServer::~DataServiceServer() {}

bool DataServiceServer::start() {
  this->m_conn->listen(5300);
  this->m_conn->onAccept([](const TcpConnectionPtr& client) {

  });
  return true;
}

/**
 * @brief class DataServiceClient
 *
 */
DataServiceClient::DataServiceClient(/* args */)
    : m_conn(TcpConnection::CreateInstance()) {}

DataServiceClient::~DataServiceClient() {}

bool DataServiceClient::start() { return true; }

NS_FF_END
