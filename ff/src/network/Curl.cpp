#include <curl/curl.h>
#include <ff/Curl.h>

using namespace std;

NS_FF_BEG

size_t Curl::CurlWriteData(void* buf, size_t size, size_t nmemb, void* stream) {
  auto writeBytes = size * nmemb;
  ostream* out = (ostream*)stream;
  out->write((const char*)buf, writeBytes);
  return writeBytes;
}

Curl::Curl() : m_curl(curl_easy_init()), m_headers(nullptr) {
  this->m_pOutputStream = &this->m_outputStream;
  this->setOption(CURLOPT_WRITEFUNCTION, Curl::CurlWriteData);
  this->setOption(CURLOPT_WRITEDATA, this->m_pOutputStream);
  this->setConnectTimeout(10 * 1000);
  this->setReadTimeout(10 * 1000);
}

Curl::Curl(const std::string& url, const CurlHeaders& headers)
    : m_curl(curl_easy_init()), m_headers(nullptr) {
  this->m_pOutputStream = &this->m_outputStream;
  this->setOption(CURLOPT_WRITEFUNCTION, Curl::CurlWriteData);
  this->setOption(CURLOPT_WRITEDATA, this->m_pOutputStream);
  this->setUrl(url);
  this->setHeaders(headers);
  this->setConnectTimeout(10 * 1000);
  this->setReadTimeout(60 * 1000);
}

Curl::~Curl() {
  if (nullptr != this->m_headers) {
    curl_slist_free_all(this->m_headers);
    this->m_headers = nullptr;
  }

  if (nullptr != this->m_curl) {
    curl_easy_cleanup(this->m_curl);
    this->m_curl = nullptr;
  }
}

Curl::operator CURL*() { return this->m_curl; }

Curl& Curl::setConnectTimeout(int64_t msTimeout) {
  this->setOption(CURLOPT_CONNECTTIMEOUT_MS, msTimeout);
  return *this;
}

Curl& Curl::setReadTimeout(int64_t msTimeout) {
  this->setOption(CURLOPT_TIMEOUT_MS, msTimeout);
  return *this;
}

Curl& Curl::setOutputStream(std::ostream* outputStream) {
  this->m_pOutputStream =
      nullptr == outputStream ? &this->m_outputStream : outputStream;
  this->setOption(CURLOPT_WRITEDATA, this->m_pOutputStream);
  return *this;
}

Curl& Curl::reset() {
  curl_easy_reset(this->m_curl);
  this->m_outputStream.str("");
  this->m_outputStream.clear();
  this->setOption(CURLOPT_WRITEFUNCTION, Curl::CurlWriteData);
  this->m_pOutputStream = &this->m_outputStream;
  this->setOption(CURLOPT_WRITEDATA, this->m_pOutputStream);
  return *this;
}

Curl& Curl::setUrl(const std::string& url) {
  this->setOption(CURLOPT_URL, url.c_str());
  return *this;
}

CURLcode Curl::perform() {
  this->setOption(CURLOPT_HTTPHEADER, this->m_headers);

  return curl_easy_perform(this->m_curl);
}

Curl& Curl::setHeaders(const CurlHeaders& headers) {
  for (auto& p : headers) {
    string header = p.first + ": " + p.second;
    this->m_headers = curl_slist_append(this->m_headers, header.c_str());
  }
  return *this;
}

Curl& Curl::setContentType(const std::string& contentType) {
  this->setHeaders({{"Content-Type", contentType}});
  return *this;
}

Curl& Curl::setBody(const std::string& body) {
  this->m_postFields = body;
  this->setOption(CURLOPT_POSTFIELDS, this->m_postFields.c_str());
  return *this;
}

Curl& Curl::setMethod(HttpMethod method) {
  switch (method) {
    case HttpMethod::Get:
      this->setOption(CURLOPT_CUSTOMREQUEST, "GET");
      break;
    case HttpMethod::Post:
      this->setOption(CURLOPT_CUSTOMREQUEST, "POST");
      break;
    case HttpMethod::Put:
      this->setOption(CURLOPT_CUSTOMREQUEST, "PUT");
      break;
    case HttpMethod::Delete:
      this->setOption(CURLOPT_CUSTOMREQUEST, "DELETE");
      break;
    case HttpMethod::Head:
      this->setOption(CURLOPT_CUSTOMREQUEST, "HEAD");
      break;
    case HttpMethod::Options:
      this->setOption(CURLOPT_CUSTOMREQUEST, "OPTIONS");
      break;
    case HttpMethod::Patch:
      this->setOption(CURLOPT_CUSTOMREQUEST, "PATCH");
      break;
    case HttpMethod::Trace:
      this->setOption(CURLOPT_CUSTOMREQUEST, "TRACE");
      break;
    case HttpMethod::Connect:
      this->setOption(CURLOPT_CUSTOMREQUEST, "CONNECT");
      break;
  }

  return *this;
}

std::stringstream& Curl::getOutputStream() { return this->m_outputStream; }

Curl& Curl::setOAuth2Token(const std::string& token) {
  this->setOption(CURLOPT_HTTPAUTH, CURLAUTH_BEARER);
  this->setOption(CURLOPT_XOAUTH2_BEARER, token.c_str());
  return *this;
}

int Curl::getResponseCode() {
  int code = -1;
  this->getInfo(CURLINFO_RESPONSE_CODE, &code);
  return code;
}

// ==================================================
// ==================================================

HttpPost::HttpPost() : m_httppost(nullptr), m_httppostLast(nullptr) {
  this->m_headers = curl_slist_append(m_headers, "Expect:");
}

HttpPost::HttpPost(const std::string& url, const CurlParams& formDatas,
                   const CurlHeaders& headers)
    : Curl(url, headers), m_httppost(nullptr), m_httppostLast(nullptr) {
  this->m_headers = curl_slist_append(m_headers, "Expect:");
  this->setFormDatas(formDatas);
}

HttpPost::~HttpPost() {
  if (nullptr != this->m_httppost) curl_formfree(this->m_httppost);
}

HttpPost& HttpPost::setFormDatas(const CurlParams& datas) {
  for (auto& p : datas) {
    curl_formadd(&m_httppost, &m_httppostLast, CURLFORM_COPYNAME,
                 p.first.c_str(), CURLFORM_COPYCONTENTS, p.second.c_str(),
                 CURLFORM_END);
  }
  return *this;
}

HttpPost& HttpPost::setFormFile(const std::string& fieldName,
                                const std::string& filePath) {
  curl_formadd(&m_httppost, &m_httppostLast, CURLFORM_COPYNAME,
               fieldName.c_str(), CURLFORM_FILE, filePath.c_str(),
               CURLFORM_END);
  return *this;
}

CURLcode HttpPost::perform() {
  curl_easy_setopt(this->m_curl, CURLOPT_HTTPPOST, this->m_httppost);
  return Curl::perform();
}

NS_FF_END
