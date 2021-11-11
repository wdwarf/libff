/*
 * Zip.h
 *
 *  Created on: Nov 23, 2018
 *      Author: ducky
 */

#ifndef FF_COMPRESS_ZIP_H_
#define FF_COMPRESS_ZIP_H_

#include <ff/Exception.h>
#include <ff/File.h>
#include <ff/Object.h>
#include <ff/ZipEntry.h>

#include <istream>
#include <string>

NS_FF_BEG

class LIBFF_API ZipBuffer {
 public:
  ZipBuffer(const void *data, uint32_t size);

  const void *data() const;
  uint32_t size() const;

 private:
  const void *m_data;
  uint32_t m_size;
};

class LIBFF_API Zip {
 public:
  Zip(const std::string &filePath);
  virtual ~Zip();

  void createNew() _throws(Exception);
  void open() _throws(Exception);
  void close();
  bool isOpened() const;

  Zip &operator<<(const ZipEntry &entry) _throws(Exception);
  Zip &operator<<(const ff::File &file) _throws(Exception);
  Zip &operator<<(std::istream &stream) _throws(Exception);
  Zip &operator<<(const ff::ZipBuffer &buffer) _throws(Exception);
  Zip &write(const void *data, uint32_t size) _throws(Exception);

  /**
   * 将源文件/目录src以名称newFileName加入到parentEntry里去
   */
  Zip &zip(const std::string &src, const std::string &parentEntry = "",
           const std::string &newFileName = "") _throws(Exception);

  const std::string &getFilePath() const;

 private:
  class ZipImpl;
  ZipImpl *impl;
};

NS_FF_END

#endif /* FF_COMPRESS_ZIP_H_ */
