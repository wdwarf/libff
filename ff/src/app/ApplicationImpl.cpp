/*
 * ApplicationImpl2.cpp
 *
 *  Created on: Aug 5, 2019
 *      Author: root
 */

#include "ApplicationImpl.h"
#include <ff/File.h>
#include <mutex>

using namespace std;

namespace NS_FF {

Application::ApplicationImpl::ApplicationImpl(Application* _app, int argc,
		char** argv) :
		m_app(_app), m_exitCode(0), m_running(false) {
	for (int i = 0; i < argc; ++i) {
		this->m_cmdLines.push_back(argv[i]);
	}
}

Application::ApplicationImpl::~ApplicationImpl() {
}

int Application::ApplicationImpl::run() {
	{
		std::lock_guard<mutex> lk(this->m_mutex);

		if (this->m_running)
			return -1;
		this->m_exitCode = 0;
		this->m_running = true;
	}

	this->m_app->onInitialize();
	this->m_app->run();

	{
		std::unique_lock<mutex> lk(this->m_mutex);
		this->m_cond.wait(lk, [&]{ return (false == this->m_running); });
	}

	this->m_app->onUninitialize();

	std::lock_guard<mutex> lk(this->m_mutex);
	this->m_running = false;
	return this->m_exitCode;
}

void Application::ApplicationImpl::exit(int code) {
	std::lock_guard<mutex> lk(this->m_mutex);

	if (!this->m_running)
		return;
	this->m_running = false;
	this->m_exitCode = code;
	this->m_cond.notify_one();
}

int Application::ApplicationImpl::getExitCode() const {
	return this->m_exitCode;
}

const std::vector<std::string>& Application::ApplicationImpl::getCommandLines() const {
	return this->m_cmdLines;
}

const std::string& Application::ApplicationImpl::getCommandLine(
		int index) const {
	return this->m_cmdLines[index];
}

int Application::ApplicationImpl::getCommandLineCount() const {
	return this->m_cmdLines.size();
}

const Variant& Application::ApplicationImpl::getValue(
		const std::string& key) const {
	return this->m_settings.getValue(key);
}
void Application::ApplicationImpl::setValue(const std::string& key,
		const Variant& value) {
	this->m_settings.setValue(key, value);
}
bool Application::ApplicationImpl::hasValue(const std::string& key) const {
	return this->m_settings.hasValue(key);
}
void Application::ApplicationImpl::removeValue(const std::string& key) {
	this->m_settings.removeValue(key);
}
std::set<std::string> Application::ApplicationImpl::getKeys() const {
	return this->m_settings.getKeys();
}
void Application::ApplicationImpl::saveSettings(const std::string& file) {
	this->m_settings.saveToFile(file);
}
void Application::ApplicationImpl::loadSettings(const std::string& file) {
	this->m_settings.loadFromFile(file);
}

std::string Application::ApplicationImpl::GetApplicationPath() {
	char path[256] = { 0 };
	if (::readlink("/proc/self/exe", path, 256) <= 0) {
		return "";
	}
	return path;
}

std::string Application::ApplicationImpl::GetApplicationName() {
	return File(Application::ApplicationImpl::GetApplicationPath()).getName();
}

std::string Application::ApplicationImpl::GetCurrentWorkDir() {
	return ::getcwd(NULL, 0);
}

} /* namespace NS_FF */
