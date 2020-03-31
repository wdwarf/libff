/*
 * AdoField.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: u16
 */

#include <ff/windows/AdoField.h>
#include <ctime>

namespace NS_FF {

AdoField::AdoField(void) {
	this->field.CreateInstance(__uuidof(Field));
}

AdoField::AdoField(const AdoField &field) {
	this->operator =(field.field);
}

AdoField::~AdoField(void) {
	if (this->field)
		this->field.Release();
}

AdoField::AdoField(FieldPtr &field) {
	this->field = field;
}

void AdoField::operator =(const AdoField &field) {
	this->field = field.field;
}

void AdoField::operator =(const FieldPtr &field) {
	this->field = field;
}

AdoField::operator FieldPtr() {
	return this->field;
}

variant_t AdoField::getValue() {
	return this->field->GetValue();
}

StringT AdoField::valAsString() {
	variant_t val = this->field->GetValue();
	if (VT_NULL != val.vt) {
		return (const TCHAR*) bstr_t(val);
	}
	return _T("");
}

long AdoField::valAsInt() {
	variant_t val = this->field->GetValue();
	if (VT_NULL != val.vt) {
		return val;
	}
	return 0;
}

double AdoField::valAsFloat() {
	variant_t val = this->field->GetValue();
	if (VT_NULL != val.vt) {
		return val;
	}
	return 0.f;
}

time_t AdoField::valAsTime() {
	variant_t val = this->field->GetValue();
	if (VT_NULL != val.vt && VT_DATE == val.vt) {
		double date = val;
		long tz = 0;
		_get_timezone(&tz);
		//或
		//tz = timezone;
		return (time_t) ((date - 25569) * 86400 + tz + 0.5);
	}
	return 0;
}

FieldPtr& AdoField::operator->() {
	return this->field;
}

StringT AdoField::getName() {
	return (const TCHAR*) this->field->GetName();
}

AdoField::operator long() {
	return this->valAsInt();
}

AdoField::operator double() {
	return this->valAsFloat();
}

AdoField::operator bool() {
	return bool(this->valAsInt());
}

AdoField::operator StringT() {
	return this->valAsString();
}

AdoField::operator variant_t() {
	return this->getValue();
}

AdoField::operator time_t() {
	return this->valAsTime();
}

} /* namespace NS_FF */
