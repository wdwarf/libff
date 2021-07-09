#include <ff/FileSystem.h>

#include <cstring>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/statfs.h>
#endif

using namespace std;

NS_FF_BEG

SpaceInfo::SpaceInfo() { memset(this, 0, sizeof(SpaceInfo)); }

FileSystem::FileSystem(const std::string& path) : m_path(path) {}

FileSystem::~FileSystem() {}

SpaceInfo FileSystem::df() {
  SpaceInfo si;

#ifdef _WIN32
  ULARGE_INTEGER uiFreeBytesAvailableToCaller;
  ULARGE_INTEGER uiTotalNumberOfBytes;
  ULARGE_INTEGER uiTotalNumberOfFreeBytes;
  if (GetDiskFreeSpaceExA(this->m_path.c_str(), &uiFreeBytesAvailableToCaller,
                          &uiTotalNumberOfBytes, &uiTotalNumberOfFreeBytes)) {
    si.total = uiTotalNumberOfBytes.QuadPart;
    si.free = uiTotalNumberOfFreeBytes.QuadPart;
    si.available = uiFreeBytesAvailableToCaller.QuadPart;
  }
#else
  struct statfs diskInfo;
  memset(&diskInfo, 0, sizeof(diskInfo));
  if (0 == statfs(this->m_path.c_str(), &diskInfo)) {
    uint64_t blocksize = diskInfo.f_bsize;
    si.total = blocksize * diskInfo.f_blocks;
    si.free = diskInfo.f_bfree * blocksize;
    si.available = diskInfo.f_bavail * blocksize;
  }
#endif
  si.used = si.total - si.available;

  return si;
}

NS_FF_END
