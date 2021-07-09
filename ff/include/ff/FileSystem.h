#ifndef _FF_FILESYSTEM_H_
#define _FF_FILESYSTEM_H_

#include <ff/ff_config.h>

NS_FF_BEG

struct SpaceInfo {
  uint64_t total;
  uint64_t free;
  uint64_t available;
  uint64_t used;

  SpaceInfo();
};

class FileSystem {
 public:
  FileSystem(const std::string& path);
  ~FileSystem();

  // disk free
  SpaceInfo df();

 private:
  std::string m_path;
};

NS_FF_END

#endif
