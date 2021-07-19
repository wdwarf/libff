/*
 * File.cpp
 *
 *  Created on: 2018年3月13日
 *      Author: liyawu
 */

#include <errno.h>
#include <ff/File.h>
#include <ff/String.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <sys/stat.h>
#else
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

using namespace std;

NS_FF_BEG

#if defined(__MINGW32__) || defined(WIN32)
static const char* PATH_SEPARATER = "\\";
#else
static const char* PATH_SEPARATER = "/";
#endif

class FileIterator::FileIteratorImpl {
 public:
  FileIteratorImpl(const std::string& path) : m_path(path) {
#ifdef _WIN32
#else
    this->m_dir = opendir(path.c_str());
#endif
  }

  ~FileIteratorImpl() {
#ifdef _WIN32
    FindClose(this->m_dir);
#else
    if (nullptr != this->m_dir) closedir(this->m_dir);
#endif
  }

  bool valid() const {
#ifdef _WIN32
    return (INVALID_HANDLE_VALUE != this->m_dir);
#else
    return (nullptr != this->m_dirent);
#endif
  }

  bool next() {
#ifdef _WIN32
    if (!this->valid()) {
      this->m_dir = FindFirstFileA((m_path + PATH_SEPARATER + "*.*").c_str(),
                                   &m_findData);
      if (this->valid()) {
        do {
          string name = this->m_findData.cFileName;
          if ("." == name || ".." == name) continue;
          return true;
        } while (FindNextFileA(this->m_dir, &this->m_findData));
        return false;
      }
      return false;
    }

    while (FindNextFileA(this->m_dir, &this->m_findData)) {
      string name = this->m_findData.cFileName;
      if ("." == name || ".." == name) continue;
      return true;
    };
#else
    while (nullptr != (this->m_dirent = readdir(this->m_dir))) {
      string name = this->m_dirent->d_name;
      if ("." == name || ".." == name) continue;
      return true;
    }
#endif

    return false;
  }

  File getFile() {
    if (!this->valid()) return File();
#ifdef _WIN32
    return File(this->m_path, this->m_findData.cFileName);
#else
    return File(this->m_path, this->m_dirent->d_name);
#endif
  }

 private:
  string m_path;
#ifdef _WIN32
  HANDLE m_dir = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATAA m_findData;
#else
  DIR* m_dir;
  struct dirent* m_dirent = NULL;
#endif
};

FileIterator::FileIterator(const std::string& path)
    : m_impl(new FileIterator::FileIteratorImpl(path)) {}

FileIterator::FileIterator(FileIterator&& it) {
  this->m_impl = it.m_impl;
  it.m_impl = nullptr;
}

FileIterator::~FileIterator() { delete this->m_impl; }

bool FileIterator::next() { return this->m_impl->next(); }

File FileIterator::getFile() { return this->m_impl->getFile(); }

//==========================================================

File::File() {}

File::File(const std::string& path) { this->setPath(path); }

File::File(const std::string& parent, const std::string& child) {
  string p1 = File(parent).getPath();
  if (!p1.empty()) p1 += PATH_SEPARATER;
  string p2 = File(child).getPath();
  if (!p2.empty() && PATH_SEPARATER[0] == p2[0]) {
    p2 = p2.substr(1);
  }
  this->setPath(p1 + p2);
}

File::File(const std::list<std::string>& path) { this->path = path; }

File::File(std::initializer_list<std::string> path) {
  for (auto& child : path) {
    this->setPath(File(*this, child));
  }
}

File& File::operator=(const File& f) {
  this->path = f.path;
  return *this;
}

File::File(const File& file) { this->path = file.path; }

File::File(File&& file) { this->path = std::move(file.path); }

File::~File() {
  //
}

void File::setPath(const std::string& path) {
  string p = TrimCopy(path);
  if (p.empty()) return;

  this->path.clear();

  stringstream nodeStr;
  for (string::iterator it = p.begin(); it != p.end(); ++it) {
    char c = *it;
    if ('/' == c || '\\' == c) {
      string node = nodeStr.str();
      nodeStr.clear();
      nodeStr.str("");
      Trim(node);
      if (!node.empty()) {
        this->path.push_back(node);
      } else if (this->path.empty()) {
        string rootNode;
        rootNode.push_back(c);
        this->path.push_back(rootNode);
      }
    } else {
      nodeStr << c;
    }
  }

  string node = nodeStr.str();
  nodeStr.clear();
  nodeStr.str("");
  Trim(node);
  if (!node.empty()) {
    this->path.push_back(node);
  } else if (this->path.empty()) {
    this->path.push_back(PATH_SEPARATER);
  }
}

string File::getPath() const {
  string path;
  for (std::list<std::string>::const_iterator it = this->path.begin();
       it != this->path.end(); ++it) {
    if (path.empty()) {
      path = *it;
    } else {
      if ("/" == path || "\\" == path) {
        path += *it;
      } else {
        path += PATH_SEPARATER + *it;
      }
    }
  }
  return path;
}

string File::getName() const {
  if (!this->path.empty()) {
    string name = this->path.back();
    if (!name.empty() && "/" != name && "\\" != name &&
        ':' != name[name.length() - 1]) {
      return name;
    }
  }
  return "";
}

File File::getParent() const { return this->cut(); }

bool File::isDirectory() const {
  string path = this->getPath();
  if (path.empty()) return false;

  if (':' == path[path.length() - 1]) {
    path += PATH_SEPARATER;
  }

#ifdef _WIN32
  DWORD attr = ::GetFileAttributesA(path.c_str());
  return (INVALID_FILE_ATTRIBUTES != attr && (attr & FILE_ATTRIBUTE_DIRECTORY));
#else
  struct stat buf;
  int re = stat(path.c_str(), &buf);
  if (0 != re) return false;

  return S_ISDIR(buf.st_mode);
#endif
}

bool File::isRegularFile() const {
  string path = this->getPath();
  if (path.empty()) return false;

  if (':' == path[path.length() - 1]) {
    path += PATH_SEPARATER;
  }

#ifdef _WIN32
  DWORD attr = ::GetFileAttributesA(path.c_str());
  return (INVALID_FILE_ATTRIBUTES != attr && (attr & FILE_ATTRIBUTE_NORMAL));
#else
  struct stat buf;
  int re = stat(path.c_str(), &buf);
  if (0 != re) return false;

  return S_ISREG(buf.st_mode);
#endif
}

bool File::isExists() const {
  string path = this->getPath();
  if (path.empty()) return false;

  if (':' == path[path.length() - 1]) {
    path += PATH_SEPARATER;
  }

#ifdef _WIN32
  return (INVALID_FILE_ATTRIBUTES != ::GetFileAttributesA(path.c_str()));
#else
  return (0 == access(path.c_str(), F_OK));
#endif
}

bool File::isReadable() const {
  string path = this->getPath();
  if (path.empty()) return false;

  if (':' == path[path.length() - 1]) {
    path += PATH_SEPARATER;
  }

#ifdef _WIN32
  DWORD attr = ::GetFileAttributesA(path.c_str());
  return (INVALID_FILE_ATTRIBUTES != attr);
#else
  return (0 == access(path.c_str(), R_OK));
#endif
}

bool File::isWritable() const {
  string path = this->getPath();
  if (path.empty()) return false;

  if (':' == path[path.length() - 1]) {
    path += PATH_SEPARATER;
  }

#ifdef _WIN32
  DWORD attr = ::GetFileAttributesA(path.c_str());
  return (INVALID_FILE_ATTRIBUTES != attr &&
          (FILE_ATTRIBUTE_READONLY != (attr & FILE_ATTRIBUTE_READONLY)));
#else
  return (0 == access(path.c_str(), W_OK));
#endif
}

bool File::isExecutable() const {
  string path = this->getPath();
  if (path.empty()) return false;

  if (':' == path[path.length() - 1]) {
    path += PATH_SEPARATER;
  }

#ifdef _WIN32
  struct stat buf;
  if (0 != stat(path.c_str(), &buf)) return false;

  return (_S_IEXEC == (_S_IEXEC & buf.st_mode));
#else
  return (0 == access(path.c_str(), X_OK));
#endif
}

long long File::getSize() const {
  string path = this->getPath();
  if (path.empty()) return false;

  struct stat buf;
  if (0 != stat(path.c_str(), &buf)) return 0;

#ifdef _WIN32
  if (_S_IFDIR & buf.st_mode) return 0;
#else
  if (S_ISDIR(buf.st_mode)) {
    return 0;
  }
#endif

  return buf.st_size;
}

bool File::mkdir() const {
  if (this->isExists() && !this->isDirectory()) {
    return false;
  }

  if (this->isExists()) return false;

  string path = this->getPath();

#if defined(__linux__) || defined(__CYGWIN32__)
  return (0 == ::mkdir(path.c_str(), S_IRWXU));
#elif defined(__MINGW32__)
  return (0 == ::mkdir(path.c_str()));
#elif defined(_WIN32)
  return (TRUE == ::CreateDirectoryA(path.c_str(), NULL));
#endif
}

bool File::empty() const {
  if (!this->isExists() || !this->isDirectory()) return true;

  auto it = this->iterator();
  while (it.next()) {
    if (!it.getFile().remove(true)) return false;
  }

  return true;
}

bool File::mkdirs() const {
  if (this->isExists() && !this->isDirectory()) {
    return false;
  }

  if (this->isExists()) return false;

  string path;
  for (std::list<std::string>::const_iterator it = this->path.begin();
       it != this->path.end(); ++it) {
    if (path.empty()) {
      path = *it;
    } else {
      if ("/" == path || "\\" == path) {
        path += *it;
      } else {
        path += PATH_SEPARATER + *it;
      }
    }

    File f(path);
    if (!f.isExists()) {
      f.mkdir();
    }
  }

  return true;
}

bool File::remove(bool recursive) const {
  if (!this->isExists()) return false;

  if (this->isDirectory()) {
    auto it = this->iterator();
    while (it.next()) {
      auto file = it.getFile();
      if (!recursive && file.isDirectory()) continue;
      if (!file.remove(true)) return false;
    }
#ifdef _WIN32
    return (TRUE == RemoveDirectory(this->getPath().c_str()));
#endif
  }

  return (0 == ::remove(this->getPath().c_str()));
}

FileIterator File::iterator() const { return FileIterator(this->getPath()); }

File File::cut(int count) const {
  std::list<std::string> p = this->path;
  for (; count > 0 && !p.empty(); --count) {
    p.pop_back();
  }

  return File(p);
}

bool File::rename(const std::string& path) const {
  string currentPath = this->getPath();
  return (0 == ::rename(currentPath.c_str(), path.c_str()));
}

bool File::copyTo(const std::string& path, bool forceReplace) const {
  if (!this->isExists()) {
    return false;
  }

  if (*this == path) return false;

  File newPath(path);
  if (!forceReplace && newPath.isExists()) {
    return false;
  }

  if (this->isDirectory()) {
    if (newPath.isExists() && !newPath.isDirectory()) {
      return false;
    }
    newPath.mkdirs();

    auto it = this->iterator();
    while (it.next()) {
      auto file = it.getFile();
      file.copyTo(File(path, file.getName()), forceReplace);
    }
    return true;
  }

  ifstream in(this->getPath().c_str(), ios::in | ios::binary);
  ofstream out(path.c_str(), ios::out | ios::binary | ios::trunc);

  const int bufSize = 4096;
  char buf[bufSize];
  while (!in.eof()) {
    in.read(buf, bufSize);
    out.write(buf, in.gcount());
  }

  in.close();
  out.close();
  return true;
}

bool File::moveTo(const std::string& path, bool forceReplace) const {
  if (!this->isExists()) {
    return false;
  }

  if (*this == path) return false;

#ifdef _WIN32
  return (TRUE == MoveFileExA(this->getPath().c_str(), path.c_str(),
                              forceReplace ? MOVEFILE_REPLACE_EXISTING |
                                                 MOVEFILE_COPY_ALLOWED
                                           : 0));
#else
#endif
  return (this->copyTo(path, forceReplace) && this->remove(true));
}

bool File::isEmpty() const { return this->path.empty(); }

DateTime File::getModifyTime() const {
  if (!this->isExists()) {
    return DateTime(0);
  }
  string path = this->getPath();
  struct stat buf;
  int re = stat(path.c_str(), &buf);
  if (0 != re) {
    return DateTime(0);
  }

  return DateTime(buf.st_mtime);
}

DateTime File::getCreateTime() const {
  if (!this->isExists()) {
    return DateTime(0);
  }
  string path = this->getPath();
  struct stat buf;
  int re = stat(path.c_str(), &buf);
  if (0 != re) {
    return DateTime(0);
  }

  return DateTime(buf.st_ctime);
}

bool File::operator==(const std::string& path) const {
  return this->operator==(File(path));
}

bool File::operator==(const File& f) const { return this->path == f.path; }

File::operator std::string() const { return this->getPath(); }

std::string File::getSeparater() const { return PATH_SEPARATER; }

std::string File::getSuffix() const {
  auto name = this->getName();
  auto pos = name.find_last_of(".");
  if (string::npos == pos) return "";
  return name.substr(pos + 1);
}

Buffer File::readAll() const {
	Buffer out;
  if (!this->isExists() || this->isDirectory()) return out;

  fstream f(this->getPath(), ios::in);
  if (!f.is_open()) return out;
  char buf[2048];
  size_t readBytes = 0;
  while (!f.eof()) {
    f.read(buf, 2048);
    readBytes = f.gcount();
    if (readBytes > 0) out.append(buf, readBytes);
  }

  f.close();
  return out;
}

std::ostream& operator<<(std::ostream& o, const File& file) {
  o << file.getPath();
  return o;
}

NS_FF_END
