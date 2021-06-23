/*
 * Application.cpp
 *
 *  Created on: 2018年7月18日
 *      Author: liyawu
 */

#include <ff/Application.h>
#include "ApplicationImpl.h"
#include <cassert>
#include <vector>
#include <string>
#include <set>

using namespace std;

NS_FF_BEG

static Application* __app = nullptr;

Application* GetApp() {
	return __app;
}

Application::Application(int argc, char** argv) :
		impl(new Application::ApplicationImpl(this, argc, argv)) {
	assert(nullptr == __app);
	if (nullptr != __app) {
		THROW_EXCEPTION(ApplicationException,
				"There is anther application on running.", -1);
	}
	__app = this;
}

Application::~Application() {
	if (this == __app) {
		__app = nullptr;
	}
	delete this->impl;
}

int Application::run() {
	return this->impl->run();
}

const std::vector<std::string>& Application::getCommandLines() const {
	return this->impl->getCommandLines();
}

const std::string& Application::getCommandLine(int index) const {
	return this->impl->getCommandLine(index);
}

int Application::getCommandLineCount() const {
	return this->impl->getCommandLineCount();
}

void Application::exit(int code) {
	this->impl->exit(code);
}

int Application::getArgc() const {
	return this->impl->getArgc();
}

char** Application::getArgv() const {
	return this->impl->getArgv();
}

int Application::getExitCode() const {
	return this->impl->getExitCode();
}

std::string Application::GetApplicationName() {
	return Application::ApplicationImpl::GetApplicationName();
}

std::string Application::GetApplicationPath() {
	return Application::ApplicationImpl::GetApplicationPath();
}

std::string Application::GetCurrentWorkDir() {
	return Application::ApplicationImpl::GetCurrentWorkDir();
}

NS_FF_END /* namespace NS_FF */

