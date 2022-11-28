/*
 * ApplicationImpl2.cpp
 *
 *  Created on: Aug 5, 2019
 *      Author: root
 */

#include "ApplicationImpl.h"

#include <ff/File.h>

#include <iostream>
#include <mutex>

#ifdef _WIN32
#include <direct.h>

#define _getcwd_ _getcwd
#else
#define _getcwd_ getcwd
#endif

using namespace std;

NS_FF_BEG

Application::ApplicationImpl::ApplicationImpl(Application* _app, int argc,
                                              char** argv)
    : m_app(_app), m_argc(argc), m_argv(argv), m_exitCode(0), m_running(false) {
  for (int i = 0; i < argc; ++i) {
    this->m_cmdLines.push_back(argv[i]);
  }
}

Application::ApplicationImpl::~ApplicationImpl() {}

int Application::ApplicationImpl::getArgc() const { return this->m_argc; }

char** Application::ApplicationImpl::getArgv() const { return this->m_argv; }

int Application::ApplicationImpl::run() {
  {
    std::lock_guard<std::mutex> lk(this->m_mutex);

    if (this->m_running) return -1;
    this->m_exitCode = 0;
    this->m_running = true;
  }

  this->m_app->onInitialize();
  this->m_app->onRun();

  {
    std::unique_lock<std::mutex> lk(this->m_mutex);
    this->m_cond.wait(lk, [&] { return (false == this->m_running); });
  }

  this->m_app->onUninitialize();

  std::lock_guard<std::mutex> lk(this->m_mutex);
  this->m_running = false;
  return this->m_exitCode;
}

void Application::ApplicationImpl::exit(int code) {
  std::lock_guard<std::mutex> lk(this->m_mutex);

  if (!this->m_running) return;
  this->m_running = false;
  this->m_exitCode = code;
  this->m_cond.notify_one();
}

int Application::ApplicationImpl::getExitCode() const {
  return this->m_exitCode;
}

bool Application::ApplicationImpl::isRunning() const { return this->m_running; }

const std::vector<std::string>& Application::ApplicationImpl::getCommandLines()
    const {
  return this->m_cmdLines;
}

const std::string& Application::ApplicationImpl::getCommandLine(
    int index) const {
  return this->m_cmdLines[index];
}

int Application::ApplicationImpl::getCommandLineCount() const {
  return this->m_cmdLines.size();
}

std::string Application::ApplicationImpl::GetApplicationPath() {
#ifdef _WIN32
  TCHAR cPath[1024] = {0};
  ::ZeroMemory(cPath, 1024);
  GetModuleFileName(NULL, cPath, 1024);
  return cPath;
#else
  char path[256] = {0};
  if (::readlink("/proc/self/exe", path, 256) <= 0) {
    return "";
  }
  return path;
#endif
}

std::string Application::ApplicationImpl::GetApplicationDir() {
  return File(GetApplicationPath()).getParent().getPath();
}

std::string Application::ApplicationImpl::GetApplicationName() {
  auto path = Application::ApplicationImpl::GetApplicationPath();
  auto pos = path.find_last_of('/');
  if (string::npos == pos) {
    pos = path.find_last_of('\\');
    if (string::npos == pos) return path;
  }

  return path.substr(pos + 1);
}

std::string Application::ApplicationImpl::GetCurrentWorkDir() {
  return _getcwd_(NULL, 0);
}

NS_FF_END /* namespace NS_FF */
