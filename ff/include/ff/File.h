/*
 * File.h
 *
 *  Created on: 2018-03-13
 *      Author: liyawu
 */

#ifndef FF_FILE_H_
#define FF_FILE_H_

#include <string>
#include <list>
#include <ostream>

#include <ff/Object.h>
#include <ff/Exception.h>
#include <ff/DateTime.h>
#include <ff/Noncopyable.h>

namespace NS_FF {

EXCEPTION_DEF(FileException);

class File;
class FFDLL FileIterator : public Noncopyable{
public:
	FileIterator(const std::string& path);
	FileIterator(FileIterator&& it);
	~FileIterator();

	bool next();
	File getFile();

private:
	class FileIteratorImpl;
	FileIteratorImpl* m_impl;
};

class FFDLL File {
public:
	File();
	File(const std::string& path);
	File(const std::string& parent, const std::string& child);
	File(std::list<std::string> path);
	virtual ~File();

	void setPath(const std::string& path);
	std::string getPath() const;
	std::string getName() const;
	File getParent() const;

	bool isDirectory() const;
	bool isRegularFile() const;
	bool isExists() const;
	bool isReadable() const;
	bool isWritable() const;
	bool isExecutable() const;
	void mkdir() const;
	void mkdirs() const;
	void remove(bool recursive = false) const;
	long long getSize() const;
	void rename(const std::string& path) const;
	void copyTo(const std::string& path, bool forceReplace = false) const;
	void moveTo(const std::string& path, bool forceReplace = false) const;

	FileIterator iterator() const;
	std::list<File> list() const;
	File cut(int count = 1) const;
	DateTime getModifyTime() const;
	DateTime getCreateTime() const;

	bool operator==(const std::string& path) const;
	bool operator==(const File& f) const;

	operator std::string() const;
	std::string getSeparater() const;

	friend std::ostream& operator<<(std::ostream& o, const File& file);

private:
	std::list<std::string> path;
};

} /* namespace NS_FF */

#endif /* SYSTEM_FILE_H_ */
