/*
 * AdoField.h
 *
 *  Created on: Mar 31, 2020
 *      Author: u16
 */

#ifndef WINDOWS_ADOFIELD_H_
#define WINDOWS_ADOFIELD_H_

#include <ff/ff_config.h>
#include <ff/windows/AdoObject.h>

namespace NS_FF {

class AdoField: public AdoObject {
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

} /* namespace NS_FF */

#endif /* WINDOWS_ADOFIELD_H_ */
