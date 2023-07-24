/*
 * Process.cpp
 *
 *  Created on: Feb 5, 2017
 *      Author: ducky
 */
#include <ff/Process.h>
#include "ProcessImpl.h"

using namespace std;

NS_FF_BEG

Process::Process(const std::string& command) :
		impl(new ProcessImpl(this, command)) {
}

Process::~Process() {
	delete this->impl;
}

void Process::start() {
	return this->impl->start();
}

void Process::stop() {
	this->impl->stop();
}

int Process::waitForFinished() {
	return this->impl->waitForFinished();
}

int Process::getExitCode() const {
	return this->impl->getExitCode();
}

int Process::GetPid() {
	return Process::ProcessImpl::GetPid();
}

int Process::GetPPid() {
	return Process::ProcessImpl::GetPPid();
}

int Process::Exec(const std::string& command, bool wait) {
	return Process::ProcessImpl::Exec(command, wait);
}

int Process::GetPidByName(const std::string& processName) {
	return Process::ProcessImpl::GetPidByName(processName);
}

bool Process::Kill(int pid, int code) {
	return Process::ProcessImpl::Kill(pid, code);
}

bool Process::Kill(const std::string& processName, int code) {
	return Process::ProcessImpl::Kill(processName, code);
}

std::map<int, std::string> Process::ListProcesses(){
	return Process::ProcessImpl::ListProcesses();
}

const std::string& Process::getCommand() const {
	return this->impl->getCommand();
}

void Process::setCommand(const std::string& command) {
	this->impl->setCommand(command);
}

int Process::readData(char* buf, int bufLen) {
	return this->impl->readData(buf, bufLen);
}

int Process::writeData(const char* buf, int bufLen) {
	return this->impl->writeData(buf, bufLen);
}

bool Process::isAsyncRead() const {
	return this->impl->isAsyncRead();
}

void Process::setAsyncRead(bool asyncRead) {
	this->impl->setAsyncRead(asyncRead);
}

const std::string& Process::getWorkDir() const {
	return this->impl->getWorkDir();
}

void Process::setWorkDir(const std::string& workDir) {
	this->impl->setWorkDir(workDir);
}

void Process::addParameter(const std::string& arg) {
	this->impl->addParameter(arg);
}

const vector<string>& Process::getParameters() const {
	return this->impl->getParameters();
}

void Process::clearParameter() {
	this->impl->clearParameter();
}

int Process::getProcessId() const {
	return this->impl->getProcessId();
}

NS_FF_END

