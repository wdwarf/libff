/*
 * Application.h
 *
 *  Created on: 2018年7月18日
 *      Author: liyawu
 */

#ifndef FF_APPLICATION_H_
#define FF_APPLICATION_H_

#include <ff/Object.h>
#include <ff/Exception.h>
#include <ff/Variant.h>
#include <cstdint>
#include <vector>
#include <string>
#include <set>
#include <functional>

namespace NS_FF {

typedef std::function<void (uint32_t serialNum, uint64_t msgId, uint64_t msgData)> MsgHandler;

#define APP_MSG_USER			0xFFFFFFFF
#define APP_MSG_EXIT			0x00000001

EXCEPTION_DEF(ApplicationException);

class LIBFF_API Application: public Object {
public:
	Application(int argc, char** argv);
	virtual ~Application();

	int run();
	void exit(int code = 0);
	int getExitCode() const;

	virtual void onInitialize(){}
	virtual void onRun() = 0;
	virtual void onUninitialize(){}

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

	static std::string GetApplicationName();
	static std::string GetApplicationPath();
	static std::string GetCurrentWorkDir();

private:
	class ApplicationImpl;
	ApplicationImpl* impl;
};

LIBFF_API Application* GetApp();

} /* namespace NS_FF */

#endif /* FF_APPLICATION_H_ */
