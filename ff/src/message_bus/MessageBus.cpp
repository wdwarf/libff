#include <ff/Bind.h>
#include <ff/MessageBus.h>

#include <atomic>
#include <cstring>

using namespace std;

NS_FF_BEG

/**
 *
 * class MsgBusPkgHeader
 *
 */
static atomic_uint32_t g_pkg_id(0);
MsgBusPkgHeader::MsgBusPkgHeader() {
  memset(this, 0, sizeof(MsgBusPkgHeader));
  this->id(g_pkg_id++);
}

uint32_t MsgBusPkgHeader::length() const { return ::ntohl(this->m_length); }
MsgBusPkgHeader& MsgBusPkgHeader::length(uint32_t len) {
  this->m_length = ::htonl(len);
  return *this;
}

uint32_t MsgBusPkgHeader::id() const { return ::ntohl(this->m_id); }
MsgBusPkgHeader& MsgBusPkgHeader::id(uint32_t id) {
  this->m_id = ::htonl(id);
  return *this;
}

uint32_t MsgBusPkgHeader::from() const { return ::ntohl(this->m_from); }
MsgBusPkgHeader& MsgBusPkgHeader::from(uint32_t from) {
  this->m_from = ::htonl(from);
  return *this;
}

uint32_t MsgBusPkgHeader::target() const { return ::ntohl(this->m_target); }
MsgBusPkgHeader& MsgBusPkgHeader::target(uint32_t target) {
  this->m_target = ::htonl(target);
  return *this;
}

uint32_t MsgBusPkgHeader::code() const { return ::ntohl(this->m_code); }
MsgBusPkgHeader& MsgBusPkgHeader::code(uint32_t code) {
  this->m_code = ::htonl(code);
  return *this;
}

uint32_t MsgBusPkgHeader::msgId() const { return ::ntohl(this->m_msgId); }
MsgBusPkgHeader& MsgBusPkgHeader::msgId(uint32_t msgId) {
  this->m_msgId = ::htonl(msgId);
  return *this;
}

uint32_t MsgBusPkgHeader::options() const { return ::ntohl(this->m_options); }
MsgBusPkgHeader& MsgBusPkgHeader::options(uint32_t options) {
  this->m_options = ::htonl(options);
  return *this;
}

/**
 *
 * class MsgBusPackage
 *
 */

atomic_uint32_t MsgBusPackage::g_id(0);

bool MsgBusPackage::parse(const void* data, uint32_t len) {
  if (len < sizeof(MsgBusPkgHeader)) return false;

  MsgBusPkgHeader* header = (MsgBusPkgHeader*)data;

  auto length = header->length();
  if (length > len || length > MAX_MSGBUS_PKG_SIZE) return false;

  this->setData(data, length);
  return true;
}

MsgBusPkgHeader* MsgBusPackage::header() const {
  return (MsgBusPkgHeader*)(this->getData());
}

bool MsgBusPackage::generate(const MsgBusPkgHeader& header, const void* data,
                             uint32_t dataLen) {
  auto pkgLen = dataLen + sizeof(MsgBusPkgHeader);
  if (pkgLen > MAX_MSGBUS_PKG_SIZE) return false;

  this->alloc(pkgLen);
  memcpy(this->getData(), &header, sizeof(MsgBusPkgHeader));
  memcpy(this->getData() + sizeof(MsgBusPkgHeader), data, dataLen);

  auto pHdr = this->header();
  pHdr->length(pkgLen);
  if (0 == pHdr->id()) {
    pHdr->id(++MsgBusPackage::g_id);
  }
  return true;
}
void* MsgBusPackage::data() const {
  if (this->isEmpty()) return nullptr;
  return (this->getData() + sizeof(MsgBusPkgHeader));
}

uint32_t MsgBusPackage::dataSize() const {
  if (this->isEmpty()) return 0;
  MsgBusPkgHeader* hdr = header();
  return (hdr->length() - sizeof(MsgBusPkgHeader));
}

/**
 *
 * class MsgBusPackageHelper
 *
 */

MsgBusPackageHelper& MsgBusPackageHelper::append(const void* data,
                                                 uint32_t len) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_buffer.append(data, len);
  return *this;
}

MsgBusPackagePtr MsgBusPackageHelper::getPackage() {
  lock_guard<mutex> lk(this->m_mutex);
  if (this->m_buffer.getSize() < sizeof(MsgBusPkgHeader)) return nullptr;

  const MsgBusPkgHeader* hdr = (const MsgBusPkgHeader*)this->m_buffer.getData();
  auto length = hdr->length();
  if (length > MAX_MSGBUS_PKG_SIZE) {
    this->m_buffer.clear();
    return nullptr;
  }

  MsgBusPackagePtr pkg = make_shared<MsgBusPackage>();
  pkg->setData(this->m_buffer.getData(), length);

  this->m_buffer.setData(this->m_buffer.getData() + length,
                         this->m_buffer.getSize() - length);

  return pkg;
}

/**
 *
 * class PkgPromise
 *
 */

PkgPromise::PkgPromise(uint32_t id, PkgPromiseRemoveFunc func)
    : m_id(id), m_func(func) {
  this->m_future = this->m_promise.get_future();
}

PkgPromise::~PkgPromise() {
  this->lock();
  this->m_func(m_id);
  this->unlock();
}

MsgBusPackagePtr PkgPromise::get(uint32_t ms) {
  if (future_status::ready ==
      this->m_future.wait_for(chrono::milliseconds(ms))) {
    return this->m_future.get();
  }
  return nullptr;
}

void PkgPromise::set(MsgBusPackagePtr pkg) {
  this->lock();
  this->m_promise.set_value(pkg);
  this->unlock();
}

/**
 *
 * class MessageBusServer
 *
 */

MessageBusServer::MessageBusServer() {
  this->m_conn = TcpConnection::CreateInstance();
  this->m_conn->onAccept(Bind(&MessageBusServer::onAccept, this))
      .onClose(Bind(&MessageBusServer::onClose, this));
}

bool MessageBusServer::start(uint16_t port, const std::string& host) {
  return this->m_conn->listen(port, host);
}

bool MessageBusServer::stop() {
  this->m_conn->close();
  return true;
}

void MessageBusServer::onClientData(const uint8_t* data, uint32_t size,
                                    const TcpConnectionPtr& client) {
  ClientSessionPtr session;
  {
    lock_guard<mutex> lk(this->m_clientsMutex);
    session = this->m_clients[client];
  }

  session->m_pkgHelper.append(data, size);

  MsgBusPackagePtr pkg;
  while ((pkg = session->m_pkgHelper.getPackage()) != nullptr) {
    auto hdr = pkg->header();

    auto target = hdr->target();
    auto code = hdr->code();

    lock_guard<mutex> lk(this->m_clientsMutex);

    if (MsgCode::Register == code) {
      uint32_t* data = (uint32_t*)pkg->data();
      uint32_t size = pkg->dataSize() / sizeof(uint32_t);
      for (uint32_t i = 0; i < size; ++i) {
        auto session = this->m_clients[client];
        session->m_clientId = hdr->from();
        this->m_msgId2Clients[data[i]].insert(session);
      }
    }

    if (MsgCode::MsgTrans == code) {
      auto reqType = MSGOPT_REQ_TYPE(hdr->options());

      if (MsgOpt::Req == reqType) {
        auto& clients = this->m_msgId2Clients[hdr->msgId()];
        for (auto& session : clients) {
          if (0 != target && session->m_clientId != target) continue;
          session->m_connection->send(pkg->getData(), pkg->getSize());
        }
      }

      if (MsgOpt::Rsp == reqType) {
        for (auto& p : this->m_clients) {
          auto session = p.second;
          if (session->m_clientId != target) continue;
          session->m_connection->send(pkg->getData(), pkg->getSize());
        }
      }
    }
  }
}

void MessageBusServer::onAccept(const TcpConnectionPtr& client) {
  cout << client->getSocket().getRemoteAddress() << ":"
       << client->getSocket().getRemotePort() << " connected" << endl;

  client->onClose([this](const TcpConnectionPtr& client) {
    cout << client->getSocket().getRemoteAddress() << ":"
         << client->getSocket().getRemotePort() << " disconnected" << endl;
    this->removeClient(client);
  });
  client->onData(Bind(&MessageBusServer::onClientData, this));

  this->addClient(client);
}

void MessageBusServer::onClose(const TcpConnectionPtr& client) {
  lock_guard<mutex> lk(this->m_clientsMutex);
  this->m_msgId2Clients.clear();
}

void MessageBusServer::addClient(const TcpConnectionPtr& client) {
  lock_guard<mutex> lk(this->m_clientsMutex);

  auto session = make_shared<ClientSession>();
  session->m_clientId = 0;
  session->m_connection = client;
  this->m_clients.insert(make_pair(client, session));
}

void MessageBusServer::removeClient(const TcpConnectionPtr& client) {
  lock_guard<mutex> lk(this->m_clientsMutex);
  this->m_clients.erase(client);
}

/**
 *
 * class MessageBusClient
 *
 */

MessageBusClient::MessageBusClient(uint32_t clientId)
    : m_clientId(clientId),
      m_conn(TcpConnection::CreateInstance()),
      m_stoped(true) {
  this->m_conn->onData(Bind(&MessageBusClient::onData, this))
      .onClose(Bind(&MessageBusClient::onClose, this));
}

uint32_t MessageBusClient::clientId() const { return this->m_clientId; }

bool MessageBusClient::start(uint16_t serverPort, const std::string& serverHost,
                             uint16_t localPort, const std::string& localHost) {
  if (!this->m_stoped) return true;
  this->m_stoped = false;
  thread([this, serverPort, serverHost, localPort, localHost]() {
    while (!this->m_stoped) {
      if (this->m_conn->getSocket().isConnected()) {
        this_thread::sleep_for(chrono::milliseconds(1000));
        continue;
      }

      unique_lock<mutex> lk(this->m_mutex);
      if (!this->m_conn->connect(serverPort, serverHost, localPort,
                                 localHost)) {
        this_thread::sleep_for(chrono::milliseconds(300));
        continue;
      }

      this->onConnected();
      this->m_cond.wait(lk);
    }
  }).detach();

  return true;
}

bool MessageBusClient::stop() {
  unique_lock<mutex> lk(this->m_mutex);
  this->m_stoped = true;
  this->m_conn->close();
  this->m_cond.notify_one();
  return true;
}

void MessageBusClient::send(const MsgBusPackage& pkg) {
  this->m_conn->send(pkg.getData(), pkg.getSize());
}

void MessageBusClient::onConnected() { this->sendRegisterInfo(); }

void MessageBusClient::onData(const uint8_t* data, uint32_t size,
                              const TcpConnectionPtr& client) {
  this->m_pkgHelper.append(data, size);

  MsgBusPackagePtr pkg;
  while ((pkg = this->m_pkgHelper.getPackage()) != nullptr) {
    auto hdr = pkg->header();
    auto code = hdr->code();
    auto options = hdr->options();

    if (MsgCode::MsgTrans == code) {
      auto reqType = MSGOPT_REQ_TYPE(options);
      if (MsgOpt::Req == reqType) {
        this->handleReq(*pkg);
      }
      if (MsgOpt::Rsp == reqType) {
        this->handleRsp(*pkg);
      }
    }
  }
}

void MessageBusClient::onClose(const TcpConnectionPtr& client) {
  unique_lock<mutex> lk(this->m_mutex);
  this->m_cond.notify_one();
}

void MessageBusClient::sendRegisterInfo() {
  unique_lock<mutex> lk(this->m_mutexMsgId2Func);
  vector<uint32_t> codes;
  for (auto& p : this->msgId2Func) {
    codes.push_back(p.first);
  }

  if (this->m_conn->getSocket().isConnected()) {
    MsgBusPkgHeader hdr;
    hdr.code(MsgCode::Register);
    hdr.from(this->m_clientId);
    hdr.options(MsgOpt::Req);
    MsgBusPackage pkg;
    pkg.generate(hdr, &codes[0], codes.size() * sizeof(uint32_t));
    this->send(pkg);
  }
}

MessageBusClient& MessageBusClient::on(uint32_t msgId, MsgBusReqFunc func) {
  {
    unique_lock<mutex> lk(this->m_mutexMsgId2Func);
    this->msgId2Func.insert(make_pair(msgId, func));
  }
  this->sendRegisterInfo();

  return *this;
}

PkgPromisePtr MessageBusClient::req(uint32_t msgId, uint32_t target,
                                    const void* data, uint32_t dataSize) {
  lock_guard<mutex> lk(this->m_mutexPkgId2Promise);

  MsgBusPkgHeader hdr;
  hdr.code(MsgCode::MsgTrans);
  hdr.from(this->m_clientId);
  hdr.target(target);
  hdr.msgId(msgId);
  hdr.options(MsgOpt::Req);
  MsgBusPackage pkg;
  pkg.generate(hdr, data, dataSize);

  auto id = hdr.id();
  PkgPromisePtr promise(make_shared<PkgPromise>(
      id, Bind(&MessageBusClient::removePromise, this)));
  this->m_pkgId2Promise.insert(make_pair(id, &promise));

  this->send(pkg);
  return promise;
}

void MessageBusClient::removePromise(uint32_t pkgId) {
  lock_guard<mutex> lk(this->m_mutexPkgId2Promise);
  this->m_pkgId2Promise.erase(pkgId);
}

void MessageBusClient::handleReq(const MsgBusPackage& pkg) {
  auto remoteHdr = pkg.header();

  MsgBusReqFunc func;
  {
    unique_lock<mutex> lk(this->m_mutexMsgId2Func);
    auto it = this->msgId2Func.find(remoteHdr->msgId());
    if (it != this->msgId2Func.end()) func = it->second;
  }

  if (func) func(pkg);
}

void MessageBusClient::handleRsp(const MsgBusPackage& pkg) {
  auto remoteHdr = pkg.header();

  auto pkgId = remoteHdr->id();
  lock_guard<mutex> lk(this->m_mutexPkgId2Promise);
  auto it = this->m_pkgId2Promise.find(pkgId);
  if (it == this->m_pkgId2Promise.end()) return;
  
  (*it->second)->set(make_shared<MsgBusPackage>(pkg));
  this->m_pkgId2Promise.erase(pkgId);
}
NS_FF_END
