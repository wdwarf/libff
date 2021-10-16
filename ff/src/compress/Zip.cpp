/*
 * Zip.cpp
 *
 *  Created on: Nov 23, 2018
 *      Author: ducky
 */

#include "minizip/zip.h"

#include <fcntl.h>
#include <ff/Exception.h>
#include <ff/File.h>
#include <ff/Zip.h>
#include <zlib.h>

#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>
#endif

#ifdef _WIN32
#define USEWIN32IOAPI
#include "iowin32.h"
#endif

using namespace std;

NS_FF_BEG

#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)

#ifdef _WIN32
static uLong GetFileTime(
    const char *f, /* name of file to get info on */
    tm_zip *tmzip, /* return value: access, modific. and creation times */
    uLong *dt /* dostime */) {
  int ret = 0;
  {
    FILETIME ftLocal;
    WIN32_FIND_DATAA ff32;

    HANDLE hFind = FindFirstFileA(f, &ff32);
    if (hFind != INVALID_HANDLE_VALUE) {
      FileTimeToLocalFileTime(&(ff32.ftLastWriteTime), &ftLocal);
      FileTimeToDosDateTime(&ftLocal, ((LPWORD)dt) + 1, ((LPWORD)dt) + 0);
      FindClose(hFind);
      ret = 1;
    }
  }
  return ret;
}
#else
static uLong GetFileTime(
    const char *f, /* name of file to get info on */
    tm_zip *tmzip, /* return value: access, modific. and creation times */
    uLong *dt      /* dostime */
) {
  int ret = 0;
  struct stat s; /* results of stat() */
  struct tm *filedate;
  time_t tm_t = 0;

  if (strcmp(f, "-") != 0) {
    char name[MAXFILENAME + 1];
    int len = strlen(f);
    if (len > MAXFILENAME) len = MAXFILENAME;

    strncpy(name, f, MAXFILENAME - 1);
    /* strncpy doesnt append the trailing NULL, of the string is too long. */
    name[MAXFILENAME] = '\0';

    if (name[len - 1] == '/') name[len - 1] = '\0';
    /* not all systems allow stat'ing a file with / appended */
    if (stat(name, &s) == 0) {
      tm_t = s.st_mtime;
      ret = 1;
    }
  }
  filedate = localtime(&tm_t);

  tmzip->tm_sec = filedate->tm_sec;
  tmzip->tm_min = filedate->tm_min;
  tmzip->tm_hour = filedate->tm_hour;
  tmzip->tm_mday = filedate->tm_mday;
  tmzip->tm_mon = filedate->tm_mon;
  tmzip->tm_year = filedate->tm_year;

  return ret;
}
#endif

//////////////////////////////////////////////////////////
// class Zip::ZipImpl
//////////////////////////////////////////////////////////

class Zip::ZipImpl {
 public:
  ZipImpl(const std::string &filePath);
  virtual ~ZipImpl();

  void createNew();
  void open();
  void close();
  bool isOpened() const;
  const std::string &getFilePath() const;

  void setCurrentEntry(const ZipEntry &entry);
  void zipFileToCurrEntry(const std::string &file);
  void writeToCurrEntry(istream &in);
  void writeToCurrEntry(const void *data, uint32_t size);

  void zip(const std::string &src, const std::string &entry = "",
           const std::string &newFileName = "");

 private:
  std::string _filePath;
  void *_zipFile;
  mutable std::recursive_mutex mutex;

  void doZipRegFile(const std::string &src, const std::string &entry = "",
                    const std::string &newFileName = "");
};

Zip::ZipImpl::ZipImpl(const std::string &filePath)
    : _filePath(filePath), _zipFile(NULL) {}

Zip::ZipImpl::~ZipImpl() { this->close(); }

void Zip::ZipImpl::createNew() {
  this->close();
  this->_zipFile = zipOpen(this->_filePath.c_str(), APPEND_STATUS_CREATE);
  if (NULL == this->_zipFile) {
    THROW_EXCEPTION(Exception,
                    "zip file[" + this->_filePath + "] create failed.", 0);
  }
}

void Zip::ZipImpl::open() {
  if (this->isOpened()) return;
  this->_zipFile = zipOpen(this->_filePath.c_str(), APPEND_STATUS_ADDINZIP);
  if (NULL == this->_zipFile) {
    THROW_EXCEPTION(Exception, "zip file[" + this->_filePath + "] open failed.",
                    0);
  }
}

void Zip::ZipImpl::close() {
  lock_guard<recursive_mutex> lk(this->mutex);
  if (this->_zipFile) {
    zipClose(this->_zipFile, NULL);
    this->_zipFile = NULL;
  }
}

bool Zip::ZipImpl::isOpened() const {
  lock_guard<recursive_mutex> lk(this->mutex);
  return (NULL != this->_zipFile);
}

const std::string &Zip::ZipImpl::getFilePath() const { return _filePath; }

void Zip::ZipImpl::zipFileToCurrEntry(const std::string &file) {
  lock_guard<recursive_mutex> lk(this->mutex);
  fstream f(file, ios::in | ios::binary);
  this->writeToCurrEntry(f);
}

void Zip::ZipImpl::writeToCurrEntry(istream &in) {
  const int bufSize = 20480;
  char buf[bufSize];
  while (!in.eof()) {
    in.read(buf, bufSize);
    uint32_t readBytes = static_cast<uint32_t>(in.gcount());

    if (readBytes <= 0) {
      break;
    }

    if (0 != zipWriteInFileInZip(this->_zipFile, buf, readBytes)) {
      THROW_EXCEPTION(Exception, "istream zip failed.", 0);
    }
  }
}

void Zip::ZipImpl::writeToCurrEntry(const void *data, uint32_t size) {
  if (nullptr == data || 0 == size) return;

  lock_guard<recursive_mutex> lk(this->mutex);
  if (0 != zipWriteInFileInZip(this->_zipFile, data, size)) {
    THROW_EXCEPTION(Exception, "buffer zip failed.", 0);
  }
}

void Zip::ZipImpl::setCurrentEntry(const ZipEntry &entry) {
  lock_guard<recursive_mutex> lk(this->mutex);
  zipCloseFileInZip(this->_zipFile);

  zip_fileinfo zi;
  memset(&zi, 0, sizeof(zi));

  // GetFileTime(srcFile.getPath().c_str(), &zi.tmz_date, &zi.dosDate);

  if (0 != zipOpenNewFileInZip(this->_zipFile, entry.getEntry().c_str(), &zi,
                               NULL, 0, NULL, 0, NULL, Z_DEFLATED,
                               Z_DEFAULT_COMPRESSION)) {
    THROW_EXCEPTION(Exception, "open entry[" + entry.getEntry() + "] failed.",
                    0);
  }
}

void Zip::ZipImpl::zip(const std::string &src, const std::string &entry,
                       const std::string &newFileName) {
  lock_guard<recursive_mutex> lk(this->mutex);

  File srcFile(src);

  if (!srcFile.isExists()) {
    THROW_EXCEPTION(Exception, "file[" + src + "] not exists.", 0);
  }

  if (srcFile.isDirectory()) {
    string dirName = srcFile.getName();
    File srcEntry(entry, newFileName.empty() ? dirName : newFileName);

    auto it = srcFile.iterator();
    while (it.next()) {
      auto file = it.getFile();
      if (file.isDirectory()) {
        string newEntry = file.getName();
        this->zip(file, srcEntry);
        continue;
      }

      doZipRegFile(file, srcEntry);
    }

    return;
  }

  this->doZipRegFile(src, entry, newFileName);
}

void Zip::ZipImpl::doZipRegFile(const std::string &src,
                                const std::string &entry,
                                const std::string &newFileName) {
  File srcFile(src);
  string zipFileName = (newFileName.empty() ? srcFile.getName() : newFileName);
  File zipFile(entry, zipFileName);

  zip_fileinfo zi;
  memset(&zi, 0, sizeof(zi));

  GetFileTime(srcFile.getPath().c_str(), &zi.tmz_date, &zi.dosDate);

  zipCloseFileInZip(this->_zipFile);
  if (0 != zipOpenNewFileInZip(this->_zipFile, zipFile.getPath().c_str(), &zi,
                               NULL, 0, NULL, 0, NULL, Z_DEFLATED,
                               Z_DEFAULT_COMPRESSION)) {
    THROW_EXCEPTION(Exception, "file[" + src + "] zip failed.", 0);
  }

  fstream f;
  f.open(srcFile.getPath().c_str(), ios::in | ios::binary);
  this->writeToCurrEntry(f);

  if (0 != zipCloseFileInZip(this->_zipFile)) {
    THROW_EXCEPTION(Exception, "file[" + src + "] zip failed.", 0);
  }
}

//////////////////////////////////////////////////////////
// class Zip
//////////////////////////////////////////////////////////

Zip::Zip(const std::string &filePath) : impl(new Zip::ZipImpl(filePath)) {}

Zip::~Zip() { delete this->impl; }

void Zip::createNew() { this->impl->createNew(); }

void Zip::open() { this->impl->open(); }

void Zip::close() { this->impl->close(); }

bool Zip::isOpened() const { return this->impl->isOpened(); }

const std::string &Zip::getFilePath() const {
  return this->impl->getFilePath();
}

Zip &Zip::operator<<(const ZipEntry &entry) {
  this->impl->setCurrentEntry(entry);
  return *this;
}

Zip &Zip::operator<<(const std::string &file) {
  this->impl->zipFileToCurrEntry(file);
  return *this;
}

Zip &Zip::write(const void *data, uint32_t size) {
  this->impl->writeToCurrEntry(data, size);
  return *this;
}

Zip &Zip::zip(const std::string &src, const std::string &entry,
              const std::string &newFileName) {
  this->impl->zip(src, entry, newFileName);
  return *this;
}

NS_FF_END
