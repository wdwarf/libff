/*
 * IAppender.h
 *
 *  Created on: Aug 8, 2019
 *      Author: root
 */

#ifndef FF_IAPPENDER_H_
#define FF_IAPPENDER_H_

#include <ff/LogInfo.h>
#include <ff/Object.h>
#include <ff/ff_config.h>

#include <list>
#include <memory>

NS_FF_BEG

class LIBFF_API IAppender {
 public:
  IAppender() = default;
  virtual ~IAppender() = default;

  virtual void log(const LogInfo& logInfo) = 0;

  virtual void log(const std::list<LogInfo>& logInfos) {
    for (auto& logInfo : logInfos) {
      this->log(logInfo);
    }
  }
};

typedef std::shared_ptr<IAppender> AppenderPtr;

NS_FF_END

#endif /* FF_IAPPENDER_H_ */
