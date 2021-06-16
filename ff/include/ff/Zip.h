/*
 * Zip.h
 *
 *  Created on: Nov 23, 2018
 *      Author: ducky
 */

#ifndef FF_COMPRESS_ZIP_H_
#define FF_COMPRESS_ZIP_H_

#include <ff/Object.h>
#include <ff/ZipEntry.h>
#include <string>

NS_FF_BEG

class Zip {
public:
	Zip(const std::string& filePath);
	virtual ~Zip();

	void createNew();
	void open();
	void close();
	bool isOpened() const;

	// 设定当前entry
	Zip& operator <<(const ZipEntry& entry);
	/**
	 * 添加一个文件file到压缩包
	 * 文件会按原名称加入到当前entry，同zip(file);
	 */
	Zip& operator <<(const std::string& file);

	/**
	 * 将源文件/目录src以名称newFileName加入到entry里去
	 */
	Zip& zip(const std::string& src, const std::string& entry = "",
			const std::string& newFileName = "");

	const std::string& getFilePath() const;

private:
	class ZipImpl;
	ZipImpl* impl;
};

NS_FF_END

#endif /* FF_COMPRESS_ZIP_H_ */
