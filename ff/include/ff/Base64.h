/*
 Auth: LiYawu

 Date: 2013-06-29 

 Description: base64
 */
#ifndef FF_Base64_H_
#define FF_Base64_H_

#include <ff/Object.h>
#include <string>
#include <ostream>
using std::string;
using std::ostream;

namespace NS_FF {

class LIBFF_API Base64 {
public:
	static string Encrypt(string str);
	static string Encrypt(const void* buf, unsigned int len);
	static bool Decrypt(void* des, unsigned int* desLen, string src);
	static bool Decrypt(ostream& des, string src);
	static bool Decrypt(char* des, unsigned int* desLen, const char* src, unsigned int srcLen);

private:
	Base64(void);
	virtual ~Base64(void);
};

}

#endif
