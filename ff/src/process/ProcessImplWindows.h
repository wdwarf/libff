/*
 * ProcessImplWindows.h
 *
 *  Created on: 2018年7月2日
 *      Author: admin
 */

#if defined(WIN32) || defined(__MINGW32__)
#ifndef FF_PROCESSIMPLWINDOWS_H_
#define FF_PROCESSIMPLWINDOWS_H_

#include <ff/Process.h>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <Windows.h>

using namespace std;

namespace NS_FF {

class Process::ProcessImpl {
public:

	ProcessImpl(Process* proc, const std::string& command);
	~ProcessImpl();

	void start();
	void stop();
	void waitForFinished();
	int getProcessId() const;
	const std::string& getCommand() const;
	void setCommand(const std::string& command);
	int readData(char* buf, int bufLen);
	bool isAsyncRead() const;
	void setAsyncRead(bool asyncRead);
	const std::string& getWorkDir() const;
	void setWorkDir(const std::string& workDir);

	static int GetPid();
	static int GetPPid();
	static int Exec(const std::string& command, bool wait);
	static int GetPidByName(const std::string& processName);
	static bool Kill(int pid, int code = 15);
	static bool Kill(const std::string& processName, int code = 15);
	static std::map<int, std::string> Process::ListProcesses();

private:
	void doReadData();
	void watchTerminated();
	Process* _proc;
	std::string command;
	bool asyncRead;
	std::string workDir;
	HANDLE hRead;
	HANDLE hWrite;
	PROCESS_INFORMATION pi;

	std::thread readThread;
	std::thread watchThread;
};

} /* namespace NS_FF */

#endif /* FF_PROCESSIMPLWINDOWS_H_ */

#endif
