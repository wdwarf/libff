/*
 * DllLoader.cpp
 *
 *  Created on: 2016-12-16
 *      Author: liyawu
 */

#include <ff/DllLoader.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

using namespace std;

namespace NS_FF {

DllLoader::DllLoader() :
		m_handle(NULL) {
}

DllLoader::DllLoader(const std::string& dllPath) :
		m_handle(NULL) {
	this->load(dllPath);
}

DllLoader::~DllLoader() {
	this->unload();
}

bool DllLoader::load(const std::string& dllPath) {
	this->unload();
#ifdef _WIN32
	this->m_handle = LoadLibrary(dllPath.c_str());
#else
	this->m_handle = dlopen(dllPath.c_str(), RTLD_LAZY);
#endif
	return (NULL != this->m_handle);
}

bool DllLoader::isLoaded() const {
	return (NULL != this->m_handle);
}

bool DllLoader::unload() {
	if (NULL == this->m_handle)
		return true;

#ifdef _WIN32
	FreeLibrary(this->m_handle);
#else
	dlclose(this->m_handle);
#endif
	this->m_handle = NULL;
	return true;
}

void* DllLoader::getProc(const std::string& procName) {
	if (!this->isLoaded() || procName.empty())
		return NULL;

#ifdef _WIN32
	return GetProcAddress(this->m_handle, procName.c_str());
#else
	return dlsym(this->m_handle, procName.c_str());
#endif
}

DLLHANDLE DllLoader::getModuleHandle() {
	return this->m_handle;
}

DllLoader::operator bool() const{
	return this->isLoaded();
}

void* DllLoader::operator()(const std::string& procName){
	return this->getProc(procName);
}

} /* namespace NS_FF */
