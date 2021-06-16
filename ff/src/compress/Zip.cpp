/*
 * Zip::ZipImpl.cpp
 *
 *  Created on: Nov 23, 2018
 *      Author: ducky
 */

#include <ff/Zip.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <zlib.h>
#include "minizip/zip.h"
#include <ff/File.h>
#include <ff/Exception.h>

using namespace std;

NS_FF_BEG

//////////////////////////////////////////////////////////
// class Zip::ZipImpl
//////////////////////////////////////////////////////////

class Zip::ZipImpl {
public:
	ZipImpl(const std::string& filePath);
	virtual ~ZipImpl();

	void createNew();
	void open();
	void close();
	bool isOpened() const;
	const std::string& getFilePath() const;

	void setCurrentEntry(const ZipEntry& entry);
	void zipFileToCurrEntry(const std::string& file);

	void zip(const std::string& src, const std::string& entry = "",
			const std::string& newFileName = "");

private:
	std::string _filePath;
	std::string _currentEntry;
	void* _zipFile;
	mutable std::recursive_mutex mutex;

	void doZipRegFile(const std::string& src, const std::string& entry = "",
			const std::string& newFileName = "");
};

Zip::ZipImpl::ZipImpl(const std::string& filePath) :
		_filePath(filePath), _zipFile(NULL) {
}

Zip::ZipImpl::~ZipImpl() {
	this->close();
}

void Zip::ZipImpl::createNew() {
	this->close();
	this->_zipFile = zipOpen(this->_filePath.c_str(), APPEND_STATUS_CREATE);
	if (NULL == this->_zipFile) {
		THROW_EXCEPTION(Exception,
				"zip file[" + this->_filePath + "] create failed.", 0);
	}
}

void Zip::ZipImpl::open() {
	if (this->isOpened())
		return;
	this->_zipFile = zipOpen(this->_filePath.c_str(), APPEND_STATUS_ADDINZIP);
	if (NULL == this->_zipFile) {
		THROW_EXCEPTION(Exception,
				"zip file[" + this->_filePath + "] open failed.", 0);
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

const std::string& Zip::ZipImpl::getFilePath() const {
	return _filePath;
}

void Zip::ZipImpl::zipFileToCurrEntry(const std::string& file) {
	this->zip(file, this->_currentEntry);

	lock_guard<recursive_mutex> lk(this->mutex);
	this->_currentEntry = "";
}

void Zip::ZipImpl::setCurrentEntry(const ZipEntry& entry) {
	lock_guard<recursive_mutex> lk(this->mutex);
	this->_currentEntry = entry.getEntry();
}

void Zip::ZipImpl::zip(const std::string& src, const std::string& entry,
		const std::string& newFileName) {
	lock_guard<recursive_mutex> lk(this->mutex);

	File srcFile(src);

	if(!srcFile.isExists()){
		THROW_EXCEPTION(Exception, "file[" + src + "] not exists.", 0);
	}

	if (srcFile.isDirectory()) {
		auto it = srcFile.iterator();
		while(it.next()){
			auto file = it.getFile();
			if (file.isDirectory())
			{
				string newEntry = file.getName();
				this->zip(file, File(File(entry, newFileName), newEntry));
				continue;
			}

			doZipRegFile(file, File(entry, newFileName));
		}

		return;
	}

	this->doZipRegFile(src, entry, newFileName);
}

void Zip::ZipImpl::doZipRegFile(const std::string& src,
		const std::string& entry, const std::string& newFileName) {
	File srcFile(src);
	string zipFileName = (newFileName.empty() ? srcFile.getName() : newFileName);
	File zipFile(entry, zipFileName);

	if (0 != zipOpenNewFileInZip(this->_zipFile, zipFile.getPath().c_str(),
	NULL, NULL, 0,
	NULL, 0,
	NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION)) {
		THROW_EXCEPTION(Exception, "file[" + src + "] zip failed.", 0);
	}

	fstream f;
	f.open(srcFile.getPath().c_str(), ios::in | ios::binary);
	const int bufSize = 20480;
	char buf[bufSize];
	while (!f.eof()) {
		f.read(buf, bufSize);
		size_t readBytes = f.gcount();

		if (readBytes <= 0) {
			break;
		}

		if (0 != zipWriteInFileInZip(this->_zipFile, buf, readBytes)) {
			THROW_EXCEPTION(Exception, "file[" + src + "] zip failed.", 0);
		}
	}

	if (0 != zipCloseFileInZip(this->_zipFile)) {
		THROW_EXCEPTION(Exception, "file[" + src + "] zip failed.", 0);
	}
}

//////////////////////////////////////////////////////////
// class Zip
//////////////////////////////////////////////////////////

Zip::Zip(const std::string& filePath) :
		impl(new Zip::ZipImpl(filePath)) {

}

Zip::~Zip() {
	delete this->impl;
}

void Zip::createNew() {
	this->impl->createNew();
}

void Zip::open() {
	this->impl->open();
}

void Zip::close() {
	this->impl->close();
}

bool Zip::isOpened() const {
	return this->impl->isOpened();
}

const std::string& Zip::getFilePath() const {
	return this->impl->getFilePath();
}

Zip& Zip::operator <<(const ZipEntry& entry) {
	this->impl->setCurrentEntry(entry);
	return *this;
}

Zip& Zip::operator <<(const std::string& file) {
	this->impl->zipFileToCurrEntry(file);
	return *this;
}

Zip& Zip::zip(const std::string& src, const std::string& entry,
		const std::string& newFileName) {
	this->impl->zip(src, entry, newFileName);
	return *this;
}

NS_FF_END

