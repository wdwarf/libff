/**
 * @file PeFile.h
 * @auth DuckyLi
 * @date 2023-03-01 21:49:07
 * @description
 */

#ifndef _PEFILE_H_
#define _PEFILE_H_

#include <ff/ff_config.h>
#include <windows.h>

#include <string>

NS_FF_BEG

class LIBFF_API PeFile {
 public:
  PeFile();
  ~PeFile();

  bool open(const std::string& filePath);
  void close();

  const IMAGE_NT_HEADERS* getNtHeader() const;

 private:
  HANDLE m_fileHandle;
  HANDLE m_fileMap;
  void* m_fileMem;
};

NS_FF_END

#endif /** _PEFILE_H_ */
