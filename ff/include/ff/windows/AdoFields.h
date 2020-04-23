/*
 * AdoFields.h
 *
 *  Created on: Mar 31, 2020
 *      Author: u16
 */

#ifndef WINDOWS_ADOFIELDS_H_
#define WINDOWS_ADOFIELDS_H_

#include <ff/ff_config.h>
#include <ff/windows/AdoObject.h>
#include <ff/windows/AdoField.h>
#include <vector>

namespace NS_FF {

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

} /* namespace NS_FF */

#endif /* WINDOWS_ADOFIELDS_H_ */
