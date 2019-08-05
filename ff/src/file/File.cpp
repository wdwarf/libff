/*
 * File.cpp
 *
 *  Created on: 2018年3月13日
 *      Author: liyawu
 */

#include <ff/File.h>
#include <ff/String.h>

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

using namespace std;

namespace NS_FF {

#if defined(__MINGW32__) || defined(WIN32)
static const char* PATH_SEPARATER = "\\";
#else
static const char* PATH_SEPARATER = "/";
#endif

File::File() {
}

File::File(const std::string& path) {

	this->setPath(path);
}

File::File(const std::string& parent, const std::string& child) {
	string p1 = File(parent).getPath();
	if (!p1.empty())
		p1 += PATH_SEPARATER;
	string p2 = File(child).getPath();
	if (!p2.empty() && PATH_SEPARATER[0] == p2[0]) {
		p2 = p2.substr(1);
	}
	this->setPath(p1 + p2);
}

File::File(std::list<std::string> path) {
	this->path = path;
}

File::~File() {
	//
}

void File::setPath(const std::string& path) {
	string p = TrimCopy(path);
	if (p.empty())
		return;

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
		if (!name.empty() && "/" != name && "\\" != name
				&& ':' != name[name.length() - 1]) {
			return name;
		}
	}
	return "";
}

File File::getParent() const {
	return this->cut();
}

bool File::isDirectory() const {
	string path = this->getPath();
	if (path.empty())
		return false;

	if (':' == path[path.length() - 1]) {
		path += PATH_SEPARATER;
	}

	struct stat buf;
	int re = stat(path.c_str(), &buf);
	if (0 != re)
		return false;

	return S_ISDIR(buf.st_mode);
}

bool File::isRegularFile() const {
	string path = this->getPath();
	if (path.empty())
		return false;

	if (':' == path[path.length() - 1]) {
		path += PATH_SEPARATER;
	}

	struct stat buf;
	int re = stat(path.c_str(), &buf);
	if (0 != re)
		return false;

	return S_ISREG(buf.st_mode);
}

bool File::isExists() const {
	string path = this->getPath();
	if (path.empty())
		return false;

	if (':' == path[path.length() - 1]) {
		path += PATH_SEPARATER;
	}

	return (0 == access(path.c_str(), F_OK));
}

bool File::isReadable() const {
	string path = this->getPath();
	if (path.empty())
		return false;

	if (':' == path[path.length() - 1]) {
		path += PATH_SEPARATER;
	}

	return (0 == access(path.c_str(), R_OK));
}

bool File::isWritable() const {
	string path = this->getPath();
	if (path.empty())
		return false;

	if (':' == path[path.length() - 1]) {
		path += PATH_SEPARATER;
	}

	return (0 == access(path.c_str(), W_OK));
}

bool File::isExecutable() const {
	string path = this->getPath();
	if (path.empty())
		return false;

	if (':' == path[path.length() - 1]) {
		path += PATH_SEPARATER;
	}

	return (0 == access(path.c_str(), X_OK));
}

long long File::getSize() const {
	string path = this->getPath();
	if (path.empty())
		return false;

	struct stat buf;
	int re = stat(path.c_str(), &buf);

	if (0 != re) {
		THROW_EXCEPTION(FileException, "get file stat failed", errno);
	}

	if (S_ISDIR(buf.st_mode)) {
		THROW_EXCEPTION(FileException, "is a directory", 0);
	}

	return buf.st_size;
}

void File::mkdir() const {
	if (this->isExists() && !this->isDirectory()) {
		THROW_EXCEPTION(FileException, this->getPath() + " is not a directory",
				0);
	}

	if (this->isExists())
		return;

	string path = this->getPath();

#if defined(__linux__) || defined(__CYGWIN32__)
	int re = ::mkdir(path.c_str(), S_IRWXU);
#elif defined(__MINGW32__)
	int re = ::mkdir(path.c_str());
#endif

	if (0 != re) {
		THROW_EXCEPTION(FileException,
				"mkdir[" + path + "] failed, " + strerror(errno), errno);
	}
}

void File::mkdirs() const {
	if (this->isExists() && !this->isDirectory()) {
		THROW_EXCEPTION(FileException, this->getPath() + " is not a directory",
				0);
	}

	if (this->isExists())
		return;

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
}

void File::remove(bool recursive) const {
	if (!this->isExists())
		return;

	if (recursive && this->isDirectory()) {
		std::list<File> files = this->list();
		if (!files.empty()) {
			for (std::list<File>::iterator it = files.begin();
					it != files.end(); ++it) {
				it->remove(true);
			}
		}
	}

	string path = this->getPath();
	int re = ::remove(path.c_str());
	if (0 != re) {
		THROW_EXCEPTION(FileException,
				"remove[" + path + "] failed, " + strerror(errno), errno);
	}
}

std::list<File> File::list() const {
	std::list<File> files;
	if (this->isDirectory()) {
		string path = this->getPath();

		DIR* dir = opendir(path.c_str());
		if (NULL == dir)
			return files;

		struct dirent* _dirent = NULL;
		while (NULL != (_dirent = readdir(dir))) {
			string name = _dirent->d_name;
			if ("." == name || ".." == name)
				continue;
			files.push_back(File(path + PATH_SEPARATER + name));
		}

		closedir(dir);
	}
	return files;
}

File File::cut(int count) const {
	std::list<std::string> p = this->path;
	for (; count > 0 && !p.empty(); --count) {
		p.pop_back();
	}

	return File(p);
}

void File::rename(const std::string& path) const {
	string currentPath = this->getPath();
	int re = ::rename(currentPath.c_str(), path.c_str());
	if (0 != re) {
		THROW_EXCEPTION(FileException,
				"rename[" + currentPath + "] to [" + path + "] failed, " + strerror(errno),
				errno);
	}
}

void File::copyTo(const std::string& path, bool forceReplace) const {
	if (!this->isExists()) {
		THROW_EXCEPTION(FileException,
				"copy failed, [" + this->getPath() + "] not exists", 0);
	}

	if (*this == path)
		return;

	File newPath(path);
	if (!forceReplace && newPath.isExists()) {
		THROW_EXCEPTION(FileException, "copy failed, [" + path + "] has exists",
				0);
	}

	if (this->isDirectory()) {
		if (newPath.isExists() && !newPath.isDirectory()) {
			THROW_EXCEPTION(FileException,
					"copy failed, [" + path
							+ "] has exists and it's not a directory.", 0);
		}
		newPath.mkdirs();

		std::list<File> files = this->list();
		for (std::list<File>::iterator it = files.begin(); it != files.end();
				++it) {
			it->copyTo(File(path, it->getName()), forceReplace);
		}

		return;
	}

	int fdSrc = 0;
	int fdDest = 0;

	try {
		fdSrc = ::open(this->getPath().c_str(), O_RDONLY);
		if (fdSrc <= 0) {
			THROW_EXCEPTION(FileException,
					"file[" + this->getPath() + "] open failed.", 0);
		}

		fdDest = ::open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC,
		S_IRUSR | S_IWUSR);
		if (fdSrc <= 0) {
			THROW_EXCEPTION(FileException, "file[" + path + "] create failed.",
					0);
		}

		int readBytes = 0;
		const int bufSize = 1024;
		char buf[bufSize];
		do {
			readBytes = ::read(fdSrc, buf, bufSize);
			if (0 == readBytes)
				break;
			if ((readBytes < 0) && (EINTR != errno)) {
				THROW_EXCEPTION(FileException,
						"file[" + this->getPath() + "] read failed.", 0);
			}

			int writeBytes = ::write(fdDest, buf, readBytes);
			if ((writeBytes < 0) && (EINTR != errno)) {
				THROW_EXCEPTION(FileException,
						"file[" + path + "] write failed.", 0);
			}
		} while (readBytes > 0);
	} catch (Exception& e) {
		close(fdSrc);
		close(fdDest);
		throw;
	}

	close(fdSrc);
	close(fdDest);
}

void File::moveTo(const std::string& path, bool forceReplace) const {
	if (!this->isExists()) {
		THROW_EXCEPTION(FileException,
				"move failed, [" + this->getPath() + "] not exists", 0);
	}

	if (*this == path)
		return;

	this->copyTo(path, forceReplace);
	this->remove(true);
}

DateTime File::getModifyTime() const {
	if (!this->isExists()) {
		THROW_EXCEPTION(FileException,
				"file[" + this->getPath() + "] not exists", 0);
	}
	string path = this->getPath();
	struct stat buf;
	int re = stat(path.c_str(), &buf);
	if (0 != re) {
		THROW_EXCEPTION(FileException,
				"file[" + this->getPath() + "] stat failed, " + strerror(errno),
				errno);
	}

	return DateTime(buf.st_mtime);
}

DateTime File::getCreateTime() const {
	if (!this->isExists()) {
		THROW_EXCEPTION(FileException,
				"file[" + this->getPath() + "] not exists", 0);
	}
	string path = this->getPath();
	struct stat buf;
	int re = stat(path.c_str(), &buf);
	if (0 != re) {
		THROW_EXCEPTION(FileException,
				"file[" + this->getPath() + "] stat failed, " + strerror(errno),
				errno);
	}

	return DateTime(buf.st_ctime);
}

bool File::operator==(const std::string& path) const {
	return this->operator ==(File(path));
}

bool File::operator==(const File& f) const {
	return this->path == f.path;
}

File::operator std::string() const {
	return this->getPath();
}

std::string File::getSeparater() const {
	return PATH_SEPARATER;
}

std::ostream& operator<<(std::ostream& o, const File& file) {
	o << file.getPath();
	return o;
}

} /* namespace NS_FF */
