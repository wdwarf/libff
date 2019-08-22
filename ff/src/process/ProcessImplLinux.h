/*
 * ProcessImplLinux.h
 *
 *  Created on: 2018年7月2日
 *      Author: admin
 */

#if defined(__linux__) || defined(__CYGWIN32__)

#ifndef FF_PROCESSIMPLLINUX_H_
#define FF_PROCESSIMPLLINUX_H_

#include <ff/Process.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <thread>

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
	void addParameter(const std::string& arg);
	const std::vector<std::string>& getParameters() const;
	void clearParameter();

	static int GetPid();
	static int GetPPid();
	static int Exec(const std::string& command, bool wait);
	static int GetPidByName(const std::string& processName);
	static bool Kill(int pid, int code = 15);
	static bool Kill(const std::string& processName, int code = 15);
	static std::map<int, std::string> ListProcesses();

private:
	void doReadData();
	Process* _proc;
	std::string command;
	int pipeFd[2];
	int pid;
	bool asyncRead;
	std::string workDir;
	std::vector<std::string> args;

	std::thread readThread;
};

} /* namespace NS_FF */

#endif /* FF_PROCESSIMPLLINUX_H_ */

#endif
