/*
 * DllLoader.h
 *
 *  Created on: 2016-12-16
 *      Author: liyawu
 */

#ifndef DLL_DLLLOADER_H_
#define DLL_DLLLOADER_H_

#include <string>
#include <memory>
#include <ff/ff_config.h>
#include <ff/Noncopyable.h>
#include <ff/Exception.h>

#ifdef _WIN32
#define DLLHANDLE	HMODULE
#else
#define DLLHANDLE	void*
#endif

namespace NS_FF {

EXCEPTION_DEF(DllException);

/**
 * @bref 动态库加载器
 */
class DllLoader: public Noncopyable {
public:
	DllLoader();
	DllLoader(const std::string &dllPath) _throws(DllException);
	~DllLoader();

	/**
	 * @bref 加载dll/so
	 */
	void load(const std::string &dllPath) _throws(DllException);

	/**
	 * @bref 是否已加载
	 */
	bool isLoaded() const;

	/**
	 * @bref 卸载dll/so
	 */
	void unload();

	/**
	 * @bref 获取一个入口
	 */
	void* getProc(const std::string &procName);

	/**
	 * @bref 获取dll/so句柄
	 */
	DLLHANDLE getModuleHandle();

	/**
	 * @bref 同isLoaded
	 * ＠see isLoaded
	 */
	operator bool() const;

	/**
	 * @bref 同load
	 * @see load
	 */
	void* operator()(const std::string &procName);

private:
	DLLHANDLE m_handle;	/** 打开的句柄 */
};

using DllLoaderPtr = std::shared_ptr<DllLoader>;

} /* namespace NS_FF */

#endif /* DLL_DLLLOADER_H_ */
