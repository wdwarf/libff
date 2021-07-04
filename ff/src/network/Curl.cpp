#include <ff/Curl.h>
using namespace std;

NS_FF_BEG

size_t Curl::CurlWriteData(void* buf, size_t size, size_t nmemb, void* stream) {
  auto writeBytes = size * nmemb;
  stringstream* out = (stringstream*)stream;
  out->write((const char*)buf, writeBytes);
  return writeBytes;
}

Curl::Curl() : m_curl(curl_easy_init()), m_header(nullptr) {
  this->m_header = curl_slist_append(m_header, "Expect:");
  this->setOption(CURLOPT_WRITEFUNCTION, Curl::CurlWriteData);
  this->setOption(CURLOPT_WRITEDATA, (void*)&m_outputStream);
}

Curl::~Curl() {
  if (nullptr != this->m_header) {
    curl_slist_free_all(this->m_header);
    this->m_header = nullptr;
  }

  if (nullptr != this->m_curl) {
    curl_easy_cleanup(this->m_curl);
    this->m_curl = nullptr;
  }
}

Curl& Curl::setUrl(const std::string& url) {
  this->setOption(CURLOPT_URL, url.c_str());
  return *this;
}

Curl& Curl::perform() {
  this->setOption(CURLOPT_HTTPHEADER, this->m_header);

  curl_easy_perform(this->m_curl);
  return *this;
}

Curl& Curl::setHeaders(const Headers& headers) {
  for (auto& p : headers) {
    string header = p.first + ":" + p.second;
    this->m_header = curl_slist_append(this->m_header, header.c_str());
  }
  return *this;
}

std::stringstream& Curl::getOutputStream() { return this->m_outputStream; }

NS_FF_END
