/*
 * Process.h
 *
 *  Created on: Feb 5, 2017
 *      Author: ducky
 */
#ifndef FF_PROCESS_H_
#define FF_PROCESS_H_

#include <ff/ff_config.h>
#include <ff/Object.h>
#include <ff/Exception.h>
#include <ff/Noncopyable.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace NS_FF {

EXCEPTION_DEF(ProcessException);

class Process: protected Noncopyable, public Object {
public:
	Process(const std::string &command);
	virtual ~Process();

	void start();
	void stop();
	void waitForFinished();
	int getProcessId() const;
	const std::string& getCommand() const;
	void setCommand(const std::string &command);
	int readData(char *buf, int bufLen);
	bool isAsyncRead() const;
	void setAsyncRead(bool asyncRead);
	const std::string& getWorkDir() const;
	void setWorkDir(const std::string &workDir);
	void addParameter(const std::string &arg);
	const std::vector<std::string>& getParameters() const;
	void clearParameter();

	virtual void onReadData(const char *buf, int bufLen) {
	}

public:
	static int GetPid();
	static int GetPPid();
	static int Exec(const std::string &command, bool wait = false);
	static int GetPidByName(const std::string &processName);
	static bool Kill(int pid, int code = 15);
	static bool Kill(const std::string &processName, int code = 15);
	static std::map<int, std::string> ListProcesses();

private:
	class ProcessImpl;
	ProcessImpl *impl;

};

typedef std::shared_ptr<Process> ProcessPtr;

} /* namespace NS_FF */

#endif /* FF_PROCESS_H_ */
