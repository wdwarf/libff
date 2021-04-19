/*
 * AdoCatalog.h
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#ifndef WINDOWS_ADOCATALOG_H_
#define WINDOWS_ADOCATALOG_H_

#include <string>
#include <vector>
#include <ff/ff_config.h>
#include <ff/windows/AdoObject.h>

NS_FF_BEG

////////#define DEFINE_WADO_OBJ
#define DEFINE_WADO_OBJ(className, adoInterface, adoClass) \
public:\
typedef ADOX::adoInterface##Ptr Ptr;\
typedef ADOX::adoClass AdoClass;\
className()\
{\
this->ptr.CreateInstance(__uuidof(AdoClass));\
}\
\
className(Ptr p)\
{\
this->ptr = p;\
}\
className(const className& className##Obj)\
{\
this->ptr = className##Obj.ptr;\
}\
\
~className()\
{\
if (this->ptr)\
{\
this->ptr.Release();\
}\
}\
\
operator Ptr()\
{\
return this->ptr;\
}\
\
operator bool()\
{\
return this->ptr;\
}\
\
Ptr operator->()\
{\
return this->ptr;\
}\
\
className& operator=(Ptr p)\
{\
this->ptr = p;\
return *this;\
}\
private:\
	Ptr ptr
////////end of #define DEFINE_WADO_OBJ

class LIBFF_API Key {
private:
	std::vector<StringT> columnNames;
	StringT name;
	int type;
};

class LIBFF_API AdoTable {
public:
	StringT getName();
	StringT getType();
	std::vector<StringT> getPrimaryKey();

DEFINE_WADO_OBJ(AdoTable, _Table, Table);
};

class LIBFF_API AdoTables {
public:
	AdoTable operator[](long i);
	long getCount();

DEFINE_WADO_OBJ(AdoTables, Tables, Tables);
};

class LIBFF_API AdoCatalog: public AdoObject {
public:
	void setConnection(_ConnectionPtr conn);
	AdoTables getTables();
	AdoTable getTable(const StringT &tableName);

DEFINE_WADO_OBJ(AdoCatalog, _Catalog, Catalog);
};

NS_FF_END

#endif /* WINDOWS_ADOCATALOG_H_ */
