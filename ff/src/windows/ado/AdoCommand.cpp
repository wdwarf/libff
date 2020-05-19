/*
 * AdoCommand.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#include <ff/windows/AdoCommand.h>
#include <ff/String.h>
#include <regex>

using namespace std;

namespace NS_FF {

AdoParameter::AdoParameter(void) {
	this->param.CreateInstance(__uuidof(Parameter));
}

AdoParameter::AdoParameter(_ParameterPtr param) {
	this->param = param;
}

AdoParameter::~AdoParameter(void) {
	if (this->param) {
		this->param.Release();
	}
}

AdoParameter::operator _ParameterPtr() {
	return this->param;
}

_ParameterPtr AdoParameter::operator->() {
	return this->param;
}

AdoParameter& AdoParameter::operator=(_ParameterPtr param) {
	this->param = param;
	return *this;
}

DataTypeEnum AdoParameter::setTypeAsEmpty() {
	this->param->Type = adEmpty;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsTinyInt() {
	this->param->Type = adTinyInt;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsSmallInt() {
	this->param->Type = adSmallInt;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsInteger() {
	this->param->Type = adInteger;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsBigInt() {
	this->param->Type = adBigInt;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsUnsignedTinyInt() {
	this->param->Type = adUnsignedTinyInt;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsUnsignedSmallInt() {
	this->param->Type = adUnsignedSmallInt;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsUnsignedInt() {
	this->param->Type = adUnsignedInt;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsUnsignedBigInt() {
	this->param->Type = adUnsignedBigInt;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsSingle() {
	this->param->Type = adSingle;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsDouble() {
	this->param->Type = adDouble;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsCurrency() {
	this->param->Type = adCurrency;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsDecimal() {
	this->param->Type = adDecimal;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsNumeric() {
	this->param->Type = adNumeric;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsBoolean() {
	this->param->Type = adBoolean;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsError() {
	this->param->Type = adError;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsUserDefined() {
	this->param->Type = adUserDefined;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsVariant() {
	this->param->Type = adVariant;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsIDispatch() {
	this->param->Type = adIDispatch;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsIUnknown() {
	this->param->Type = adIUnknown;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsGUID() {
	this->param->Type = adGUID;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsDate() {
	this->param->Type = adDate;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsDBDate() {
	this->param->Type = adDBDate;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsDBTime() {
	this->param->Type = adDBTime;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsDBTimeStamp() {
	this->param->Type = adDBTimeStamp;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsBSTR() {
	this->param->Type = adBSTR;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsChar() {
	this->param->Type = adChar;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsVarChar() {
	this->param->Type = adVarChar;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsLongVarChar() {
	this->param->Type = adLongVarChar;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsWChar() {
	this->param->Type = adWChar;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsVarWChar() {
	this->param->Type = adVarWChar;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsLongVarWChar() {
	this->param->Type = adLongVarWChar;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsBinary() {
	this->param->Type = adBinary;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsVarBinary() {
	this->param->Type = adVarBinary;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsLongVarBinary() {
	this->param->Type = adLongVarBinary;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsChapter() {
	this->param->Type = adChapter;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsFileTime() {
	this->param->Type = adFileTime;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsPropVariant() {
	this->param->Type = adPropVariant;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsVarNumeric() {
	this->param->Type = adVarNumeric;
	return this->param->Type;
}

DataTypeEnum AdoParameter::setTypeAsArray() {
	this->param->Type = adArray;
	return this->param->Type;
}

//------------------------------------------------------------------------------------------------

AdoParameters::AdoParameters(void) {
	this->params.CreateInstance(__uuidof(Parameters));
}

AdoParameters::~AdoParameters(void) {
	if (this->params) {
		this->params.Release();
	}
}

AdoParameters::AdoParameters(ParametersPtr params) {
	this->params = params;
}

AdoParameters::operator ParametersPtr() {
	return this->params;
}

ParametersPtr AdoParameters::operator->() {
	return this->params;
}

AdoParameter AdoParameters::getParameter(const StringT &paramName) {
	return this->params->Item[bstr_t(paramName.c_str())];
}

//------------------------------------------------------------------------------------------------

AdoCommand::AdoCommand(void) {
	this->cmd.CreateInstance(__uuidof(Command));
	this->cmd->Prepared = true;
}

AdoCommand::AdoCommand(const AdoConnection& conn)
{
	this->cmd.CreateInstance(__uuidof(Command));
	this->cmd->Prepared = true;
	this->setConnection(conn);
}

AdoCommand::AdoCommand(_CommandPtr cmd) {
	this->cmd = cmd;
}

AdoCommand::~AdoCommand(void) {
	if (this->cmd) {
		this->cmd.Release();
	}
}

AdoCommand& AdoCommand::operator=(_CommandPtr cmd) {
	this->cmd = cmd;
	if (cmd) {
		this->cmdText = (const TCHAR*) cmd->CommandText;
	}
	this->paranNames.clear();
	return *this;
}

void AdoCommand::setConnection(const AdoConnection &conn) {
	this->cmd->ActiveConnection = conn;
}

AdoParameters AdoCommand::getParameters() {
	return this->cmd->Parameters;
}

void AdoCommand::setSql(const StringT &sql) {
	this->cmdText = sql;
	_CommandPtr newCmd;
	newCmd.CreateInstance(__uuidof(Command));
	newCmd->ActiveConnection = this->cmd->ActiveConnection;
	this->cmd.Release();
	this->cmd = newCmd;
	ReplaceAll(this->cmdText, _T("\\:"), _T("#COLON#"));
	StringT newCmdText = this->cmdText;
	_W(regex) reg = _W(regex)(_T("(:[\\w_]+[\\w_\\d]*)|\\?"));
	_W(smatch) what;
	vector<StringT> names;
	StringT::const_iterator start, end;
	start = newCmdText.begin();
	end = newCmdText.end();
	while (regex_search(start, end, what, reg)) {
		names.push_back(what[0]);
		start = what[0].second;
	}

	newCmdText = regex_replace(newCmdText, reg, _T("?"));
	ReplaceAll(newCmdText, _T("#COLON#"), _T(":"));
	ReplaceAll(this->cmdText, _T("#COLON#"), _T(":"));

	this->cmd->CommandText = newCmdText.c_str();

	int n = 0;
	for (vector<StringT>::iterator it = names.begin(); it != names.end();
			++it) {
		StringT name = *it;
		Trim(name);
		ToUpper(name);
		ReplaceAll(name, _T(":"), _T(""));
		if (_T("?") == name) {
			_W(stringstream) s;
			s << n++;
			name = s.str();
		}
		_ParameterPtr param = this->cmd->CreateParameter(name.c_str(),
				adVarChar, adParamInput, 255);
		this->cmd->Parameters->Append(param);
		param.Release();
	}
}

StringT AdoCommand::getSql() {
	return this->cmdText;
}

AdoRecordSet AdoCommand::execute(long *recordsAffected) {
	variant_t ra = 0;
	AdoRecordSet rs = this->cmd->Execute(&ra, NULL, 0);
	if (recordsAffected) {
		*recordsAffected = ra;
	}
	return rs;
}

void AdoCommand::setPrepared(bool prepared) {
	this->cmd->Prepared = prepared;
}

bool AdoCommand::getPrepared() {
	return this->cmd->Prepared;
}

AdoConnection AdoCommand::getConnection() {
	return this->cmd->GetActiveConnection();
}

_CommandPtr AdoCommand::operator->() {
	return this->cmd;
}

AdoCommand::operator _CommandPtr() {
	return this->cmd;
}

} /* namespace NS_FF */
