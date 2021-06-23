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
#include <ff/Settings.h>
#include <cstdint>
#include <vector>
#include <string>
#include <set>
#include <functional>

NS_FF_BEG

EXCEPTION_DEF(ApplicationException);

class LIBFF_API Application: public Settings {
public:
	Application(int argc, char** argv);
	virtual ~Application();

	int run();
	void exit(int code = 0);
	int getExitCode() const;
	int getArgc() const;
	char** getArgv() const;

	virtual void onInitialize(){}
	virtual void onRun() = 0;
	virtual void onUninitialize(){}

	const std::vector<std::string>& getCommandLines() const;
	const std::string& getCommandLine(int index) const;
	int getCommandLineCount() const;

	static std::string GetApplicationName();
	static std::string GetApplicationPath();
	static std::string GetCurrentWorkDir();

private:
	class ApplicationImpl;
	ApplicationImpl* impl;
};

LIBFF_API Application* GetApp();

NS_FF_END

#endif /* FF_APPLICATION_H_ */
