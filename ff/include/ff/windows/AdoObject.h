/*
 * AdoObject.h
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#ifndef WINDOWS_ADOOBJECT_H_
#define WINDOWS_ADOOBJECT_H_

#include <ff/ff_config.h>
#include <string>

#ifndef _LOCAL_ADO_DLL
#import "C:\Program Files\Common Files\system\ado\msadox.dll"
#import "C:\Program files\common files\system\ole db\oledb32.dll" rename_namespace("wOLEDB")
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","wADOEOF")
#else
#import "msadox.dll"
#import "oledb32.dll" rename_namespace("wOLEDB")
#import "msado15.dll" no_namespace rename("EOF","wADOEOF")
#endif

NS_FF_BEG

class LIBFF_API AdoObject {
public:
	AdoObject();
	virtual ~AdoObject();

	static StringT BstrToStr(const bstr_t &str);
};

NS_FF_END

#endif /* WINDOWS_ADOOBJECT_H_ */

