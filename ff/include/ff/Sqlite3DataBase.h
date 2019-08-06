/*
 * Sqlite3DataBase.h
 *
 *  Created on: 2017年9月27日
 *      Author: liyawu
 */

#ifndef FF_SQLITE3dATABASE_H_
#define FF_SQLITE3dATABASE_H_

#include <ff/ff_config.h>

#ifdef __USE_SQLITE3_DB__

#include <sqlite3.h>
#include <string>
#include <list>
#include <set>
#include <ff/Exception.h>
#include <ff/Variant.h>
#include <ff/Buffer.h>

namespace NS_FF{

EXCEPTION_DEF(Sqlite3Exception)

class Statment {
public:
	Statment();
	virtual ~Statment();

	operator sqlite3_stmt*();
	operator sqlite3_stmt**();

	int getDataCount() const;
	int getColumnCount() const;
	int getColumnType(int col) const;
	std::string getColumnName(int col) const;
	std::list<std::string> getColumns() const;
	int indexOfColumn(const std::string& columnName) const;
	Variant getColumnValue(int col) const;
	Variant getColumnValue(const std::string& columnName) const;
	std::string getTableName(int col) const;

	std::string getSql();
	int reset();
	int clearbindings();
	int bindNull(int index);
	int bind(int index, const std::string& value);
	int bind(int index, int value);
	int bind(int index, double value);
	int bind(int index, const void* data, int size);
	int bindNull(const std::string& paramName);
	int bind(const std::string& paramName, const std::string& value);
	int bind(const std::string& paramName, int value);
	int bind(const std::string& paramName, double value);
	int bind(const std::string& paramName, const void* data, int size);
	int bindParamCount() const;
	std::string bindParamName(int index) const;
	int bindParamIndex(const std::string& paramName) const;

	int step();
	bool isReadOnly();
	bool isBusy();

private:
	sqlite3_stmt* stmt;
};

class ColumnInfo{
public:
	ColumnInfo();
	ColumnInfo(const std::string& columnName, const std::string& dataType, bool primaryKey = false,
			bool notNull = false, bool autoInc = false, const std::string& tableName = "",
			const std::string& collate = "");

	bool isAutoInc() const;
	void setAutoInc(bool autoInc);
	const std::string& getCollate() const;
	void setCollate(const std::string& collate);
	const std::string& getColumnName() const;
	void setColumnName(const std::string& columnName);
	const std::string& getDataType() const;
	void setDataType(const std::string& dataType);
	bool isNotNull() const;
	void setNotNull(bool notNull);
	bool isPrimaryKey() const;
	void setPrimaryKey(bool primaryKey);
	const std::string& getTableName() const;
	void setTableName(const std::string& tableName);

	bool operator==(const ColumnInfo& colInfo) const;
	bool operator<(const ColumnInfo& colInfo) const;

private:
	std::string tableName;
	std::string columnName;
	bool notNull;
	bool primaryKey;
	bool autoInc;
	std::string collate;
	std::string dataType;
};

class TableInfo{
public:
	TableInfo();
	TableInfo(const std::string& tableName);

	std::set<ColumnInfo> getColumns() const;
	const std::string& getTableName() const;
	void addColumn(const ColumnInfo& colInfo);
	const ColumnInfo& getColumn(const std::string& columnName) const;
	const ColumnInfo& operator[](const std::string& columnName) const;

	void setTableName(const std::string& tableName);

private:
	std::string tableName;
	std::set<ColumnInfo> columns;
};

class Sqlite3DataBase {
public:
	Sqlite3DataBase();
	virtual ~Sqlite3DataBase();

	void open(const std::string& dbFilePath, bool createIfNotExists = false);
	void close();
	Statment prepare(const std::string& sql);
	std::string errorMsg() const;
	int errorCode() const;
	int beginTransaction();
	int beginExclusiveTransaction();
	int commitTransaction();
	int rollbackTransaction();
	int getChanges();

	std::list<std::string> getTables();
	TableInfo getTableInfo(const std::string& tableName);

	typedef int (*ExecCallback)(void*, int, char**, char**);
	int exec(const std::string& sql, ExecCallback callbackFunc = NULL, void* callbackParam = NULL);

	static std::string GetVersion();
	static int GetVersionNumber();
	static std::string GetSouceId();
	static bool IsValidSqlStatement(const std::string& sql);
private:
	sqlite3* db;
	std::string dbFile;
};

} /* namespace NS_FF */

#endif /* __USE_SQLITE3_DB__ */

#endif /* FF_SQLITE3dATABASE_H_ */
