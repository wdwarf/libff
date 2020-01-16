/*
 * ApplicationImpl.h
 *
 *  Created on: Aug 5, 2019
 *      Author: root
 */

#ifndef APP_APPLICATIONIMPL_H_
#define APP_APPLICATIONIMPL_H_

#include <unistd.h>

#include <string>
#include <list>
#include <mutex>
#include <condition_variable>
#include <map>

#include <ff/Application.h>
#include <ff/Settings.h>
#include <ff/EPoll.h>

namespace NS_FF {

class Application::ApplicationImpl: public Settings, public EPoll {
public:
	ApplicationImpl(Application* _app, int argc, char** argv);
	virtual ~ApplicationImpl();

	int run();
	void exit(int code);
	int getExitCode() const;
	const std::vector<std::string>& getCommandLines() const;
	const std::string& getCommandLine(int index) const;
	int getCommandLineCount() const;
	const Variant& getValue(const std::string& key) const;
	void setValue(const std::string& key, const Variant& value);
	bool hasValue(const std::string& key) const;
	void removeValue(const std::string& key);
	std::set<std::string> getKeys() const;
	void saveSettings(const std::string& file);
	void loadSettings(const std::string& file);

	void subscribeMsgHandler(uint32_t msgId, MsgHandler handler);
	void sendMessage(uint32_t msgId, uint32_t msgData, MsgHandler callBack = nullptr, int32_t timeoutMs = 3000);
	void responseMessage(uint32_t serialNum, uint64_t msgId, uint64_t msgData = 0);

	static std::string GetApplicationPath();
	static std::string GetApplicationName();
	static std::string GetCurrentWorkDir();

private:
	Application* m_app;
	Settings m_settings;
	std::vector<std::string> m_cmdLines;
	int m_exitCode;
	bool m_running;
	std::mutex m_mutex;
	std::map<uint32_t, MsgHandler> m_serialNum2MsgCallback;
	std::multimap<uint32_t, MsgHandler> m_msgId2MsgCallback;
	std::mutex m_serialNum2MsgCallbackMutex;
	std::mutex m_msgId2MsgCallbackMutex;

	void onSignal(const SignalInfo& sig) override;
	void onSignalRsp(const SignalInfo& sigRsp) override;
};

} /* namespace NS_FF */

#endif /* APP_APPLICATIONIMPL_H_ */
