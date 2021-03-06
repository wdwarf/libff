/*
 * MD5.h
 *
 *  Created on: 05/04/2012
 *      Author: wdwarf
 */

#ifndef FF_MD5_H_
#define FF_MD5_H_

#include <ff/Object.h>
#include <string>
#include <sstream>
using std::string;
using std::stringstream;

NS_FF_BEG

struct MD5Result {
	unsigned char result[16];

	MD5Result();
	std::string toString() const;
	operator std::string() const;
	bool operator==(const MD5Result& md5Result) const;
	bool operator==(const std::string& md5Str) const;

	friend bool operator==(const std::string& md5Str,
			const MD5Result& md5Result);
};

class LIBFF_API MD5 {
public:
	static MD5Result Generate(const void* encrypt, unsigned int length);
	static MD5Result Generate(string input);
	static MD5Result FileMD5CheckSum(const std::string& filePath);

private:
	MD5();
	~MD5();

private:
	typedef struct {
		unsigned int count[2];
		unsigned int state[4];
		unsigned char buffer[64];
	} MD5_CTX;

	static void MD5Init(MD5_CTX *context);
	static void MD5Update(MD5_CTX *context, const unsigned char *input,
			unsigned int inputlen);
	static void MD5Final(MD5_CTX *context, unsigned char digest[16]);
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
