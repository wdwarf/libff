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

class Base64: virtual public Object {
public:
	static string encrypt(string str);
	static string encrypt(const char* buf, long len);
	static void decrypt(char* des, long* desLen, string src);
	static void decrypt(ostream& des, string src);
	static void decrypt(char* des, long* desLen, const char* src, long srcLen);

private:
	Base64(void);
	virtual ~Base64(void);
};

}

#endif
