/*
 * ApplicationImpl2.cpp
 *
 *  Created on: Aug 5, 2019
 *      Author: root
 */

#include "ApplicationImpl.h"
#include <ff/File.h>
#include <mutex>
#include <iostream>

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
	this->m_app->onRun();

	thread eventThread([&] {
		while (this->m_running) {
			this->update(100);
		}
	});
	eventThread.join();

	this->m_app->onUninitialize();

	std::lock_guard<mutex> lk(this->m_mutex);
	this->m_running = false;
	return this->m_exitCode;
}

void Application::ApplicationImpl::exit(int code) {
	std::lock_guard<mutex> lk(this->m_mutex);

	if (!this->m_running)
		return;

	this->signal(APP_MSG_EXIT, code);
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

void Application::ApplicationImpl::sendMessage(uint32_t msgId, uint32_t msgData,
		MsgHandler callBack, int32_t timeoutMs) {
	auto serialNum = this->signal(msgId, msgData);
	if (callBack) {
		std::lock_guard<mutex> lk(this->m_serialNum2MsgCallbackMutex);
		this->m_serialNum2MsgCallback.insert(make_pair(serialNum, callBack));
	}
}

void Application::ApplicationImpl::responseMessage(uint32_t serialNum, uint64_t msgId, uint64_t msgData){
	this->signalRsp(serialNum, msgId, msgData);
}

void Application::ApplicationImpl::subscribeMsgHandler(uint32_t msgId,
		MsgHandler handler) {
	std::lock_guard<mutex> lk(this->m_msgId2MsgCallbackMutex);
	this->m_msgId2MsgCallback.insert(make_pair(msgId, handler));
}

void Application::ApplicationImpl::onSignal(const SignalInfo& sig) {
	switch (sig.sigId) {
	case APP_MSG_EXIT: {
		if (!this->m_running)
			return;
		std::lock_guard<mutex> lk(this->m_mutex);
		this->m_exitCode = sig.sigEvent;
		this->m_running = false;
		break;
	}
	default: {
		std::lock_guard<mutex> lk(this->m_msgId2MsgCallbackMutex);
	    auto callbackRange = this->m_msgId2MsgCallback.equal_range(sig.sigId);
	    if(callbackRange.first != this->m_msgId2MsgCallback.end())
	    {
	        for (auto it = callbackRange.first ; it != callbackRange.second; ++it)
	        	it->second(sig.serialNum, sig.sigId, sig.sigEvent);
	    }
	    break;
	}
	}
}

void Application::ApplicationImpl::onSignalRsp(const SignalInfo& sigRsp) {
	MsgHandler func;
	{
		std::lock_guard<mutex> lk(this->m_serialNum2MsgCallbackMutex);
		auto it = this->m_serialNum2MsgCallback.find(sigRsp.serialNum);
		if (it == this->m_serialNum2MsgCallback.end())
			return;
		func = it->second;
		this->m_serialNum2MsgCallback.erase(it);
	}

	if(func)
		func(sigRsp.serialNum, sigRsp.sigId, sigRsp.sigEvent);
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
