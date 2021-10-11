/*
 * Zip.h
 *
 *  Created on: Nov 23, 2018
 *      Author: ducky
 */

#ifndef FF_COMPRESS_ZIP_H_
#define FF_COMPRESS_ZIP_H_

#include <ff/Exception.h>
#include <ff/Object.h>
#include <ff/ZipEntry.h>

#include <string>

NS_FF_BEG

class LIBFF_API Zip {
 public:
  Zip(const std::string &filePath);
  virtual ~Zip();

  void createNew() _throws(Exception);
  void open() _throws(Exception);
  void close();
  bool isOpened() const;

  // 设定当前entry
  Zip &operator<<(const ZipEntry &entry) _throws(Exception);
  /**
   * 添加一个文件file到压缩包
   * 文件会按原名称加入到当前entry，同zip(file);
   */
  Zip &operator<<(const std::string &file) _throws(Exception);

  /**
   * 将源文件/目录src以名称newFileName加入到entry里去
   */
  Zip &zip(const std::string &src, const std::string &entry = "",
           const std::string &newFileName = "") _throws(Exception);

  const std::string &getFilePath() const;

 private:
  class ZipImpl;
  ZipImpl *impl;
};

NS_FF_END

#endif /* FF_COMPRESS_ZIP_H_ */
