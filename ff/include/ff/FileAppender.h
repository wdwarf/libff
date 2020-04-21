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
		std::ofstream m_logFile;	/** ��ǰ����־�ļ� */
		ff::File m_logDir;			/** ��־�ļ�Ŀ¼ */
		std::string m_prefix;		/** ��־�ļ�ǰ׺ */
		std::string m_logFileName;	/** ��ǰ��־�ļ����� */
		int32_t m_reservedCount;	/** ������־�ļ����� */

		void checkLogFile();
		std::string genLogFileName();
		void clearOldFiles();
	};

} /* namespace NS_FF */

#endif /* FF_FILEAPPENDER_H_ */
