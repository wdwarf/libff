/*
 * AdoRecordSet.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: u16
 */

#include <ff/windows/AdoRecordSet.h>

using namespace std;

namespace NS_FF {

AdoRecordSet::AdoRecordSet(void) {
	this->rs.CreateInstance(__uuidof(Recordset));
	this->cursorType = adOpenStatic;
	this->lockType = adLockOptimistic;
	this->options = adCmdText;
}

AdoRecordSet::AdoRecordSet(_RecordsetPtr rs) {
	this->operator=(rs);
}

AdoRecordSet::~AdoRecordSet(void) {
	if (this->rs)
		this->rs.Release();
}

void AdoRecordSet::first() {
	this->rs->MoveFirst();
}

void AdoRecordSet::next() {
	this->rs->MoveNext();
}

void AdoRecordSet::last() {
	this->rs->MoveLast();
}

void AdoRecordSet::prev() {
	this->rs->MovePrevious();
}

AdoFields AdoRecordSet::getFields() {
	return this->rs->GetFields();
}

bool AdoRecordSet::bof() {
	return 0 != this->rs->GetBOF();
}

bool AdoRecordSet::eof() {
	return 0 != this->rs->GetwADOEOF();
}

void AdoRecordSet::del() {
	this->rs->Delete(adAffectCurrent);
	this->rs->UpdateBatch(adAffectCurrent);
}

long AdoRecordSet::getRecordCount() {
	return this->rs->GetRecordCount();
}

void AdoRecordSet::operator =(_RecordsetPtr rs) {
	this->rs = rs;
}

AdoRecordSet::operator _RecordsetPtr() {
	return this->rs;
}

AdoRecordSet::operator bool() {
	return !(this->eof() || this->bof());
}

void AdoRecordSet::open() {
	this->rs->Open(sql, this->rs->GetActiveConnection(), this->cursorType,
			this->lockType, this->options);
}

void AdoRecordSet::close() {
	this->rs->Close();
}

void AdoRecordSet::setConnection(const AdoConnection &conn) {
	this->rs->PutActiveConnection(
			conn.operator _ConnectionPtr().GetInterfacePtr());
}

AdoConnection AdoRecordSet::getConnection() {
	return (_ConnectionPtr) this->rs->GetActiveConnection();
}

void AdoRecordSet::setSql(const StringT &sql) {
	this->sql = sql.c_str();
}

StringT AdoRecordSet::getSql() {
	return (const TCHAR*) this->sql;
}

void AdoRecordSet::setCursorType(CursorTypeEnum cursorType) {
	this->cursorType = cursorType;
}

void AdoRecordSet::setLockType(LockTypeEnum lockType) {
	this->lockType = lockType;
}

void AdoRecordSet::setOptions(long options) {
	this->options = options;
}

_RecordsetPtr& AdoRecordSet::operator->() {
	return this->rs;
}

} /* namespace NS_FF */
