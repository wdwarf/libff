/*
 * AdoField.h
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#ifndef WINDOWS_ADOFIELD_H_
#define WINDOWS_ADOFIELD_H_

#include <ff/ff_config.h>
#include <ff/windows/AdoObject.h>

NS_FF_BEG

class LIBFF_API AdoField: public AdoObject {
public:
	AdoField(void);
	AdoField(const AdoField &field);
	AdoField(FieldPtr &field);
	virtual ~AdoField(void);

	FieldPtr& operator->();
	operator FieldPtr();
	void operator =(const AdoField &field);
	void operator =(const FieldPtr &field);
	StringT getName();

	operator variant_t();
	operator long();
	operator double();
	operator bool();
	operator StringT();
	operator time_t();

	variant_t getValue();
	StringT valAsString();
	long valAsInt();
	double valAsFloat();
	time_t valAsTime();

private:
	FieldPtr field;
};

NS_FF_END

#endif /* WINDOWS_ADOFIELD_H_ */
