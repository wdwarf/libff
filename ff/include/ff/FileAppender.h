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

namespace NS_FF {

	class LIBFF_API FileAppender : public ff::IAppender {
	public:
		FileAppender(const std::string& logDir,
			const std::string& fileNamePrefix = "Log_", int32_t reservedCount = 7);
		virtual ~FileAppender();

		void log(const LogInfo& logInfo) override;

	private:
		std::ofstream m_logFile;	/** 当前的日志文件 */
		ff::File m_logDir;			/** 日志文件目录 */
		std::string m_prefix;		/** 日志文件前缀 */
		std::string m_logFileName;	/** 当前日志文件名称 */
		int32_t m_reservedCount;	/** 保留日志文件数量 */

		void checkLogFile();
		std::string genLogFileName();
		void clearOldFiles();
	};

} /* namespace NS_FF */

#endif /* FF_FILEAPPENDER_H_ */
