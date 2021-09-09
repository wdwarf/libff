#ifndef FF_MESSAGEBUS_H_
#define FF_MESSAGEBUS_H_

#include <ff/Buffer.h>
#include <ff/Synchronizable.h>
#include <ff/TcpConnection.h>
#include <ff/ff_config.h>

#include <atomic>
#include <condition_variable>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <set>

NS_FF_BEG

#define MAX_MSGBUS_PKG_SIZE 4096

enum MsgOpt {
  Req = 0x00000000,
  Rsp = 0x00000001,
};

#define MSGOPT_REQ_TYPE(options) (options & 0x00000001)

enum MsgCode {
  Register = 0,   // 注册
  HeartBeat = 1,  // 心跳
  MsgTrans = 2    // 消息交换
};

/**
 * 1、客户端注册（消息登记）
 * 2、心跳管理
 * 3、消息请求
 * 4、消息响应
 */

class MsgBusPkgHeader {
  uint32_t m_length;
  uint32_t m_id;
  uint32_t m_from;
  uint32_t m_target;
  uint32_t m_code;
  uint32_t m_msgId;
  uint32_t m_options;

 public:
  MsgBusPkgHeader();

  uint32_t length() const;
  MsgBusPkgHeader& length(uint32_t len);
  uint32_t id() const;
  MsgBusPkgHeader& id(uint32_t id);
  uint32_t from() const;
  MsgBusPkgHeader& from(uint32_t from);
  uint32_t target() const;
  MsgBusPkgHeader& target(uint32_t target);
  uint32_t code() const;
  MsgBusPkgHeader& code(uint32_t code);
  uint32_t msgId() const;
  MsgBusPkgHeader& msgId(uint32_t msgId);
  uint32_t options() const;
  MsgBusPkgHeader& options(uint32_t options);
};

class MsgBusPackage : public Buffer {
 public:
  bool parse(const void* data, uint32_t len);
  bool generate(const MsgBusPkgHeader& header, const void* data = nullptr,
                uint32_t dataLen = 0);

  MsgBusPkgHeader* header() const;
  void* data() const;
  uint32_t dataSize() const;

 private:
  static std::atomic_uint32_t g_id;
};

using MsgBusPackagePtr = std::shared_ptr<MsgBusPackage>;
using PkgPromiseRemoveFunc = std::function<void(uint32_t msgId)>;
class PkgPromise : public Synchronizable {
 public:
  PkgPromise(uint32_t msgId, PkgPromiseRemoveFunc func);
  ~PkgPromise();

  MsgBusPackagePtr get(uint32_t ms);
  void set(MsgBusPackagePtr pkg);

 private:
  uint32_t m_id;
  PkgPromiseRemoveFunc m_func;
  std::promise<MsgBusPackagePtr> m_promise;
  std::future<MsgBusPackagePtr> m_future;
};

using PkgPromisePtr = std::shared_ptr<PkgPromise>;

class MsgBusPackageHelper {
 public:
  MsgBusPackageHelper& append(const void* data, uint32_t len);
  MsgBusPackagePtr getPackage();

 private:
  ff::Buffer m_buffer;
  std::mutex m_mutex;
};

struct ClientSession {
  uint32_t m_clientId;
  TcpConnectionPtr m_connection;
  MsgBusPackageHelper m_pkgHelper;
};

using ClientSessionPtr = std::shared_ptr<ClientSession>;

class MessageBusServer {
 public:
  MessageBusServer();

  bool start(uint16_t port, const std::string& host = "");
  bool stop();

 private:
  TcpConnectionPtr m_conn;
  std::mutex m_clientsMutex;
  std::map<uint32_t, std::set<ClientSessionPtr> > m_msgId2Clients;

  void onClose(const TcpConnectionPtr& client);
  void onAccept(const TcpConnectionPtr& client);

  void onClientData(const uint8_t* data, uint32_t size,
                    const TcpConnectionPtr& client);

  std::map<TcpConnectionPtr, ClientSessionPtr> m_clients;

  void addClient(const TcpConnectionPtr& client);
  void removeClient(const TcpConnectionPtr& client);
};

using MsgBusReqFunc = std::function<void(const MsgBusPackage& pkg)>;

class MessageBusClient {
 public:
  MessageBusClient(uint32_t clientId);

  bool start(uint16_t serverPort, const std::string& serverHost,
             uint16_t localPort, const std::string& localHost = "");
  bool stop();

  void send(const MsgBusPackage& pkg);

  MessageBusClient& on(uint32_t msgId, MsgBusReqFunc func);
  PkgPromisePtr req(uint32_t msgId, uint32_t target = 0,
                    const void* data = nullptr, uint32_t dataSize = 0);
  uint32_t clientId() const;
  bool isConnected() const;

 private:
  uint32_t m_clientId;
  std::mutex m_mutex;
  std::condition_variable m_cond;
  MsgBusPackageHelper m_pkgHelper;
  TcpConnectionPtr m_conn;
  std::atomic_bool m_stoped;
  std::mutex m_mutexMsgId2Func;
  std::map<uint32_t, MsgBusReqFunc> msgId2Func;
  std::map<uint32_t, PkgPromisePtr*> m_pkgId2Promise;
  std::mutex m_mutexPkgId2Promise;

  void removePromise(uint32_t pkgId);

  void onConnected();
  void onData(const uint8_t* data, uint32_t size,
              const TcpConnectionPtr& client);
  void onClose(const TcpConnectionPtr& client);

  void sendRegisterInfo();

  void handleReq(const MsgBusPackage& pkg);
  void handleRsp(const MsgBusPackage& pkg);
};

using MessageBusClientPtr = std::shared_ptr<MessageBusClient>;
using MessageBusServerPtr = std::shared_ptr<MessageBusServer>;

NS_FF_END

#endif
