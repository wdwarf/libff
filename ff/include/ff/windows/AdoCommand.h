/*
 * AdoCommand.h
 *
 *  Created on: Mar 31, 2020
 *      Author: u16
 */

#ifndef WINDOWS_ADOCOMMAND_H_
#define WINDOWS_ADOCOMMAND_H_

#include <ff/ff_config.h>
#include <ff/windows/AdoObject.h>
#include <ff/windows/AdoConnection.h>
#include <ff/windows/AdoRecordSet.h>
#include <vector>
#include <string>

namespace NS_FF {

class LIBFF_API AdoParameter: public AdoObject {
public:
	AdoParameter(void);
	AdoParameter(_ParameterPtr param);
	virtual ~AdoParameter(void);

	operator _ParameterPtr();
	_ParameterPtr operator->();
	AdoParameter& operator=(_ParameterPtr param);

	DataTypeEnum setTypeAsEmpty();
	DataTypeEnum setTypeAsTinyInt();
	DataTypeEnum setTypeAsSmallInt();
	DataTypeEnum setTypeAsInteger();
	DataTypeEnum setTypeAsBigInt();
	DataTypeEnum setTypeAsUnsignedTinyInt();
	DataTypeEnum setTypeAsUnsignedSmallInt();
	DataTypeEnum setTypeAsUnsignedInt();
	DataTypeEnum setTypeAsUnsignedBigInt();
	DataTypeEnum setTypeAsSingle();
	DataTypeEnum setTypeAsDouble();
	DataTypeEnum setTypeAsCurrency();
	DataTypeEnum setTypeAsDecimal();
	DataTypeEnum setTypeAsNumeric();
	DataTypeEnum setTypeAsBoolean();
	DataTypeEnum setTypeAsError();
	DataTypeEnum setTypeAsUserDefined();
	DataTypeEnum setTypeAsVariant();
	DataTypeEnum setTypeAsIDispatch();
	DataTypeEnum setTypeAsIUnknown();
	DataTypeEnum setTypeAsGUID();
	DataTypeEnum setTypeAsDate();
	DataTypeEnum setTypeAsDBDate();
	DataTypeEnum setTypeAsDBTime();
	DataTypeEnum setTypeAsDBTimeStamp();
	DataTypeEnum setTypeAsBSTR();
	DataTypeEnum setTypeAsChar();
	DataTypeEnum setTypeAsVarChar();
	DataTypeEnum setTypeAsLongVarChar();
	DataTypeEnum setTypeAsWChar();
	DataTypeEnum setTypeAsVarWChar();
	DataTypeEnum setTypeAsLongVarWChar();
	DataTypeEnum setTypeAsBinary();
	DataTypeEnum setTypeAsVarBinary();
	DataTypeEnum setTypeAsLongVarBinary();
	DataTypeEnum setTypeAsChapter();
	DataTypeEnum setTypeAsFileTime();
	DataTypeEnum setTypeAsPropVariant();
	DataTypeEnum setTypeAsVarNumeric();
	DataTypeEnum setTypeAsArray();

private:
	_ParameterPtr param;
};

class LIBFF_API AdoParameters: public AdoObject {
public:
	AdoParameters(void);
	AdoParameters(ParametersPtr params);
	virtual ~AdoParameters(void);

	operator ParametersPtr();
	ParametersPtr operator->();
	AdoParameter getParameter(const StringT &paramName);

	template<class T>
	AdoParameter operator[](T paramName) {
		SStreamT str;
		str << paramName;
		return this->params->Item[str.str().c_str()];
	}

private:
	ParametersPtr params;
};

class LIBFF_API AdoCommand: public AdoObject {
public:
	AdoCommand(void);
	AdoCommand(const AdoConnection& conn);
	AdoCommand(_CommandPtr cmd);
	virtual ~AdoCommand(void);

	AdoRecordSet Execute();
	void setConnection(const AdoConnection &conn);
	AdoConnection getConnection();

	void setPrepared(bool prepared);
	bool getPrepared();
	void setSql(const StringT &sql);
	StringT getSql();
	AdoRecordSet execute(long *recordsAffected = NULL);

	operator _CommandPtr();
	_CommandPtr operator->();
	AdoCommand& operator=(_CommandPtr cmd);

	AdoParameters getParameters();

	template<class T>
	AdoParameter operator[](T paramName) {
		SStreamT str;
		str << paramName;
		return this->cmd->Parameters->Item[str.str().c_str()];
	}

private:
	_CommandPtr cmd;
	StringT cmdText;
	std::vector<StringT> paranNames;
};

} /* namespace NS_FF */

#endif /* WINDOWS_ADOCOMMAND_H_ */
