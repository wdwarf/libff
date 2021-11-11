/*
 * MD5.h
 *
 *  Created on: 05/04/2012
 *      Author: wdwarf
 */

#ifndef FF_MD5_H_
#define FF_MD5_H_

#include <ff/Object.h>

#include <sstream>
#include <string>
using std::string;
using std::stringstream;

NS_FF_BEG

<<<<<<< HEAD
struct MD5Result {
  unsigned char result[16];
=======
struct LIBFF_API MD5Result {
	unsigned char result[16];
>>>>>>> fbb2cbdeaca42c8ff99a99810aaaee5afe716190

  MD5Result();
  std::string toString() const;
  operator std::string() const;
  bool operator==(const MD5Result &md5Result) const;
  bool operator==(const std::string &md5Str) const;

  friend bool operator==(const std::string &md5Str, const MD5Result &md5Result);
};

class LIBFF_API MD5 {
 public:
  MD5();
  MD5(const void *buf, uint32_t len);
  ~MD5();

  void update(const void *buf, uint32_t len);
  MD5Result result() const;
  operator MD5Result() const;

  static MD5Result Generate(const void *encrypt, unsigned int length);
  static MD5Result Generate(string input);
  static MD5Result FileMD5CheckSum(const std::string &filePath);

 private:
  struct MD5Context {
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];

    MD5Context();
  };

  MD5Context m_ctx;

  static void MD5Init(MD5Context *context);
  static void MD5Update(MD5Context *context, const unsigned char *input,
                        unsigned int inputlen);
  static void MD5Final(MD5Context *context, unsigned char digest[16]);
  static void MD5Transform(unsigned int state[4],
                           const unsigned char block[64]);
  static void MD5Encode(unsigned char *output, const unsigned int *input,
                        unsigned int len);
  static void MD5Decode(unsigned int *output, const unsigned char *input,
                        unsigned int len);
  static unsigned char PADDING[];
};
}

#endif /* FF_MD5_H_ */
