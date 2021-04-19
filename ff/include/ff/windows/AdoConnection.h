/*
 * AdoConnection.h
 *
 *  Created on: Mar 31, 2020
 *      Author: liyawu
 */

#ifndef WINDOWS_ADOCONNECTION_H_
#define WINDOWS_ADOCONNECTION_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <ff/ff_config.h>
#include <ff/windows/AdoObject.h>

NS_FF_BEG

class LIBFF_API ColumnInfo {
public:
	ColumnInfo() :
			isPrimaryKey(false) {
	}

	StringT getTableCatalog();
	void setTableCatalog(const StringT &TableCatalog);
	StringT getTableSchema();
	void setTableSchema(const StringT &TableSchema);
	StringT getTableName();
	void setTableName(const StringT &TableName);
	StringT getColumnName();
	void setColumnName(const StringT &ColumnName);
	StringT getColumnGuid();
	void setColumnGuid(const StringT &ColumnGuid);
	StringT getColumnPropid();
	void setColumnPropid(const StringT &ColumnPropid);
	long getOrdinalPosition();
	void setOrdinalPosition(long OrdinalPosition);
	StringT getColumnHasdefault();
	void setColumnHasdefault(const StringT &ColumnHasdefault);
	StringT getColumnDefault();
	void setColumnDefault(const StringT &ColumnDefault);
	StringT getColumnFlags();
	void setColumnFlags(const StringT &ColumnFlags);
	bool getIsNullable();
	void setIsNullable(bool IsNullable);
	long getDataType();
	void setDataType(long DataType);
	StringT getTypeGuid();
	void setTypeGuid(const StringT &TypeGuid);
	long getCharacterMaximumLength();
	void setCharacterMaximumLength(long CharacterMaximumLength);
	long getCharacterOctetLength();
	void setCharacterOctetLength(long CharacterOctetLength);
	long getNumericPrecision();
	void setNumericPrecision(long NumericPrecision);
	long getNumericScale();
	void setNumericScale(long NumericScale);
	long getDatetimePrecision();
	void setDatetimePrecision(long DatetimePrecision);
	StringT getCharacterSetCatalog();
	void setCharacterSetCatalog(const StringT &CharacterSetCatalog);
	StringT getCharacterSetSchema();
	void setCharacterSetSchema(const StringT &CharacterSetSchema);
	StringT getCharacterSetName();
	void setCharacterSetName(const StringT &CharacterSetName);
	StringT getCollationCatalog();
	void setCollationCatalog(const StringT &CollationCatalog);
	StringT getCollationSchema();
	void setCollationSchema(const StringT &CollationSchema);
	StringT getCollationName();
	void setCollationName(const StringT &CollationName);
	StringT getDomainCatalog();
	void setDomainCatalog(const StringT &DomainCatalog);
	StringT getDomainSchema();
	void setDomainSchema(const StringT &DomainSchema);
	StringT getDomainName();
	void setDomainName(const StringT &DomainName);
	StringT getDescription();
	void setDescription(const StringT &Description);
	StringT getColumnLcid();
	void setColumnLcid(const StringT &ColumnLcid);
	StringT getColumnCompflags();
	void setColumnCompflags(const StringT &ColumnCompflags);
	StringT getColumnSortid();
	void setColumnSortid(const StringT &ColumnSortid);
	StringT getColumnTdscollation();
	void setColumnTdscollation(const StringT &ColumnTdscollation);
	bool getIsComputed();
	void setIsComputed(bool isComputed);

	static StringT getColumnDataTypeName(long iDataType);

private:
	StringT TABLE_CATALOG;
	StringT TABLE_SCHEMA;
	StringT TABLE_NAME;
	StringT COLUMN_NAME;
	StringT COLUMN_GUID;
	StringT COLUMN_PROPID;
	long ORDINAL_POSITION;
	StringT COLUMN_HASDEFAULT;
	StringT COLUMN_DEFAULT;
	StringT COLUMN_FLAGS;
	bool IS_NULLABLE;
	long DATA_TYPE;
	StringT TYPE_GUID;
	long CHARACTER_MAXIMUM_LENGTH;
	long CHARACTER_OCTET_LENGTH;
	long NUMERIC_PRECISION;
	long NUMERIC_SCALE;
	long DATETIME_PRECISION;
	StringT CHARACTER_SET_CATALOG;
	StringT CHARACTER_SET_SCHEMA;
	StringT CHARACTER_SET_NAME;
	StringT COLLATION_CATALOG;
	StringT COLLATION_SCHEMA;
	StringT COLLATION_NAME;
	StringT DOMAIN_CATALOG;
	StringT DOMAIN_SCHEMA;
	StringT DOMAIN_NAME;
	StringT DESCRIPTION;
	StringT COLUMN_LCID;
	StringT COLUMN_COMPFLAGS;
	StringT COLUMN_SORTID;
	StringT COLUMN_TDSCOLLATION;
	bool IS_COMPUTED;
	bool isPrimaryKey;
};

class LIBFF_API TableInfo {
public:
	TableInfo() {
	}

	TableInfo(const StringT &tbName) :
			name(tbName) {
	}

	const StringT& getCatalog() const {
		return catalog;
	}

	void setCatalog(const StringT &catalog) {
		this->catalog = catalog;
	}

	const StringT& getCreatedDate() const {
		return createdDate;
	}

	void setCreatedDate(const StringT &createdDate) {
		this->createdDate = createdDate;
	}

	const StringT& getDescription() const {
		return description;
	}

	void setDescription(const StringT &description) {
		this->description = description;
	}

	const StringT& getGuid() const {
		return guid;
	}

	void setGuid(const StringT &guid) {
		this->guid = guid;
	}

	const StringT& getModifiedDate() const {
		return modifiedDate;
	}

	void setModifiedDate(const StringT &modifiedDate) {
		this->modifiedDate = modifiedDate;
	}

	const StringT& getName() const {
		return name;
	}

	void setName(const StringT &name) {
		this->name = name;
	}

	const StringT& getPropid() const {
		return propid;
	}

	void setPropid(const StringT &propid) {
		this->propid = propid;
	}

	const StringT& getSchema() const {
		return schema;
	}

	void setSchema(const StringT &schema) {
		this->schema = schema;
	}

	const StringT& getType() const {
		return type;
	}

	void setType(const StringT &type) {
		this->type = type;
	}

	std::map<StringT, ColumnInfo>& getColumnInfos() {
		return this->columns;
	}
	void setColumnInfos(const std::map<StringT, ColumnInfo> &columns) {
		this->columns = columns;
	}

	std::set<StringT>& getPrimaryKeys() {
		return this->primaryKeys;
	}
	void setPrimaryKeys(const std::set<StringT> &v) {
		this->primaryKeys = v;
	}

private:
	StringT name;
	StringT catalog;
	StringT schema;
	StringT type;
	StringT guid;
	StringT description;
	StringT propid;
	StringT createdDate;
	StringT modifiedDate;
	std::set<StringT> primaryKeys;
	std::map<StringT, ColumnInfo> columns;
};

class LIBFF_API AdoConnection: public AdoObject {
public:
	AdoConnection(void);
	AdoConnection(const _ConnectionPtr &conn);
	void operator =(const AdoConnection &conn);
	void operator =(const _ConnectionPtr &conn);
	operator bool();
	virtual ~AdoConnection(void);
	virtual void Open();
	virtual void Close();
	virtual bool IsConnected() const;
	operator _ConnectionPtr() const;
	operator _Connection&() const;
	operator _Connection*() const;
	_ConnectionPtr& operator->();

	void setAccessConnStr(const StringT &dbFilePath, const StringT &password =
			(""));
	void setODBCConnStr(const StringT &dataSourceName, const StringT &dbName,
			const StringT &userId = (""), const StringT &password = (""));
	void setSQLServerConnStr(const StringT &serverName, const StringT &dbName,
			const StringT &userId, const StringT &password);
	void setMYSQLConnStr(const StringT &serverName, const StringT &dbName,
			const StringT &userId, const StringT &password,
			const StringT &port = ("3306"),
			const StringT &driverName = ("MySQL ODBC 5.2 ANSI Driver"));
	void setORACLEConnStr(const StringT &serverName, const StringT &serviceName,
			const StringT &userId, const StringT &password,
			const StringT &port = ("1521"));

	void setConnectString(const StringT &connStr);
	StringT getConnectionString() const;
	void setUserId(const StringT &userId);
	StringT getUserId() const;
	void setPassword(const StringT &pass);
	StringT getPassword() const;
	void setOptions(long opt);
	long getOptions() const;
	void setConnectTimeout(long connTimeout);
	long getConnectTimeout() const;
	void setCursorLocation(CursorLocationEnum cl);
	CursorLocationEnum getCursorLocation() const;
	void setConnectMode(ConnectModeEnum mode);
	ConnectModeEnum getConnectMode() const;
	StringT getDatabaseName() const;
	std::map<StringT, TableInfo> getTableInfos();
	TableInfo getTableInfo(const StringT &tableName);
	std::set<StringT> getTableNames();
	std::set<StringT> getPrimaryKeys(const StringT &tableName,
			const StringT &tableSchema = (""), const StringT &tableCatalog =
					(""));
	std::map<StringT, StringT>& getProperties();
	StringT getProperty(const StringT &propName);

	long getState() const;

	void beginTrans();
	void commitTrans();
	void rollbackTrans();

	//Build Data Source Dialog
	static StringT PromptDataSource(HWND hWndParent = NULL);
	static bool PromptDataSource(AdoConnection &conn, HWND hWndParent = NULL);
	static bool CreateAccessFile(const StringT &dbFilePath,
			const StringT &password = (""));

private:
	//bstr_t connStr;
	bstr_t userId;
	bstr_t password;
	std::map<StringT, StringT> properties;
	void getProperties(std::map<StringT, StringT> &properties);
	long options;
	long connTimeout;
	_ConnectionPtr conn;
};

NS_FF_END

#endif /* WINDOWS_ADOCONNECTION_H_ */
