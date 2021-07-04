#ifndef _FF_NETWORK_CURL_H_
#define _FF_NETWORK_CURL_H_

#include <curl/curl.h>
#include <ff/ff_config.h>

#include <map>
#include <sstream>
#include <string>

NS_FF_BEG

using Headers = std::map<std::string, std::string>;

class Curl {
 public:
  Curl();
  virtual ~Curl();

  Curl& setUrl(const std::string& url);
  Curl& setHeaders(const Headers& headers);
  std::stringstream& getOutputStream();
  virtual Curl& perform();

  template <typename... Args>
  Curl& setOption(CURLoption opt, Args... args) {
    curl_easy_setopt(this->m_curl, opt, std::forward<Args...>(args...));
    return *this;
  }

 private:
  CURL* m_curl;
  curl_slist* m_header;
  std::stringstream m_outputStream;

  static size_t CurlWriteData(void* buf, size_t size, size_t nmemb,
                              void* stream);
};

NS_FF_END

#endif
