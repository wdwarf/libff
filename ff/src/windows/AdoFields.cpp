/*
 * AdoFields.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: u16
 */

#include <ff/windows/AdoFields.h>

using namespace std;

namespace NS_FF {

AdoFields::AdoFields(void) {
	this->fields.CreateInstance(__uuidof(Fields));
}

AdoFields::~AdoFields(void) {
	if (this->fields)
		this->fields.Release();
}

AdoFields::AdoFields(const AdoFields &fields) {
	this->fields = fields.fields;
}

AdoFields::AdoFields(FieldsPtr &fields) {
	this->fields = fields;
}

void AdoFields::operator =(const AdoFields &fields) {
	this->fields = fields.fields;
}

void AdoFields::operator =(const FieldsPtr &fields) {
	this->fields = fields;
}

AdoFields::operator FieldsPtr() {
	return this->fields;
}

AdoField AdoFields::operator[](const StringT &fieldName) {
	return this->fields->GetItem(fieldName.c_str());
}

AdoField AdoFields::operator[](long fieldIndex) {
	return this->fields->GetItem(fieldIndex);
}

FieldsPtr& AdoFields::operator->() {
	return this->fields;
}

long AdoFields::getFieldCount() {
	return this->fields->GetCount();
}

vector<StringT> AdoFields::getFieldNames() {
	vector<StringT> re;
	long fieldCount = this->fields->GetCount();
	for (long i = 0; i < fieldCount; ++i) {
		bstr_t name = this->fields->GetItem(i)->GetName();
		if (name.length() > 0) {
			re.push_back((const TCHAR*) name);
		}
	}
	return re;
}

} /* namespace NS_FF */
