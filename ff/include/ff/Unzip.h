/*
 * Zip.h
 *
 *  Created on: Nov 23, 2018
 *      Author: ducky
 */

#ifndef FF_COMPRESS_UNZIP_H_
#define FF_COMPRESS_UNZIP_H_

#include <ff/Exception.h>
#include <ff/Object.h>

#include <ctime>
#include <string>
#include <vector>

NS_FF_BEG

struct LIBFF_API ZipFileInfo {
  std::string name;
  uint64_t uncompressedSize;
  uint64_t compressedSize;
  std::string method;
  uint64_t ratio;
  uint64_t crc;
  struct {
    uint32_t sec;
    uint32_t min;
    uint32_t hour;
    uint32_t mday;
    uint32_t mon;
    uint32_t year;
  } date;
};

class LIBFF_API Unzip {
 public:
  Unzip(const std::string& filePath);
  ~Unzip();

  std::vector<ZipFileInfo> list() const;
  bool list(std::vector<ZipFileInfo>& v) const;
  bool unzipTo(const std::string& dir = "", bool overwrite = true,
               const std::string& password = "");
  bool unzipTo(const std::vector<std::string>& files,
               const std::string& dir = "", bool overwrite = true,
               const std::string& password = "");

  const std::string& getFilePath() const;

 private:
  std::string m_filePath;
};

NS_FF_END

#endif /* FF_COMPRESS_UNZIP_H_ */
