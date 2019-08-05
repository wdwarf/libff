/*
 * ApplicationImpl.h
 *
 *  Created on: Aug 5, 2019
 *      Author: root
 */

#ifndef APP_APPLICATIONIMPL_H_
#define APP_APPLICATIONIMPL_H_

#include <ff/Application.h>
#include <ff/Settings.h>
#include <ff/Semaphore.h>
#include <string>
#include <list>
#include <mutex>
#include <unistd.h>

namespace NS_FF {

class Application::ApplicationImpl: public Settings {
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

	static std::string GetApplicationPath();
	static std::string GetApplicationName();
	static std::string GetCurrentWorkDir();

private:
	Application* m_app;
	Settings m_settings;
	std::vector<std::string> m_cmdLines;
	std::mutex m_mutex;
	Semaphore m_sem;
	int m_exitCode;
	bool m_running;
};

} /* namespace NS_FF */

#endif /* APP_APPLICATIONIMPL_H_ */
