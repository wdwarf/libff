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
#include <vector>
#include <string>
#include <set>

namespace NS_FF {

EXCEPTION_DEF(ApplicationException);

class FFDLL Application: public Object {
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

	static std::string GetApplicationName();
	static std::string GetApplicationPath();
	static std::string GetCurrentWorkDir();

private:
	class ApplicationImpl;
	ApplicationImpl* impl;
};

FFDLL Application* GetApplication();

} /* namespace NS_FF */

#endif /* FF_APPLICATION_H_ */
