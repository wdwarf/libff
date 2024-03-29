#ifndef _FF_NETWORK_CURL_H_
#define _FF_NETWORK_CURL_H_

#include <curl/curl.h>
#include <ff/ff_config.h>

#include <map>
#include <sstream>
#include <string>

NS_FF_BEG

using CurlHeaders = std::map<std::string, std::string>;
using CurlParams = std::map<std::string, std::string>;

enum class HttpMethod : uint8_t {
  Get,
  Post,
  Put,
  Delete,
  Head,
  Options,
  Patch,
  Trace,
  Connect
};

class LIBFF_API Curl {
 public:
  Curl();
  Curl(const std::string& url, const CurlHeaders& headers = {});
  virtual ~Curl();

  operator CURL*();

  Curl& setUrl(const std::string& url);
  Curl& setHeaders(const CurlHeaders& headers);
  Curl& setMethod(HttpMethod method);
  Curl& setContentType(const std::string& contentType);
  Curl& setBody(const std::string& body);

  Curl& setOAuth2Token(const std::string& token);
  Curl& setConnectTimeout(int64_t msTimeout);
  Curl& setReadTimeout(int64_t msTimeout);
  Curl& reset();
  std::stringstream& getOutputStream();
  Curl& setOutputStream(std::ostream* outputStream);
  virtual CURLcode perform();

  template <typename... Args>
  Curl& setOption(CURLoption opt, Args... args) {
    curl_easy_setopt(this->m_curl, opt, std::forward<Args...>(args...));
    return *this;
  }

  template <class... Arg>
  CURLcode getInfo(CURLINFO info, Arg... args) const {
    return curl_easy_getinfo(m_curl, info, std::forward<Arg>(args)...);
  }

  int getResponseCode();

 protected:
  CURL* m_curl;
  curl_slist* m_headers;
  std::stringstream m_outputStream;
  std::ostream* m_pOutputStream;
  std::string m_postFields;

  static size_t CurlWriteData(void* buf, size_t size, size_t nmemb,
                              void* stream);
};

class LIBFF_API HttpPost : public Curl {
 public:
  HttpPost();
  HttpPost(const std::string& url, const CurlParams& formDatas,
           const CurlHeaders& headers = {});
  virtual ~HttpPost();

  HttpPost& setFormDatas(const CurlParams& datas);
  HttpPost& setFormFile(const std::string& fieldName,
                        const std::string& filePath);
  CURLcode perform() override;

 protected:
  curl_httppost* m_httppost;
  curl_httppost* m_httppostLast;
};

NS_FF_END

#endif
