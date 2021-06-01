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
#include <initializer_list>

#include <ff/Object.h>
#include <ff/Exception.h>
#include <ff/DateTime.h>
#include <ff/Noncopyable.h>

NS_FF_BEG

EXCEPTION_DEF(FileException);

class File;
class LIBFF_API FileIterator : public Noncopyable{
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

class LIBFF_API File {
public:
	File();
	File(const std::string& path);
	File(const std::string& parent, const std::string& child);
	File(const std::list<std::string>& path);
	File(std::initializer_list<std::string> path);
	File(File&& file);
	File(const File& file);
	virtual ~File();

	void setPath(const std::string& path);
	std::string getPath() const;
	std::string getName() const;
	std::string getSuffix() const;
	File getParent() const;

	bool isDirectory() const;
	bool isRegularFile() const;
	bool isExists() const;
	bool isReadable() const;
	bool isWritable() const;
	bool isExecutable() const;
	bool mkdir() const;
	bool mkdirs() const;
	bool empty() const;
	bool remove(bool recursive = false) const;
	long long getSize() const;
	bool rename(const std::string& path) const;
	bool copyTo(const std::string& path, bool forceReplace = false) const;
	bool moveTo(const std::string& path, bool forceReplace = false) const;

	bool isEmpty() const;

	FileIterator iterator() const;

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

NS_FF_END

#endif /* SYSTEM_FILE_H_ */
