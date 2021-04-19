/*
 * AdoRecordSet.h
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#ifndef WINDOWS_ADORECORDSET_H_
#define WINDOWS_ADORECORDSET_H_

#include <ff/ff_config.h>
#include <ff/windows/AdoObject.h>
#include <ff/windows/AdoFields.h>
#include <ff/windows/AdoConnection.h>

NS_FF_BEG

class LIBFF_API AdoRecordSet: public AdoObject {
public:
	AdoRecordSet(void);
	AdoRecordSet(_RecordsetPtr rs);
	virtual ~AdoRecordSet(void);

	operator _RecordsetPtr();
	_RecordsetPtr& operator->();
	void operator =(_RecordsetPtr rs);
	operator bool();

	void prev();
	void next();
	void first();
	void last();
	bool bof();
	bool eof();
	void del();
	long getRecordCount();

	void open();
	void close();

	void setSql(const StringT &sql);
	StringT getSql();
	void setCursorType(CursorTypeEnum cursorType);
	void setLockType(LockTypeEnum lockType);
	void setOptions(long options);

	AdoFields getFields();
	void setConnection(const AdoConnection &conn);
	AdoConnection getConnection();

private:
	bstr_t sql;
	long options;
	LockTypeEnum lockType;
	CursorTypeEnum cursorType;
	_RecordsetPtr rs;
};

NS_FF_END

#endif /* WINDOWS_ADORECORDSET_H_ */
