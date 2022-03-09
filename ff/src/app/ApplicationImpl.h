/*
 * ApplicationImpl.h
 *
 *  Created on: Aug 5, 2019
 *      Author: root
 */

#ifndef APP_APPLICATIONIMPL_H_
#define APP_APPLICATIONIMPL_H_

#ifdef _WIN32
#else
#include <unistd.h>
#endif

#include <string>
#include <list>
#include <mutex>
#include <condition_variable>
#include <map>

#include <ff/Application.h>
#include <ff/Settings.h>

NS_FF_BEG

class Application::ApplicationImpl {
public:
	ApplicationImpl(Application* _app, int argc, char** argv);
	virtual ~ApplicationImpl();

	int run();
	void exit(int code);
	int getArgc() const;
	char** getArgv() const;
	int getExitCode() const;
	bool isRunning() const;
	const std::vector<std::string>& getCommandLines() const;
	const std::string& getCommandLine(int index) const;
	int getCommandLineCount() const;

	static std::string GetApplicationPath();
	static std::string GetApplicationDir();
	static std::string GetApplicationName();
	static std::string GetCurrentWorkDir();

private:
	Application* m_app;
	int m_argc;
	char** m_argv;
	std::vector<std::string> m_cmdLines;
	std::mutex m_mutex;
	std::condition_variable m_cond;
	int m_exitCode;
	bool m_running;
};

NS_FF_END

#endif /* APP_APPLICATIONIMPL_H_ */
