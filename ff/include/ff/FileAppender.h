/*
 * FileAppender.h
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#ifndef FF_FILEAPPENDER_H_
#define FF_FILEAPPENDER_H_

#include <ff/ff_config.h>
#include <ff/IAppender.h>
#include <ff/File.h>
#include <fstream>
#include <string>

NS_FF_BEG

	class LIBFF_API FileAppender : public NS_FF::IAppender {
	public:
		FileAppender(const std::string& logDir,
			const std::string& fileNamePrefix = "Log_", int32_t reservedCount = 7);
		virtual ~FileAppender();

		void log(const LogInfo& logInfo) override;

	private:
		std::ofstream m_logFile;
		NS_FF::File m_logDir;
		std::string m_prefix;
		std::string m_logFileName;
		int32_t m_reservedCount;

		void checkLogFile();
		std::string genLogFileName();
		void clearOldFiles();
	};

NS_FF_END

#endif /* FF_FILEAPPENDER_H_ */
