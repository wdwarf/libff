/*
 * AdoConnection.cpp
 *
 *  Created on: Mar 31, 2020
 *      Author: u16
 */

#include <ff/windows/AdoConnection.h>
#include <ff/windows/AdoField.h>
#include <ff/windows/AdoFields.h>
#include <ff/windows/AdoRecordSet.h>
#include <ff/String.h>
#include <msdasc.h>

using namespace std;

namespace NS_FF {

#define NONTHROW(stat) try{stat}catch(...){}
//#define NONTHROW(stat) stat

AdoConnection::AdoConnection(void) {
	HRESULT re = conn.CreateInstance(__uuidof(Connection));
	this->options = adConnectUnspecified;
	this->conn->CursorLocation = adUseClient;
}

AdoConnection::AdoConnection(const _ConnectionPtr &conn) {
	this->conn = conn;
	this->options = adConnectUnspecified;
	this->conn->CursorLocation = conn->GetCursorLocation();
}

void AdoConnection::operator =(const AdoConnection &conn) {
	this->conn = conn;
	this->options = adConnectUnspecified;
	this->conn->CursorLocation = conn.conn->GetCursorLocation();
}

void AdoConnection::operator =(const _ConnectionPtr &conn) {
	this->conn = conn;
	this->options = adConnectUnspecified;
	this->conn->CursorLocation = conn->GetCursorLocation();
}

AdoConnection::operator bool() {
	return this->conn;
}

AdoConnection::~AdoConnection(void) {
	conn.Release();
}

void AdoConnection::Open() {
	this->conn->Open("", userId, password, options);
	this->properties.clear();
	//this->getProperties(this->properties);
}

void AdoConnection::Close() {
	this->conn->Close();
}

bool AdoConnection::IsConnected() const {
	return adStateOpen == (adStateOpen & this->conn->GetState());
}

AdoConnection::operator _ConnectionPtr() const {
	return this->conn;
}

AdoConnection::operator _Connection&() const {
	return this->conn;
}

AdoConnection::operator _Connection*() const {
	return this->conn;
}

void AdoConnection::setConnectString(const StringT &connStr) {
	this->conn->ConnectionString = connStr.c_str();
}

StringT AdoConnection::getConnectionString() const {
	if (this->conn && this->conn->ConnectionString.length() > 0) {
		return (const TCHAR*) this->conn->ConnectionString;
	}
	return _T("");
}

void AdoConnection::setUserId(const StringT &userId) {
	this->userId = userId.c_str();
}

StringT AdoConnection::getUserId() const {
	return (const TCHAR*) this->userId;
}

void AdoConnection::setPassword(const StringT &pass) {
	this->password = pass.c_str();
}

StringT AdoConnection::getPassword() const {
	return (const TCHAR*) this->password;
}

void AdoConnection::setOptions(long opt) {
	this->options = opt;
}

long AdoConnection::getOptions() const {
	return this->options;
}

long AdoConnection::getState() const {
	return this->conn->GetState();
}

void AdoConnection::setCursorLocation(CursorLocationEnum cl) {
	this->conn->CursorLocation = cl;
}

CursorLocationEnum AdoConnection::getCursorLocation() const {
	return this->conn->CursorLocation;
}

void AdoConnection::setConnectMode(ConnectModeEnum mode) {
	this->conn->PutMode(mode);
}

ConnectModeEnum AdoConnection::getConnectMode() const {
	return this->conn->GetMode();
}

void AdoConnection::beginTrans() {
	this->conn->BeginTrans();
}

void AdoConnection::commitTrans() {
	this->conn->CommitTrans();
}

void AdoConnection::rollbackTrans() {
	this->conn->RollbackTrans();
}

_ConnectionPtr& AdoConnection::operator->() {
	return this->conn;
}

StringT ColumnInfo::getColumnDataTypeName(long iDataType) {
	StringT typeName;
	switch (iDataType) {
	case 2:
		typeName = _T("SMALLMONEY");
		break;
	case 3:
		typeName = _T("INT");
		break;
	case 4:
		typeName = _T("REAL");
		break;
	case 5:
		typeName = _T("FLOAT");
		break;
	case 6:
		typeName = _T("MONEY");
		break;
	case 7:
		typeName = _T("DATETIME");
		break;
	case 11:
		typeName = _T("BIT");
		break;
	case 13:
		typeName = _T("TIMESTAMP");
		break;
	case 17:
		typeName = _T("UNIQUEIDENTIFIER");
		break;
	case 20:
		typeName = _T("BIGINT");
		break;
	case 72:
		typeName = _T("UUID");
		break;
	case 128:
		typeName = _T("VARBINARY");
		break;
	case 129:
		typeName = _T("VARCHAR");
		break;
	case 130:
		typeName = _T("NVARCHAR");
		break;
	case 131:
		typeName = _T("DECIMAL");
		break;
	case 135:
		typeName = _T("TIMESTAMP");
		break;
	case 139:
		typeName = _T("NUMBER");
		break;
	case 200:
		typeName = _T("");
		break;
	case 201:
		typeName = _T("TIMESTAMP");
		break;
	case 202:
		typeName = _T("NVARCHAR");
		break;
	case 203:
		typeName = _T("NTEXT");
		break;
	case 204:
		typeName = _T("VARCHAR");
		break;
	case 205:
		typeName = _T("IMAGE");
		break;
	}
	return typeName;
}

set<StringT> AdoConnection::getPrimaryKeys(const StringT &tableName,
		const StringT &tableSchema, const StringT &tableCatalog) {
	set<StringT> re;
	AdoRecordSet rs = this->conn->OpenSchema(adSchemaPrimaryKeys);
	_W(stringstream) filterStr;
	filterStr << _T("TABLE_NAME='") << tableName << _T("'");
	if (!tableSchema.empty()) {
		filterStr << _T(" and TABLE_SCHEMA='") << tableSchema << _T("'");
	}
	if (!tableCatalog.empty()) {
		filterStr << _T(" and TABLE_CATALOG='") << tableCatalog << _T("'");
	}
	rs->Filter = filterStr.str().c_str();
	while (!rs.eof()) {
		AdoFields fs = rs.getFields();
		re.insert(fs[_T("COLUMN_NAME")]);
		rs.next();
	}
	return re;
}

StringT AdoConnection::getDatabaseName() const {
	bstr_t name = this->conn->GetDefaultDatabase();
	if (name.length() > 0) {
		return (const TCHAR*) name;
	} else {
		return _T("");
	}
}

map<StringT, TableInfo> AdoConnection::getTableInfos() {
	map<StringT, TableInfo> re;

	StringT userName = this->getProperty(_T("User Name"));
	StringT schemaUsage = this->getProperty(_T("Schema Usage"));
	StringT dbmsName = this->getProperty(_T("DBMS Name"));
	ToLower(dbmsName);
	long iSchemaUsage = 0;
	SStreamT str;
	str << schemaUsage;
	str >> iSchemaUsage;

	variant_t vtTableSchema(bstr_t(userName.c_str()));

	variant_t var;
	var.vt = VT_EMPTY;
	variant_t vtCriteria;
	long ix = 0;
	SAFEARRAY *pSa = SafeArrayCreateVector(VT_VARIANT, 0, 4);
	ix = 0;
	SafeArrayPutElement(pSa, &ix, &var);
	ix = 1;
	if ((iSchemaUsage > 0) && (_T("oracle") == dbmsName))
		SafeArrayPutElement(pSa, &ix, &vtTableSchema);
	else
		SafeArrayPutElement(pSa, &ix, &var);
	ix = 2;
	SafeArrayPutElement(pSa, &ix, &var);
	ix = 3;
	SafeArrayPutElement(pSa, &ix, &var);
	vtCriteria.vt = VT_ARRAY | VT_VARIANT;
	vtCriteria.parray = pSa;

	AdoRecordSet rs = this->conn->OpenSchema(adSchemaTables, vtCriteria);
	while (!rs.eof()) {
		AdoFields fields = rs->GetFields();
		StringT tbType = fields[_T("TABLE_TYPE")].valAsString();
		if (_T("TABLE") == tbType) {
			TableInfo info;
			NONTHROW(info.setCatalog(fields[_T("TABLE_CATALOG")].valAsString()););
			NONTHROW(info.setSchema(fields[_T("TABLE_SCHEMA")].valAsString()););
			NONTHROW(info.setName(fields[_T("TABLE_NAME")].valAsString()););
			NONTHROW(info.setType(fields[_T("TABLE_TYPE")].valAsString()););
			NONTHROW(info.setGuid(fields[_T("TABLE_GUID")].valAsString()););
			NONTHROW(info.setDescription(fields[_T("DESCRIPTION")].valAsString()););
			NONTHROW(info.setPropid(fields[_T("TABLE_PROPID")].valAsString()););
			NONTHROW(info.setCreatedDate(fields[_T("DATE_CREATED")].valAsString()););
			NONTHROW(info.setModifiedDate(fields[_T("DATE_MODIFIED")].valAsString()););

			info.setPrimaryKeys(
			this->getPrimaryKeys(info.getName(), info.getSchema(), info.getCatalog()));

			re.insert(make_pair(info.getName(), info));
		}
		rs->MoveNext();
	}

	for (map<StringT, TableInfo>::iterator it = re.begin(); it != re.end();
			++it) {
		StringT tableName = it->second.getName();
		variant_t vtTbName(bstr_t(tableName.c_str()));
		variant_t vtTableSchema(bstr_t(it->second.getSchema().c_str()));
		variant_t vtTableCatalog(bstr_t(it->second.getCatalog().c_str()));

		variant_t var;
		var.vt = VT_EMPTY;
		variant_t vtCriteria;
		long ix = 0;
		SAFEARRAY *pSa = SafeArrayCreateVector(VT_VARIANT, 0, 4);
		ix = 0;
		if (iSchemaUsage > 0)
			SafeArrayPutElement(pSa, &ix, &vtTableCatalog);
		else
			SafeArrayPutElement(pSa, &ix, &var);
		ix = 1;
		if (iSchemaUsage > 0)
			SafeArrayPutElement(pSa, &ix, &vtTableSchema);
		else
			SafeArrayPutElement(pSa, &ix, &var);
		ix = 2;
		SafeArrayPutElement(pSa, &ix, &vtTbName);
		ix = 3;
		SafeArrayPutElement(pSa, &ix, &var);
		vtCriteria.vt = VT_ARRAY | VT_VARIANT;
		vtCriteria.parray = pSa;

		rs = this->conn->OpenSchema(adSchemaColumns, vtCriteria);

		while (!rs.eof()) {
			AdoFields fields = rs->GetFields();

			ColumnInfo columnInfo;
			NONTHROW(columnInfo.setTableCatalog(fields[_T("TABLE_CATALOG")]););
			NONTHROW(columnInfo.setTableSchema(fields[_T("TABLE_SCHEMA")]););
			NONTHROW(columnInfo.setTableName(fields[_T("TABLE_NAME")]););
			NONTHROW(columnInfo.setColumnName(fields[_T("COLUMN_NAME")]););
			NONTHROW(columnInfo.setColumnGuid(fields[_T("COLUMN_GUID")]););
			NONTHROW(columnInfo.setColumnPropid(fields[_T("COLUMN_PROPID")]););
			NONTHROW(columnInfo.setOrdinalPosition(fields[_T("ORDINAL_POSITION")]););
			NONTHROW(columnInfo.setColumnHasdefault(fields[_T("COLUMN_HASDEFAULT")]););
			NONTHROW(columnInfo.setColumnDefault(fields[_T("COLUMN_DEFAULT")]););
			NONTHROW(columnInfo.setColumnFlags(fields[_T("COLUMN_FLAGS")]););
			NONTHROW(columnInfo.setIsNullable(fields[_T("IS_NULLABLE")]););
			NONTHROW(columnInfo.setDataType(fields[_T("DATA_TYPE")]););
			NONTHROW(columnInfo.setTypeGuid(fields[_T("TYPE_GUID")]););
			NONTHROW(columnInfo.setCharacterMaximumLength(fields[_T("CHARACTER_MAXIMUM_LENGTH")]););
			NONTHROW(columnInfo.setCharacterOctetLength(fields[_T("CHARACTER_OCTET_LENGTH")]););
			NONTHROW(columnInfo.setNumericPrecision(fields[_T("NUMERIC_PRECISION")]););
			NONTHROW(columnInfo.setNumericScale(fields[_T("NUMERIC_SCALE")]););
			NONTHROW(columnInfo.setDatetimePrecision(fields[_T("DATETIME_PRECISION")]););
			NONTHROW(columnInfo.setCharacterSetCatalog(fields[_T("CHARACTER_SET_CATALOG")]););
			NONTHROW(columnInfo.setCharacterSetSchema(fields[_T("CHARACTER_SET_SCHEMA")]););
			NONTHROW(columnInfo.setCharacterSetName(fields[_T("CHARACTER_SET_NAME")]););
			NONTHROW(columnInfo.setCollationCatalog(fields[_T("COLLATION_CATALOG")]););
			NONTHROW(columnInfo.setCollationSchema(fields[_T("COLLATION_SCHEMA")]););
			NONTHROW(columnInfo.setCollationName(fields[_T("COLLATION_NAME")]););
			NONTHROW(columnInfo.setDomainCatalog(fields[_T("DOMAIN_CATALOG")]););
			NONTHROW(columnInfo.setDomainSchema(fields[_T("DOMAIN_SCHEMA")]););
			NONTHROW(columnInfo.setDomainName(fields[_T("DOMAIN_NAME")]););
			NONTHROW(columnInfo.setDescription(fields[_T("DESCRIPTION")]););
			NONTHROW(columnInfo.setColumnLcid(fields[_T("COLUMN_LCID")]););
			NONTHROW(columnInfo.setColumnCompflags(fields[_T("COLUMN_COMPFLAGS")]););
			NONTHROW(columnInfo.setColumnSortid(fields[_T("COLUMN_SORTID")]););
			NONTHROW(columnInfo.setColumnTdscollation(fields[_T("COLUMN_TDSCOLLATION")]););
			NONTHROW(columnInfo.setIsComputed(fields[_T("IS_COMPUTED")]););

			rs->MoveNext();
		}
	}
	return re;
}

TableInfo AdoConnection::getTableInfo(const StringT &tableName) {
	TableInfo re;
	StringT upperTbName = tableName;
	Trim(upperTbName);
	ToUpper(upperTbName);

	StringT userName = this->getProperty(_T("User Name"));
	StringT schemaUsage = this->getProperty(_T("Schema Usage"));
	long iSchemaUsage = 0;
	_W(stringstream) str;
	str << schemaUsage;
	str >> iSchemaUsage;
	StringT dbmsName = this->getProperty(_T("DBMS Name"));
	ToLower(dbmsName);

	variant_t var;
	var.vt = VT_EMPTY;
	variant_t vtCriteria;
	long ix = 0;
	variant_t vtTbName(bstr_t(upperTbName.c_str()));
	variant_t vtTableSchema(bstr_t(userName.c_str()));
	SAFEARRAY *pSa = SafeArrayCreateVector(VT_VARIANT, 0, 4);
	ix = 0;
	SafeArrayPutElement(pSa, &ix, &var);
	ix = 1;
	if ((iSchemaUsage > 0) && (_T("oracle") == dbmsName))
		SafeArrayPutElement(pSa, &ix, &vtTableSchema);
	else
		SafeArrayPutElement(pSa, &ix, &var);
	ix = 2;
	SafeArrayPutElement(pSa, &ix, &vtTbName);
	ix = 3;
	SafeArrayPutElement(pSa, &ix, &var);
	vtCriteria.vt = VT_ARRAY | VT_VARIANT;
	vtCriteria.parray = pSa;

	AdoRecordSet rs = this->conn->OpenSchema(adSchemaTables, vtCriteria);

	while (!rs.eof()) {
		AdoFields fields = rs->GetFields();

		StringT tbType = fields[_T("TABLE_TYPE")].valAsString();
		if (_T("TABLE") == tbType) {
			StringT tbName = fields[_T("TABLE_NAME")].valAsString();
			Trim(tbName);
			ToUpper(tbName);
			if (tbName == upperTbName) {
				NONTHROW(re.setCatalog(fields[_T("TABLE_CATALOG")].valAsString()););
				NONTHROW(re.setSchema(fields[_T("TABLE_SCHEMA")].valAsString()););
				NONTHROW(re.setName(fields[_T("TABLE_NAME")].valAsString()););
				NONTHROW(re.setType(fields[_T("TABLE_TYPE")].valAsString()););
				NONTHROW(re.setGuid(fields[_T("TABLE_GUID")].valAsString()););
				NONTHROW(re.setDescription(fields[_T("DESCRIPTION")].valAsString()););
				NONTHROW(re.setPropid(fields[_T("TABLE_PROPID")].valAsString()););
				NONTHROW(re.setCreatedDate(fields[_T("DATE_CREATED")].valAsString()););
				NONTHROW(re.setModifiedDate(fields[_T("DATE_MODIFIED")].valAsString()););

				re.setPrimaryKeys(
				this->getPrimaryKeys(re.getName(), re.getSchema(), re.getCatalog()));

				break;
			}
		}
		rs->MoveNext();
	}

	if (re.getName().empty()) {
		return re;
	}

	StringT tbSchema = re.getSchema();
	Trim(tbSchema);
	ToUpper(tbSchema);

	rs = this->conn->OpenSchema(adSchemaColumns, vtCriteria);
	while (!rs.eof()) {
		AdoFields fields = rs->GetFields();
		StringT colTbName = fields[_T("TABLE_NAME")].valAsString();
		StringT colTbSchema = fields[_T("TABLE_SCHEMA")];
		Trim(colTbName);
		ToUpper(colTbName);
		Trim(colTbSchema);
		ToUpper(colTbSchema);

		if ((colTbName == tableName) && (tbSchema == colTbSchema)) {
			ColumnInfo columnInfo;
			NONTHROW(columnInfo.setTableCatalog(fields[_T("TABLE_CATALOG")]););
			NONTHROW(columnInfo.setTableSchema(fields[_T("TABLE_SCHEMA")]););
			NONTHROW(columnInfo.setTableName(fields[_T("TABLE_NAME")]););
			NONTHROW(columnInfo.setColumnName(fields[_T("COLUMN_NAME")]););
			NONTHROW(columnInfo.setColumnGuid(fields[_T("COLUMN_GUID")]););
			NONTHROW(columnInfo.setColumnPropid(fields[_T("COLUMN_PROPID")]););
			NONTHROW(columnInfo.setOrdinalPosition(fields[_T("ORDINAL_POSITION")]););
			NONTHROW(columnInfo.setColumnHasdefault(fields[_T("COLUMN_HASDEFAULT")]););
			NONTHROW(columnInfo.setColumnDefault(fields[_T("COLUMN_DEFAULT")]););
			NONTHROW(columnInfo.setColumnFlags(fields[_T("COLUMN_FLAGS")]););
			NONTHROW(columnInfo.setIsNullable(fields[_T("IS_NULLABLE")]););
			NONTHROW(columnInfo.setDataType(fields[_T("DATA_TYPE")]););
			NONTHROW(columnInfo.setTypeGuid(fields[_T("TYPE_GUID")]););
			NONTHROW(columnInfo.setCharacterMaximumLength(fields[_T("CHARACTER_MAXIMUM_LENGTH")]););
			NONTHROW(columnInfo.setCharacterOctetLength(fields[_T("CHARACTER_OCTET_LENGTH")]););
			NONTHROW(columnInfo.setNumericPrecision(fields[_T("NUMERIC_PRECISION")]););
			NONTHROW(columnInfo.setNumericScale(fields[_T("NUMERIC_SCALE")]););
			NONTHROW(columnInfo.setDatetimePrecision(fields[_T("DATETIME_PRECISION")]););
			NONTHROW(columnInfo.setCharacterSetCatalog(fields[_T("CHARACTER_SET_CATALOG")]););
			NONTHROW(columnInfo.setCharacterSetSchema(fields[_T("CHARACTER_SET_SCHEMA")]););
			NONTHROW(columnInfo.setCharacterSetName(fields[_T("CHARACTER_SET_NAME")]););
			NONTHROW(columnInfo.setCollationCatalog(fields[_T("COLLATION_CATALOG")]););
			NONTHROW(columnInfo.setCollationSchema(fields[_T("COLLATION_SCHEMA")]););
			NONTHROW(columnInfo.setCollationName(fields[_T("COLLATION_NAME")]););
			NONTHROW(columnInfo.setDomainCatalog(fields[_T("DOMAIN_CATALOG")]););
			NONTHROW(columnInfo.setDomainSchema(fields[_T("DOMAIN_SCHEMA")]););
			NONTHROW(columnInfo.setDomainName(fields[_T("DOMAIN_NAME")]););
			NONTHROW(columnInfo.setDescription(fields[_T("DESCRIPTION")]););
			NONTHROW(columnInfo.setColumnLcid(fields[_T("COLUMN_LCID")]););
			NONTHROW(columnInfo.setColumnCompflags(fields[_T("COLUMN_COMPFLAGS")]););
			NONTHROW(columnInfo.setColumnSortid(fields[_T("COLUMN_SORTID")]););
			NONTHROW(columnInfo.setColumnTdscollation(fields[_T("COLUMN_TDSCOLLATION")]););
			NONTHROW(columnInfo.setIsComputed(fields[_T("IS_COMPUTED")]););

			re.getColumnInfos().insert(make_pair(columnInfo.getColumnName(), columnInfo));
		}

		rs->MoveNext();
	}

	return re;
}

set<StringT> AdoConnection::getTableNames() {
	set<StringT> re;

	StringT userName = this->getProperty(_T("User Name"));
	StringT schemaUsage = this->getProperty(_T("Schema Usage"));
	long iSchemaUsage = 0;
	_W(stringstream) str;
	str << schemaUsage;
	str >> iSchemaUsage;
	StringT dbmsName = this->getProperty(_T("DBMS Name"));
	ToLower(dbmsName);

	variant_t var;
	var.vt = VT_EMPTY;
	variant_t vtCriteria;
	long ix = 0;
	variant_t vtUserName(bstr_t(userName.c_str()));
	SAFEARRAY *pSa = SafeArrayCreateVector(VT_VARIANT, 0, 4);
	ix = 0;
	SafeArrayPutElement(pSa, &ix, &var);
	ix = 1;
	if ((iSchemaUsage > 0) && (_T("oracle") == dbmsName))
		SafeArrayPutElement(pSa, &ix, &vtUserName);
	else
		SafeArrayPutElement(pSa, &ix, &var);
	ix = 2;
	SafeArrayPutElement(pSa, &ix, &var);
	ix = 3;
	SafeArrayPutElement(pSa, &ix, &var);
	vtCriteria.vt = VT_ARRAY | VT_VARIANT;
	vtCriteria.parray = pSa;

	AdoRecordSet rs = this->conn->OpenSchema(adSchemaTables, vtCriteria);
	while (!rs.eof()) {
		AdoFields fields = rs->GetFields();
		StringT tbType = fields[_T("TABLE_TYPE")].valAsString();
		if (_T("TABLE") == tbType) {
			StringT tbName = fields[_T("TABLE_NAME")].valAsString();
			re.insert(tbName);
		}
		rs->MoveNext();
	}

	return re;
}

map<StringT, StringT>& AdoConnection::getProperties() {
	if (this->properties.empty()) {
		this->getProperties(this->properties);
	}
	return this->properties;
}

StringT AdoConnection::getProperty(const StringT &propName) {
	StringT re;

	if (this->properties.empty()) {
		this->getProperties(this->properties);
	}

	map<StringT, StringT>::iterator it = this->properties.find(propName);
	if (it != this->properties.end()) {
		return it->second;
	}
	return re;
}

void AdoConnection::getProperties(map<StringT, StringT> &properties) {
	::PropertiesPtr props = this->conn->Properties;
	for (long i = 0; i < props->Count; ++i) {
		bstr_t name = props->Item[i]->Name;
		bstr_t val = props->Item[i]->Value;
		properties.insert(
				make_pair(AdoObject::BstrToStr(name),
						AdoObject::BstrToStr(val)));
	}
}

void AdoConnection::setAccessConnStr(const StringT &dbFilePath,
		const StringT &password) {
	StringT connStr =
	_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=\"") + dbFilePath
			+ _T("\";Persist Security Info=False;Jet OLEDB:Database Password=")
			+ password;
	this->conn->ConnectionString = connStr.c_str();
}

void AdoConnection::setODBCConnStr(const StringT &dataSourceName,
		const StringT &dbName, const StringT &userId, const StringT &password) {
	StringT connStr = _T("Provider=MSDASQL.1;Password=") + password
			+ _T(";Persist Security Info=True;User ID=") + userId
			+ _T(";Data Source=") + dataSourceName + _T(";Initial Catalog=")
			+ dbName;
	this->conn->ConnectionString = connStr.c_str();
}

void AdoConnection::setSQLServerConnStr(const StringT &serverName,
		const StringT &dbName, const StringT &userId, const StringT &password) {
	StringT connStr = _T("Provider=SQLOLEDB.1;Password=") + password
			+ _T(";Persist Security Info=True;User ID=") + userId
			+ _T(";Initial Catalog=") + dbName + _T(";Data Source=")
			+ serverName;
	this->conn->ConnectionString = connStr.c_str();
}

void AdoConnection::setMYSQLConnStr(const StringT &serverName,
		const StringT &dbName, const StringT &userId, const StringT &password,
		const StringT &port, const StringT &driverName) {
	StringT connStr =
			_T(
					"Provider=MSDASQL.1;Persist Security Info=True;Extended Properties=\"DRIVER={") + driverName + _T("};UID=") + userId + _T(";PWD=") + password + _T(";SERVER=") + serverName
					+ _T(";DATABASE=") + dbName + _T(";PORT=") + port + _T(";COLUMN_SIZE_S32=1;\"");
	this->conn->ConnectionString = connStr.c_str();
}

void AdoConnection::setORACLEConnStr(const StringT &serverName,
		const StringT &serviceName, const StringT &userId,
		const StringT &password, const StringT &port) {
	StringT connStr =
			_T(
					"Provider=OraOLEDB.Oracle.1;Password=") + password + _T(";User ID=") + userId
					+ _T(";Data Source=\"(DESCRIPTION =(ADDRESS_LIST =(ADDRESS = (PROTOCOL = TCP)(HOST = (") + serverName
					+ _T(")(PORT = ") + port + _T(")))(CONNECT_DATA =(SERVICE_NAME = ") + serviceName + _T(")))\"");
	this->conn->ConnectionString = connStr.c_str();
}

StringT AdoConnection::PromptDataSource(HWND hWndParent) {
	StringT connStr;
	wOLEDB::IDataSourceLocatorPtr dlPrompt;
	HRESULT re = dlPrompt.CreateInstance(__uuidof(DataLinks));
	_ConnectionPtr p_conn;
	if (SUCCEEDED(re)) {
		dlPrompt->hWnd = (long) hWndParent;
		p_conn = dlPrompt->PromptNew();
		if (p_conn) {
			bstr_t str = p_conn->ConnectionString;
			if (str.length() > 0) {
				connStr = (const TCHAR*) str;
			}
			p_conn.Release();
		}
		dlPrompt.Release();
	}
	return connStr;
}

bool AdoConnection::PromptDataSource(AdoConnection &conn, HWND hWndParent) {
	wOLEDB::IDataSourceLocatorPtr dlPrompt;
	HRESULT re = dlPrompt.CreateInstance(__uuidof(DataLinks));
	_ConnectionPtr p_conn = conn;
	if (SUCCEEDED(re)) {
		dlPrompt->hWnd = (long) hWndParent;
		_ConnectionPtr::Interface *ip = p_conn.GetInterfacePtr();
		if (ip) {
			return (TRUE == dlPrompt->PromptEdit((IDispatch**) &ip));
		}
		dlPrompt.Release();
	}
	return false;
}

bool AdoConnection::CreateAccessFile(const StringT &dbFilePath,
		const StringT &password) {
	HRESULT hr = S_FALSE;
	StringT connStr(
			_T(
					"Provider=Microsoft.JET.OLEDB.4.0;jet OleDB:Database Password=") + password + _T(";Data source = "));
	connStr += dbFilePath;

	try {
		ADOX::_CatalogPtr m_pCatalog = NULL;
		hr = m_pCatalog.CreateInstance(__uuidof(ADOX::Catalog));
		if (FAILED(hr)) {
			_com_issue_error(hr);
			return false;
		}

		hr = m_pCatalog->Create(_bstr_t(connStr.c_str()));
		m_pCatalog.Release();
	} catch (...) {
	}

	return FAILED(hr);
}

//===========================================================

StringT ColumnInfo::getTableCatalog() {
	return this->TABLE_CATALOG;
}

void ColumnInfo::setTableCatalog(const StringT &TableCatalog) {
	this->TABLE_CATALOG = TableCatalog;
}

StringT ColumnInfo::getTableSchema() {
	return this->TABLE_SCHEMA;
}

void ColumnInfo::setTableSchema(const StringT &TableSchema) {
	this->TABLE_SCHEMA = TableSchema;
}

StringT ColumnInfo::getTableName() {
	return this->TABLE_NAME;
}

void ColumnInfo::setTableName(const StringT &TableName) {
	this->TABLE_NAME = TableName;
}

StringT ColumnInfo::getColumnName() {
	return this->COLUMN_NAME;
}

void ColumnInfo::setColumnName(const StringT &ColumnName) {
	this->COLUMN_NAME = ColumnName;
}

StringT ColumnInfo::getColumnGuid() {
	return this->COLUMN_GUID;
}

void ColumnInfo::setColumnGuid(const StringT &ColumnGuid) {
	this->COLUMN_GUID = ColumnGuid;
}

StringT ColumnInfo::getColumnPropid() {
	return this->COLUMN_PROPID;
}

void ColumnInfo::setColumnPropid(const StringT &ColumnPropid) {
	this->COLUMN_PROPID = ColumnPropid;
}

long ColumnInfo::getOrdinalPosition() {
	return this->ORDINAL_POSITION;
}

void ColumnInfo::setOrdinalPosition(long OrdinalPosition) {
	this->ORDINAL_POSITION = OrdinalPosition;
}

StringT ColumnInfo::getColumnHasdefault() {
	return this->COLUMN_HASDEFAULT;
}

void ColumnInfo::setColumnHasdefault(const StringT &ColumnHasdefault) {
	this->COLUMN_HASDEFAULT = ColumnHasdefault;
}

StringT ColumnInfo::getColumnDefault() {
	return this->COLUMN_DEFAULT;
}

void ColumnInfo::setColumnDefault(const StringT &ColumnDefault) {
	this->COLUMN_DEFAULT = ColumnDefault;
}

StringT ColumnInfo::getColumnFlags() {
	return this->COLUMN_FLAGS;
}

void ColumnInfo::setColumnFlags(const StringT &ColumnFlags) {
	this->COLUMN_FLAGS = ColumnFlags;
}

bool ColumnInfo::getIsNullable() {
	return this->IS_NULLABLE;
}

void ColumnInfo::setIsNullable(bool IsNullable) {
	this->IS_NULLABLE = IsNullable;
}

long ColumnInfo::getDataType() {
	return this->DATA_TYPE;
}

void ColumnInfo::setDataType(long DataType) {
	this->DATA_TYPE = DataType;
}

StringT ColumnInfo::getTypeGuid() {
	return this->TYPE_GUID;
}

void ColumnInfo::setTypeGuid(const StringT &TypeGuid) {
	this->TYPE_GUID = TypeGuid;
}

long ColumnInfo::getCharacterMaximumLength() {
	return this->CHARACTER_MAXIMUM_LENGTH;
}

void ColumnInfo::setCharacterMaximumLength(long CharacterMaximumLength) {
	this->CHARACTER_MAXIMUM_LENGTH = CharacterMaximumLength;
}

long ColumnInfo::getCharacterOctetLength() {
	return this->CHARACTER_OCTET_LENGTH;
}

void ColumnInfo::setCharacterOctetLength(long CharacterOctetLength) {
	this->CHARACTER_OCTET_LENGTH = CharacterOctetLength;
}

long ColumnInfo::getNumericPrecision() {
	return this->NUMERIC_PRECISION;
}

void ColumnInfo::setNumericPrecision(long NumericPrecision) {
	this->NUMERIC_PRECISION = NumericPrecision;
}

long ColumnInfo::getNumericScale() {
	return this->NUMERIC_SCALE;
}

void ColumnInfo::setNumericScale(long NumericScale) {
	this->NUMERIC_SCALE = NumericScale;
}

long ColumnInfo::getDatetimePrecision() {
	return this->DATETIME_PRECISION;
}

void ColumnInfo::setDatetimePrecision(long DatetimePrecision) {
	this->DATETIME_PRECISION = DatetimePrecision;
}

StringT ColumnInfo::getCharacterSetCatalog() {
	return this->CHARACTER_SET_CATALOG;
}

void ColumnInfo::setCharacterSetCatalog(const StringT &CharacterSetCatalog) {
	this->CHARACTER_SET_CATALOG = CharacterSetCatalog;
}

StringT ColumnInfo::getCharacterSetSchema() {
	return this->CHARACTER_SET_SCHEMA;
}

void ColumnInfo::setCharacterSetSchema(const StringT &CharacterSetSchema) {
	this->CHARACTER_SET_SCHEMA = CharacterSetSchema;
}

StringT ColumnInfo::getCharacterSetName() {
	return this->CHARACTER_SET_NAME;
}

void ColumnInfo::setCharacterSetName(const StringT &CharacterSetName) {
	this->CHARACTER_SET_NAME = CharacterSetName;
}

StringT ColumnInfo::getCollationCatalog() {
	return this->COLLATION_CATALOG;
}

void ColumnInfo::setCollationCatalog(const StringT &CollationCatalog) {
	this->COLLATION_CATALOG = CollationCatalog;
}

StringT ColumnInfo::getCollationSchema() {
	return this->COLLATION_SCHEMA;
}

void ColumnInfo::setCollationSchema(const StringT &CollationSchema) {
	this->COLLATION_SCHEMA = CollationSchema;
}

StringT ColumnInfo::getCollationName() {
	return this->COLLATION_NAME;
}

void ColumnInfo::setCollationName(const StringT &CollationName) {
	this->COLLATION_NAME = CollationName;
}

StringT ColumnInfo::getDomainCatalog() {
	return this->DOMAIN_CATALOG;
}

void ColumnInfo::setDomainCatalog(const StringT &DomainCatalog) {
	this->DOMAIN_CATALOG = DomainCatalog;
}

StringT ColumnInfo::getDomainSchema() {
	return this->DOMAIN_SCHEMA;
}

void ColumnInfo::setDomainSchema(const StringT &DomainSchema) {
	this->DOMAIN_SCHEMA = DomainSchema;
}

StringT ColumnInfo::getDomainName() {
	return this->DOMAIN_NAME;
}

void ColumnInfo::setDomainName(const StringT &DomainName) {
	this->DOMAIN_NAME = DomainName;
}

StringT ColumnInfo::getDescription() {
	return this->DESCRIPTION;
}

void ColumnInfo::setDescription(const StringT &Description) {
	this->DESCRIPTION = Description;
}

StringT ColumnInfo::getColumnLcid() {
	return this->COLUMN_LCID;
}

void ColumnInfo::setColumnLcid(const StringT &ColumnLcid) {
	this->COLUMN_LCID = ColumnLcid;
}

StringT ColumnInfo::getColumnCompflags() {
	return this->COLUMN_COMPFLAGS;
}

void ColumnInfo::setColumnCompflags(const StringT &ColumnCompflags) {
	this->COLUMN_COMPFLAGS = ColumnCompflags;
}

StringT ColumnInfo::getColumnSortid() {
	return this->COLUMN_SORTID;
}

void ColumnInfo::setColumnSortid(const StringT &ColumnSortid) {
	this->COLUMN_SORTID = ColumnSortid;
}

StringT ColumnInfo::getColumnTdscollation() {
	return this->COLUMN_TDSCOLLATION;
}

void ColumnInfo::setColumnTdscollation(const StringT &ColumnTdscollation) {
	this->COLUMN_TDSCOLLATION = ColumnTdscollation;
}

bool ColumnInfo::getIsComputed() {
	return this->IS_COMPUTED;
}

void ColumnInfo::setIsComputed(bool IsComputed) {
	this->IS_COMPUTED = IsComputed;
}

} /* namespace NS_FF */
