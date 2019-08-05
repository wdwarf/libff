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

namespace NS_FF {

static Application* __app = NULL;

Application* GetApplication() {
	return __app;
}

Application::Application(int argc, char** argv) :
		impl(new Application::ApplicationImpl(this, argc, argv)) {
	assert(NULL == __app);
	if (NULL != __app) {
		THROW_EXCEPTION(ApplicationException,
				"There is anther application on running.", -1);
	}
	__app = this;
}

Application::~Application() {
	if (this == __app) {
		__app = NULL;
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

const Variant& Application::getValue(
		const std::string& key) const {
	return this->impl->getValue(key);
}

void Application::setValue(const std::string& key,
		const Variant& value) {
	this->impl->setValue(key, value);
}

bool Application::hasValue(const std::string& key) const {
	return this->impl->hasValue(key);
}

void Application::removeValue(const std::string& key) {
	this->impl->removeValue(key);
}

std::set<std::string> Application::getKeys() const {
	return this->impl->getKeys();
}

void Application::saveSettings(const std::string& file) {
	this->impl->saveSettings(file);
}

void Application::loadSettings(const std::string& file) {
	this->impl->loadSettings(file);
}

void Application::exit(int code) {
	this->impl->exit(code);
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

} /* namespace NS_FF */
