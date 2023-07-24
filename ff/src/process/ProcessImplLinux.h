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

NS_FF_BEG

class Process::ProcessImpl {
public:

	ProcessImpl(Process* proc, const std::string& command);
	~ProcessImpl();

	void start();
	void stop();
	int waitForFinished();
	int getProcessId() const;
	int getExitCode() const;
	const std::string& getCommand() const;
	void setCommand(const std::string& command);
	int readData(char* buf, int bufLen);
	int writeData(const char* buf, int bufLen);
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
	int pipeFdRead[2];
	int pipeFdWrite[2];
	int pid;
	int m_exitCode;
	bool asyncRead;
	std::string workDir;
	std::vector<std::string> args;

	std::thread readThread;
};

NS_FF_END

#endif /* FF_PROCESSIMPLLINUX_H_ */

#endif
