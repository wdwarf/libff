/*
 * FileAppender.cpp
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#include <ff/FileAppender.h>
#include <ff/LogInfo.h>
#include <ff/DateTime.h>
#include <iostream>
#include <list>

using namespace std;

namespace NS_FF {

	FileAppender::FileAppender(const std::string& logDir,
		const std::string& fileNamePrefix, int32_t reservedCount)
		: m_logDir(logDir), m_prefix(fileNamePrefix), m_reservedCount(reservedCount) {
		this->m_logDir.mkdirs();
	}

	FileAppender::~FileAppender() {
	}

	void FileAppender::log(const LogInfo& logInfo) {
		this->checkLogFile();

		this->m_logFile << logInfo.toLogString() << endl;
	}


	string FileAppender::genLogFileName() {
		return this->m_prefix + DateTime::Now().toLocalString("%Y-%m-%d")
			+ ".log";
	}

	void FileAppender::checkLogFile() {
		string fileName = this->genLogFileName();
		if (fileName != this->m_logFileName) {
			this->m_logFileName = fileName;
			this->m_logFile.close();
			this->m_logFile.open(File(this->m_logDir, m_logFileName), ios::out | ios::app);
			this->clearOldFiles();
		}
	}

	void FileAppender::clearOldFiles() {
		if (this->m_reservedCount < 0)
			return;

		if (!this->m_logDir.isExists())
			return;

		list<string> files;
		auto it = this->m_logDir.iterator();
		while (it.next()) {
			auto file = it.getFile();
			if (file.isDirectory()) continue;
			string name = file.getName();
			if (name == this->m_logFileName) continue;
			files.push_back(name);
		}

		files.sort();
		while (files.size() > this->m_reservedCount) {
			auto file = files.front();
			files.pop_front();
			File(this->m_logDir, file).remove();
		}
	}


} /* namespace NS_FF */
