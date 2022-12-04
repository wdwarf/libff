#include <ff/Bind.h>
#include <ff/MessageBus.h>
#include <ff/Tick.h>

#include <atomic>
#include <cstring>

using namespace std;

NS_FF_BEG

/**
 *
 * class MsgBusPkgHeader
 *
 */

uint32_t MsgBusPkgHeader::generateChecksum() const {
  uint32_t code = 0;
  const uint8_t* p = (const uint8_t*)this;
  uint32_t len = sizeof(MsgBusPkgHeader) - sizeof(this->m_checksum);
  for (uint32_t i = 0; i < len; ++i) {
    code += p[i];
  }
  return code;
}

/**
 *
 * class MsgBusPackage
 *
 */

atomic<uint32_t> MsgBusPackage::g_id(0);

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
  pHdr->checksum(pHdr->generateChecksum());
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
  if (this->m_buffer.getSize() < MsgBusPkgHeader::Size()) return nullptr;

  const MsgBusPkgHeader* hdr = (const MsgBusPkgHeader*)this->m_buffer.getData();
  auto length = hdr->length();
  if (this->m_buffer.getSize() < length) return nullptr;
  if (length > MAX_MSGBUS_PKG_SIZE ||
      hdr->checksum() != hdr->generateChecksum()) {
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
 * class ClientSession
 *
 */
ClientSession::ClientSession() : m_clientId(0) {}

ClientSession::~ClientSession() {}

/**
 *
 * class PkgPromise
 *
 */
#ifndef MSGBUS_USE_CUSTOM_PROMISE

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

#else

PkgPromise::PkgPromise(uint32_t id, PkgPromiseRemoveFunc func)
    : m_id(id), m_func(func) {}

PkgPromise::~PkgPromise() {
  this->lock();
  this->m_func(m_id);
  this->unlock();
}

MsgBusPackagePtr PkgPromise::get(uint32_t ms) {
  std::unique_lock<mutex> lk(this->m_mutex);
  if (this->m_cond.wait_for(lk, chrono::milliseconds(ms),
                            [this]() { return (nullptr != this->m_pkg); })) {
    return this->m_pkg;
  }
  return nullptr;
}

void PkgPromise::set(MsgBusPackagePtr pkg) {
  this->lock();
  std::unique_lock<mutex> lk(this->m_mutex);
  this->m_pkg = pkg;
  this->m_cond.notify_all();
  this->unlock();
}

#endif

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

MessageBusServer::~MessageBusServer() {}

bool MessageBusServer::start(uint16_t port, const std::string& host) {
  this->m_running = this->m_conn->listen(port, host);
  return this->m_running.load();
}

bool MessageBusServer::stop() {
  this->m_conn->close();
  while (true) {
    {
      lock_guard<mutex> lk(this->m_clientsMutex);
      if (this->m_clients.empty()) break;
    }
    std::this_thread::yield();
  }

  unique_lock<mutex> lk(this->m_stopMutex);
  if (this->m_running) this->m_stopCond.wait(lk);
  return true;
}

void MessageBusServer::onClientData(const uint8_t* data, uint32_t size,
                                    const TcpConnectionPtr& client) {
  ClientSessionPtr session;
  {
    lock_guard<mutex> lk(this->m_clientsMutex);
    auto it = this->m_clients.find(client);
    if (it == this->m_clients.end()) return;
    session = it->second;
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

      session->m_clientId = hdr->from();

      for (uint32_t i = 0; i < size; ++i) {
        this->m_msgId2Clients[data[i]].insert(session);
      }

      MsgBusPkgHeader rspHdr = *hdr;
      rspHdr.target(hdr->from());
      rspHdr.from(0);
      rspHdr.options(MsgOpt::Rsp);
      MsgBusPackage rspPkg;
      rspPkg.generate(rspHdr);
      client->send(rspPkg.getData(), rspPkg.getSize());
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

    if (MsgCode::HeartBeat == code) {
      MsgBusPkgHeader rspHdr = *hdr;
      rspHdr.target(hdr->from());
      rspHdr.from(0);
      rspHdr.options(MsgOpt::Rsp);
      MsgBusPackage rspPkg;
      rspPkg.generate(rspHdr);
      client->send(rspPkg.getData(), rspPkg.getSize());
    }
  }
}

void MessageBusServer::onAccept(const TcpConnectionPtr& client) {
  // cout << "msg bus client " << client->getSocket().getRemoteAddress() << ":"
  //      << client->getSocket().getRemotePort() << " connected" << endl;
  this->addClient(client);
  client->onClose([this](const TcpConnectionPtr& conn) {
    // cout << "msg bus client " << conn->getSocket().getRemoteAddress() << ":"
    //      << conn->getSocket().getRemotePort() << " disconnected" << endl;
    this->removeClient(conn);
  });
  client->onData(Bind(&MessageBusServer::onClientData, this));
}

void MessageBusServer::onClose(const TcpConnectionPtr& client) {
  lock_guard<mutex> lk(this->m_clientsMutex);
  this->m_msgId2Clients.clear();

  lock_guard<mutex> lkStop(this->m_stopMutex);
  this->m_running = false;
  this->m_stopCond.notify_one();
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
  auto it = this->m_clients.find(client);
  if (it == this->m_clients.end()) return;

  for (auto& p : this->m_msgId2Clients) {
    p.second.erase(it->second);
  }

  this->m_clients.erase(it);
}

/**
 *
 * class MessageBusClient
 *
 */

MessageBusClient::MessageBusClient()
    : m_conn(TcpConnection::CreateInstance()),
      m_heartbeatLossCnt(0),
      m_connected(false),
      m_stoped(true) {
  this->m_conn->onData(Bind(&MessageBusClient::onData, this))
      .onClose(Bind(&MessageBusClient::onClose, this));
}

MessageBusClient::MessageBusClient(uint32_t clientId)
    : m_clientId(clientId),
      m_heartbeatLossCnt(0),
      m_conn(TcpConnection::CreateInstance()),
      m_connected(false),
      m_stoped(true) {
  this->m_conn->onData(Bind(&MessageBusClient::onData, this))
      .onClose(Bind(&MessageBusClient::onClose, this));
}

uint32_t MessageBusClient::clientId() const { return this->m_clientId; }

void MessageBusClient::clientId(uint32_t id) { this->m_clientId = id; }

bool MessageBusClient::isConnected() const { return this->m_connected; }

bool MessageBusClient::start(uint16_t serverPort, const std::string& serverHost,
                             uint16_t localPort, const std::string& localHost) {
  if (!this->m_stoped) return true;
  this->m_stoped = false;
  this->m_connThread =
      thread([this, serverPort, serverHost, localPort, localHost]() {
        Tick hbTick;
        uint32_t hbLoseCnt = 0;
        while (!this->m_stoped) {
          if (this->m_conn->getSocket().isConnected() && this->m_connected) {
            this_thread::sleep_for(chrono::milliseconds(1000));
            if (hbTick.tock() >= 5000) {
              MsgBusPkgHeader hdr;
              hdr.code(MsgCode::HeartBeat);
              hdr.from(this->m_clientId);
              hdr.options(MsgOpt::Req);
              MsgBusPackage pkg;
              pkg.generate(hdr);

              auto promise = this->req(pkg);
              auto rspPkg = promise->get(5000);
              if (!rspPkg) {
                ++this->m_heartbeatLossCnt;
                if (this->m_heartbeatLossCnt >= 3) {
                  lock_guard<mutex> lk(this->m_mutex);
                  if (this->m_onHbLossFunc) this->m_onHbLossFunc();
                  this->m_conn->close();
                  this->m_heartbeatLossCnt = 0;
                }
              }

              hbTick.tick();
            }
            continue;
          }

          if (!this->m_conn->connect(serverPort, serverHost, localPort,
                                     localHost)) {
            this_thread::sleep_for(chrono::milliseconds(300));
            continue;
          }
          if (!this->sendRegisterInfo()) {
            this->m_conn->close();
            while (this->m_conn->getSocket().isConnected()) {
              this_thread::sleep_for(chrono::milliseconds(100));
            }
            continue;
          }

          unique_lock<mutex> lk(this->m_mutex);
          this->m_connected = true;
          this->m_heartbeatLossCnt = 0;
          if (this->m_onConnectedFunc) this->m_onConnectedFunc();
        }
      });

  return true;
}

bool MessageBusClient::stop() {
  unique_lock<mutex> lk(this->m_mutex);
  this->m_stoped = true;
  this->m_conn->close();

#ifdef MSGBUS_USE_CUSTOM_PROMISE
  this->m_cond.notify_one();
#endif
  if (this->m_connThread.joinable()) this->m_connThread.join();
  return true;
}

void MessageBusClient::send(const MsgBusPackage& pkg) {
  this->m_conn->send(pkg.getData(), pkg.getSize());
}

void MessageBusClient::onData(const uint8_t* data, uint32_t size,
                              const TcpConnectionPtr& client) {
  this->m_heartbeatLossCnt = 0;
  this->m_pkgHelper.append(data, size);

  MsgBusPackagePtr pkg;
  while ((pkg = this->m_pkgHelper.getPackage()) != nullptr) {
    auto hdr = pkg->header();
    auto code = hdr->code();
    auto options = hdr->options();

    // if (MsgCode::MsgTrans == code) {
    auto reqType = MSGOPT_REQ_TYPE(options);
    if (MsgOpt::Req == reqType) {
      this->handleReq(*pkg);
    }

    if (MsgOpt::Rsp == reqType) {
      this->handleRsp(*pkg);
    }
    // }
  }
}

void MessageBusClient::onClose(const TcpConnectionPtr& client) {
  unique_lock<mutex> lk(this->m_mutex);
  if (this->m_connected && this->m_onDisconnectedFunc) {
    this->m_onDisconnectedFunc();
  }
  this->m_connected = false;
  this->m_conn->getSocket().close();

#ifdef MSGBUS_USE_CUSTOM_PROMISE
  this->m_cond.notify_one();
#endif
}

bool MessageBusClient::sendRegisterInfo() {
  unique_lock<mutex> lk(this->m_mutexMsgId2Func);
  vector<uint32_t> codes;
  for (auto& p : this->msgId2Func) {
    codes.push_back(p.first);
  }

  // if (this->m_conn->getSocket().isConnected()) {
  //   MsgBusPkgHeader hdr;
  //   hdr.code(MsgCode::Register);
  //   hdr.from(this->m_clientId);
  //   hdr.options(MsgOpt::Req);
  //   MsgBusPackage pkg;
  //   pkg.generate(hdr, &codes[0], codes.size() * sizeof(uint32_t));
  //   this->send(pkg);
  // }
  if (this->m_conn->getSocket().isConnected()) {
    MsgBusPkgHeader hdr;
    hdr.code(MsgCode::Register);
    hdr.from(this->m_clientId);
    hdr.options(MsgOpt::Req);
    MsgBusPackage pkg;
    pkg.generate(hdr, &codes[0], codes.size() * sizeof(uint32_t));
    auto promise = this->req(pkg);
    auto rspPkg = promise->get(5 * 1000);
    return (nullptr != rspPkg.get());
  }

  return false;
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
  MsgBusPkgHeader hdr;
  hdr.code(MsgCode::MsgTrans);
  hdr.from(this->m_clientId);
  hdr.target(target);
  hdr.msgId(msgId);
  hdr.options(MsgOpt::Req);
  MsgBusPackage pkg;
  pkg.generate(hdr, data, dataSize);

  return this->req(pkg);
}

PkgPromisePtr MessageBusClient::req(const MsgBusPackage& pkg) {
  lock_guard<mutex> lk(this->m_mutexPkgId2Promise);
  auto id = pkg.header()->id();
  PkgPromisePtr promise(make_shared<PkgPromise>(
      id, Bind(&MessageBusClient::removePromise, this)));
  this->m_pkgId2Promise.insert(make_pair(id, promise.get()));

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

  (it->second)->set(make_shared<MsgBusPackage>(pkg));
  this->m_pkgId2Promise.erase(pkgId);
}

void MessageBusClient::onConnected(MsbBusOnConnectedFunc func) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_onConnectedFunc = func;
  if (this->m_connected && this->m_onConnectedFunc) this->m_onConnectedFunc();
}

void MessageBusClient::onDisconnected(MsbBusOnDisonnectedFunc func) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_onDisconnectedFunc = func;
}

void MessageBusClient::onHeartbeatLoss(MsbBusOnDisonnectedFunc func) {
  lock_guard<mutex> lk(this->m_mutex);
  this->m_onHbLossFunc = func;
}

NS_FF_END
