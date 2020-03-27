/*
 * DllLoader.h
 *
 *  Created on: 2016-12-16
 *      Author: liyawu
 */

#ifndef DLL_DLLLOADER_H_
#define DLL_DLLLOADER_H_

#include <ff/ff_config.h>
#include <string>

#ifdef _WIN32
#define DLLHANDLE	HMODULE
#else
#define DLLHANDLE	void*
#endif

namespace NS_FF {

class DllLoader {
public:
	DllLoader();
	DllLoader(const std::string& dllPath);
	~DllLoader();

	bool load(const std::string& dllPath);
	bool isLoaded() const;
	bool unload();
	void* getProc(const std::string& procName);
	DLLHANDLE getModuleHandle();
	operator bool() const;
	void* operator()(const std::string& procName);
private:
	DLLHANDLE m_handle;
};

} /* namespace NS_FF */

#endif /* DLL_DLLLOADER_H_ */
