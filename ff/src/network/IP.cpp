/*
 * IP.cpp
 *
 *  Created on: 2018年5月2日
 *      Author: liyawu
 */

#include <ff/IP.h>
#include <ff/String.h>

#include <cstdlib>
#include <cstring>

#if defined(WIN32) || defined(__MINGW32__)

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#endif

#include <cstdio>

using namespace std;

NS_FF_BEG

IP::IP() : version(VersionType::UNKNOWN) {
  memset(this->addrBuffer, 0, sizeof(this->addrBuffer));
}

IP::IP(const std::string &ip) : version(VersionType::UNKNOWN) {
  memset(this->addrBuffer, 0, sizeof(this->addrBuffer));
  this->parse(ip);
}

IP::~IP() {
  //
}

void IP::parse(const std::string &ip) {
  if (parseV4(ip)) {
    return;
  } else if (parseV6(ip)) {
    return;
  } else {
    this->clear();
    string tmpIp = TrimCopy(ip);
    if (tmpIp.empty()) return;

    char ds[61] = {0};
    hostent *he = gethostbyname(tmpIp.c_str());
    if (he) {
      if (sizeof(in6_addr) == he->h_length) {
        in6_addr ipAddr;
        memcpy((void *)&ipAddr, (void *)he->h_addr, sizeof(in6_addr));

#ifdef WIN32
        DWORD len = 60;
        WSAPROTOCOL_INFOA protocolInfo;
        memset(&protocolInfo, 0, sizeof(WSAPROTOCOL_INFOA));
        protocolInfo.iAddressFamily = AF_INET6;
        if (WSAAddressToStringA((LPSOCKADDR)&ipAddr, sizeof(ipAddr),
                                &protocolInfo, ds, &len) &&
            this->parseV6(ds))
          return;
#else
        if (inet_ntop(AF_INET6, &ipAddr, ds, 60) && this->parseV6(ds)) return;
#endif

      } else if (sizeof(in_addr) == he->h_length) {
        in_addr ipAddr;
        memcpy((void *)&ipAddr, (void *)he->h_addr, sizeof(in_addr));
        strcpy(ds, inet_ntoa(ipAddr));

        if (this->parseV4(ds)) return;
      }
    }
  }
}

bool IP::parseV4(const std::string &ip) {
  this->clear();
  int token_count = 0;
  string tmpIp = TrimCopy(ip);

  // check len, an ip can never be bigger than 15
  // 123456789012345
  // XXX.XXX.XXX.XXX
  if (tmpIp.empty()) return false;

  if (tmpIp.length() > 15) return false;

  const char *ptr = tmpIp.c_str();
  int dot_count = 0;
  bool last_char_was_dot = true;

  while (*ptr) {
    if (*ptr == '.') {
      if (last_char_was_dot) return false;
      ++dot_count;
      last_char_was_dot = true;
    } else if (isdigit(*ptr)) {
      last_char_was_dot = false;
    } else
      return false;
    ++ptr;
  }
  if ((dot_count != 3) || (last_char_was_dot)) return false;

  ptr = tmpIp.c_str();
  while (*ptr) {
    unsigned long number = 0;

    if (*ptr == '.') ++ptr;

    int digits = 0;
    while ((*ptr) && (*ptr != '.')) {
      number = (number * 10) + *(ptr++) - '0';
      ++digits;
    }
    if (digits > 3) return false;
    if (number > 255) return false;

    addrBuffer[token_count++] = (unsigned char)number;
  }

  version = VersionType::V4;
  return true;
}

bool IP::parseV6(const std::string &ip) {
  this->clear();

  unsigned char tmp_address_buffer[256] = {0};
  string tmpIp = TrimCopy(ip);

  // check len, an ipv6 can never be bigger than 39 + 11
  // 123456789012345678901234567890123456789
  // 1BCD:2BCD:3BCD:4BCD:5BCD:6BCD:7BCD:8BCD%4123456789
  if (tmpIp.empty()) return false;

  unsigned int scope = 0;
  bool have_scope = false;

  {
    int scope_pos = 0;
    for (int i = tmpIp.length() - 1; i >= 0; i--) {
      if (tmpIp[i] == '%') {
        scope_pos = i;
        have_scope = true;
        break;
      }
      if (!isdigit(tmpIp[i])) break;
    }
    if (have_scope) {
      tmpIp[scope_pos] = 0;
      scope = atol(tmpIp.c_str() + scope_pos + 1);
    }
  }

  if (strlen(tmpIp.c_str()) > 39) return false;

  const char *in_ptr = tmpIp.c_str();
  char *out_ptr = (char *)tmp_address_buffer;
  char *end_first_part = NULL;
  char second[39] = {0};
  int second_used = false;
  int colon_count = 0;
  int had_double_colon = false;
  int last_was_colon = false;
  int had_dot = false;
  int dot_count = 0;
  int digit_count = 0;
  char digits[4] = {0};
  char last_deliminiter = 0;

  while (*in_ptr != 0) {
    if (*in_ptr == '.') {
      last_deliminiter = *in_ptr;
      had_dot = true;
      dot_count++;
      if (dot_count > 3) return false;
      if ((digit_count > 3) || (digit_count < 1)) return false;
      for (int i = 0; i < digit_count; i++)
        if (!isdigit(digits[i])) return false;
      digits[digit_count] = 0;
      int value = atoi(digits);
      if ((value > 0) && (value <= 255))
        *out_ptr++ = (unsigned char)value;
      else {
        if (strcmp(digits, "0") == 0)
          *out_ptr++ = (unsigned char)0;
        else
          return false;
      }
      digit_count = 0;
    } else if (*in_ptr == ':') {
      last_deliminiter = *in_ptr;

      if (had_dot) return false;  // don't allow : after a dot

      if (digit_count) {
        // move digits to right
        {
          for (int i = 0; i < digit_count; i++) {
            digits[digit_count - 1 - i] = HexAToI(digits[digit_count - 1 - i]);
            digits[3 - i] = digits[digit_count - 1 - i];
          }
        }
        {
          for (int i = 0; i < 4 - digit_count; i++) digits[i] = 0;
        }
        {
          // pack two digits into one byte
          for (int i = 0; i < 4; i += 2) {
            unsigned char c = digits[i];
            unsigned char d = digits[i + 1];
            *out_ptr++ = (c * 16 + d);
          }
        }
        digit_count = 0;
      }
      colon_count++;
      if (last_was_colon) {
        if (had_double_colon) return false;
        end_first_part = out_ptr;
        out_ptr = second;
        second_used = true;
        had_double_colon = true;
      } else {
        last_was_colon = true;
      }
    } else {
      if (digit_count >= 4) return false;
      if (!isxdigit(*in_ptr)) return false;
      digits[digit_count] = tolower(*in_ptr);

      digit_count++;
      if (digit_count > 4) return false;
      last_was_colon = 0;
    }
    in_ptr++;
  }

  // put last bytes from digits into buffer
  if (digit_count) {
    if (last_deliminiter == ':') {
      {
        // move digits to right
        for (int i = 0; i < digit_count; i++) {
          digits[digit_count - 1 - i] = HexAToI(digits[digit_count - 1 - i]);
          digits[3 - i] = digits[digit_count - 1 - i];
        }
      }
      {
        for (int i = 0; i < 4 - digit_count; i++) digits[i] = 0;
      }
      {
        // pack two digits into one byte
        for (int i = 0; i < 4; i += 2) {
          unsigned char c = digits[i];
          unsigned char d = digits[i + 1];
          *out_ptr++ = (c * 16 + d);
        }
      }
      digit_count = 0;
    } else if (last_deliminiter == '.') {
      if ((digit_count > 3) || (digit_count < 1)) return false;
      for (int i = 0; i < digit_count; i++)
        if (!isdigit(digits[i])) return false;
      digits[digit_count] = 0;
      int value = atoi(digits);
      if ((value > 0) && (value <= 255))
        *out_ptr++ = (unsigned char)value;
      else {
        if (strcmp(digits, "0") == 0)
          *out_ptr++ = (unsigned char)0;
        else
          return false;
      }
      // digit_count = 0;
    } else
      return false;
  }

  // must have between two and seven colons
  if ((colon_count > 7) || (colon_count < 2)) return false;

  // if there was a dot there must be three of them
  if ((dot_count > 0) && (dot_count != 3)) return false;

  if (second_used) {
    int len_first = int(end_first_part - (char *)tmp_address_buffer);
    int len_second = int(out_ptr - second);

    for (unsigned int i = 0;
         i < IPV6_LENGTH_NO_SCOPE - (len_first + len_second); i++)
      *end_first_part++ = 0;
    for (int i = 0; i < len_second; i++) *end_first_part++ = second[i];
  }

  if (!end_first_part) end_first_part = out_ptr;

  // check for short address
  if (end_first_part - (char *)tmp_address_buffer != IPV6_LENGTH_NO_SCOPE)
    return false;

  version = VersionType::V6;

  memcpy(addrBuffer, tmp_address_buffer, BUFSIZE);
  if (have_scope) {
    unsigned int *scope_p = (unsigned int *)(addrBuffer + IPV6_LENGTH_NO_SCOPE);
    *scope_p = htonl(scope);
  }

  return true;
}

void IP::clear() {
  this->version = VersionType::UNKNOWN;
  memset(this->addrBuffer, 0, sizeof(this->addrBuffer));
}

bool IP::isValid() const { return (VersionType::UNKNOWN != this->version); }

bool IP::hasV6Scope() const { return (0 != this->getV6Scope()); }

unsigned int IP::getV6Scope() const {
  unsigned int *pScope =
      (unsigned int *)(this->addrBuffer + IPV6_LENGTH_NO_SCOPE);
  return ntohl(*pScope);
}

std::string IP::toString() const {
  if (this->isValid()) {
    char buf[128] = {0};
    if (VersionType::V4 == version)
      sprintf((char *)buf, "%d.%d.%d.%d", addrBuffer[0], addrBuffer[1],
              addrBuffer[2], addrBuffer[3]);
    else if (this->hasV6Scope())
      sprintf((char *)buf,
              "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
              "%02x%02x:%02x%02x:%02x%02x:%02x%02x%%%u",
              addrBuffer[0], addrBuffer[1], addrBuffer[2], addrBuffer[3],
              addrBuffer[4], addrBuffer[5], addrBuffer[6], addrBuffer[7],
              addrBuffer[8], addrBuffer[9], addrBuffer[10], addrBuffer[11],
              addrBuffer[12], addrBuffer[13], addrBuffer[14], addrBuffer[15],
              this->getV6Scope());
    else
      sprintf((char *)buf,
              "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
              "%02x%02x:%02x%02x:%02x%02x:%02x%02x",
              addrBuffer[0], addrBuffer[1], addrBuffer[2], addrBuffer[3],
              addrBuffer[4], addrBuffer[5], addrBuffer[6], addrBuffer[7],
              addrBuffer[8], addrBuffer[9], addrBuffer[10], addrBuffer[11],
              addrBuffer[12], addrBuffer[13], addrBuffer[14], addrBuffer[15]);
    return buf;
  }

  return "";
}

IP::operator std::string() const { return this->toString(); }

IP::VersionType IP::getVersion() const { return version; }

bool IP::isV4() const { return (VersionType::V4 == this->version); }

bool IP::isV6() const { return (VersionType::V6 == this->version); }

Buffer IP::toBuffer() const {
  if (this->isValid()) {
    if (VersionType::V4 == this->version) {
      return Buffer((const char *)addrBuffer, 4);
    } else {
      if (this->hasV6Scope()) {
        return Buffer((const char *)addrBuffer, IPV6_LENGTH_WITH_SCOPE);
      } else {
        return Buffer((const char *)addrBuffer, IPV6_LENGTH_NO_SCOPE);
      }
    }
  }
  return Buffer();
}

IP &IP::operator=(const std::string &ip) {
  this->parse(ip);
  return *this;
}

LIBFF_API std::ostream &operator<<(std::ostream &o, const IP &ip) {
  o << ip.toString();
  return o;
}

NS_FF_END
