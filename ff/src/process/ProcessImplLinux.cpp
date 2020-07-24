/*
 * ProcessImplLinux.cpp
 *
 *  Created on: 2018年7月2日
 *      Author: admin
 */

#if defined(__linux__) || defined(__CYGWIN32__)

#include "ProcessImplLinux.h"
#include <ff/File.h>
#include <ff/String.h>
#include <ff/Variant.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <functional>

using namespace std;

namespace NS_FF {

Process::ProcessImpl::ProcessImpl(Process *proc, const std::string &command) :
		_proc(proc), pid(0), asyncRead(true), m_exitCode(-1){
	memset(this->pipeFd, 0, sizeof(this->pipeFd));
	this->command = command;
}

Process::ProcessImpl::~ProcessImpl() {
	try {
		this->stop();
	} catch (...) {
	}
}

void Process::ProcessImpl::start() {
	this->stop();

	if (pipe(this->pipeFd) < 0) {
		THROW_EXCEPTION(ProcessException, "process start failed.", errno);
	}

	this->pid = vfork();

	if (0 == this->pid) {
		if (!this->workDir.empty()) {
			chdir(this->workDir.c_str());
		}

		close(this->pipeFd[0]);
		dup2(this->pipeFd[1], STDOUT_FILENO);
		dup2(this->pipeFd[1], STDERR_FILENO);
		close(this->pipeFd[1]);

		char **argv = new char*[args.size() + 2];
		for (size_t i = 0; i < args.size(); ++i) {
			argv[i + 1] = (char*) args[i].c_str();
		}
		argv[0] = (char*) command.c_str();
		argv[args.size() + 1] = NULL;
		int re = execvp(command.c_str(), argv);
		exit(re);
	} else {
		close(this->pipeFd[1]);
		if (this->asyncRead) {
			this->readThread = thread(bind(&Process::ProcessImpl::doReadData, this));
		}
	}
}

void Process::ProcessImpl::stop() {
	if (0 == this->pid)
		return;

	kill(this->pid, SIGABRT);

	if (this->readThread.joinable())
		this->readThread.join();

	close(this->pipeFd[0]);
	this->pid = 0;
	memset(this->pipeFd, 0, sizeof(this->pipeFd));
}

int Process::ProcessImpl::getExitCode() const {
	return this->m_exitCode;
}

int Process::ProcessImpl::waitForFinished() {
	if (0 == this->pid)
		return -1;

	int status = 0;
	waitpid(this->pid, &status, 0);
	this->m_exitCode = WEXITSTATUS(status);
	return this->m_exitCode;
}

int Process::ProcessImpl::getProcessId() const {
	return this->pid;
}

int Process::ProcessImpl::readData(char *buf, int bufLen) {
	if (0 == this->pid || 0 == this->pipeFd[0] || this->asyncRead)
		return -1;

	return read(this->pipeFd[0], buf, bufLen);
}

void Process::ProcessImpl::doReadData() {
	if (0 == this->pid)
		return;

	const int bufLen = 2048;
	char buf[bufLen];
	int readBytes = 0;
	while ((readBytes = read(this->pipeFd[0], buf, bufLen)) > 0) {
		this->_proc->onReadData(buf, readBytes);
	}
}

int Process::ProcessImpl::GetPid() {
	return getpid();
}

int Process::ProcessImpl::GetPPid() {
	return getppid();
}

int Process::ProcessImpl::Exec(const std::string &command, bool wait) {
	int pid = vfork();

	if (0 == pid) {
		int re = execlp(command.c_str(), command.c_str(), NULL);
		exit(re);
	} else {
		if (wait) {
			int status = 0;
			waitpid(pid, &status, 0);
		}
	}

	return pid;
}

int Process::ProcessImpl::GetPidByName(const std::string &processName) {
	File proc("/proc");
	auto it = proc.iterator();
	while(it.next()){
		auto currFile = it.getFile();
		File status(currFile, "status");
		if (!status.isExists())
			continue;

		fstream f;
		f.open(status.getPath().c_str(), ios::in);
		if (!f.is_open())
			continue;

		string name;
		while (!f.eof()) {
			string line;
			std::getline(f, line);
			if (0 != line.find("Name"))
				continue;
			size_t pos = line.find(":");
			if (string::npos == pos)
				continue;
			name = TrimCopy(line.substr(pos + 1));
			break;
		}
		if (name == processName) {
			return Variant(currFile.getName());
		}
	}
	return -1;
}

bool Process::ProcessImpl::Kill(int pid, int code) {
	return (0 == ::kill(pid, code));
}

bool Process::ProcessImpl::Kill(const std::string &processName, int code) {
	int pid = Process::GetPidByName(processName);
	if (pid <= 0)
		return false;
	return Process::ProcessImpl::Kill(pid, code);
}

std::map<int, std::string> Process::ProcessImpl::ListProcesses() {
	std::map<int, std::string> re;
	File proc("/proc");
	auto it = proc.iterator();
	while(it.next()){
		auto currFile = it.getFile();
		File status(currFile, "status");
		if (!status.isExists())
			continue;

		fstream f;
		f.open(status.getPath().c_str(), ios::in);
		if (!f.is_open())
			continue;

		while (!f.eof()) {
			string line;
			std::getline(f, line);
			if (0 != line.find("Name"))
				continue;
			size_t pos = line.find(":");
			if (string::npos == pos)
				continue;
			string name = TrimCopy(line.substr(pos + 1));
			int pid = Variant(currFile.getName());
			re.insert(make_pair(pid, name));
			break;
		}
	}
	return re;
}

const std::string& Process::ProcessImpl::getCommand() const {
	return command;
}

void Process::ProcessImpl::setCommand(const std::string &command) {
	this->command = command;
}

bool Process::ProcessImpl::isAsyncRead() const {
	return asyncRead;
}

void Process::ProcessImpl::setAsyncRead(bool asyncRead) {
	if (0 != this->pid) {
		THROW_EXCEPTION(ProcessException, "process is running.", -1);
	}
	this->asyncRead = asyncRead;
}

const std::string& Process::ProcessImpl::getWorkDir() const {
	return workDir;
}

void Process::ProcessImpl::setWorkDir(const std::string &workDir) {
	this->workDir = workDir;
}

void Process::ProcessImpl::addParameter(const std::string &arg) {
	this->args.push_back(arg);
}

const vector<string>& Process::ProcessImpl::getParameters() const {
	return this->args;
}

void Process::ProcessImpl::clearParameter() {
	this->args.clear();
}

} /* namespace NS_FF */

#endif
