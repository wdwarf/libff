/*
 * AdoFields.h
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#ifndef WINDOWS_ADOFIELDS_H_
#define WINDOWS_ADOFIELDS_H_

#include <ff/ff_config.h>
#include <ff/windows/AdoObject.h>
#include <ff/windows/AdoField.h>
#include <vector>

NS_FF_BEG

class LIBFF_API AdoFields: public AdoObject {
public:
	AdoFields(void);
	AdoFields(const AdoFields &fields);
	AdoFields(FieldsPtr &fields);
	virtual ~AdoFields(void);
	void operator =(const AdoFields &fields);
	void operator =(const FieldsPtr &fields);
	FieldsPtr& operator->();
	operator FieldsPtr();
	long getFieldCount();
	std::vector<StringT> getFieldNames();

	AdoField operator[](const StringT &fieldName);
	AdoField operator[](long fieldIndex);

private:
	FieldsPtr fields;
};

NS_FF_END

#endif /* WINDOWS_ADOFIELDS_H_ */
