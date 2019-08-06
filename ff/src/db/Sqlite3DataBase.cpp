/*
 * Sqlite3DataBase.cpp
 *
 *  Created on: 2017年9月27日
 *      Author: liyawu
 */

#include <ff/Sqlite3DataBase.h>

#ifdef __USE_SQLITE3_DB__

#include <ff/String.h>

using namespace std;

namespace NS_FF{

/**
 * class Statment
 */
Statment::Statment() :
		stmt(NULL) {
}

Statment::~Statment() {
	if (this->stmt) {
		sqlite3_finalize(this->stmt);
	}
}

Statment::operator sqlite3_stmt*() {
	return this->stmt;
}

Statment::operator sqlite3_stmt**() {
	return &this->stmt;
}

int Statment::getDataCount() const {
	return sqlite3_column_count(this->stmt);
}

int Statment::getColumnCount() const {
	return sqlite3_data_count(this->stmt);
}

int Statment::getColumnType(int col) const {
	return sqlite3_column_type(this->stmt, col);
}

int Statment::indexOfColumn(const string& columnName) const{
	int cols = this->getColumnCount();
	for(int i = 0; i < cols; ++i){
		if(columnName == this->getColumnName(i)){
			return i;
		}
	}

	return -1;
}

string Statment::getColumnName(int col) const{
#ifdef SQLITE_ENABLE_COLUMN_METADATA
	return sqlite3_column_table_name(this->stmt, col);
#else
	return sqlite3_column_name(this->stmt, col);
#endif
}

list<string> Statment::getColumns() const{
	list<string> cols;
	int colCount = this->getColumnCount();
	for(int i = 0; i < colCount; ++i){
		cols.push_back(this->getColumnName(i));
	}
	return cols;
}

string Statment::getTableName(int col) const{
#ifdef SQLITE_ENABLE_COLUMN_METADATA
	return sqlite3_column_table_name(this->stmt, col);
#else
	return "";
#endif
}

int Statment::bindParamCount() const{
	return sqlite3_bind_parameter_count(this->stmt);
}

int Statment::bindParamIndex(const string& paramName) const{
	return sqlite3_bind_parameter_index(this->stmt, paramName.c_str());
}

Variant Statment::getColumnValue(int col) const {
	int type = this->getColumnType(col);
	switch (type) {
	case SQLITE_INTEGER:
	case SQLITE_FLOAT:
	case SQLITE3_TEXT: {
		Variant v = (const char*)sqlite3_column_text(this->stmt, col);
		return v;
	}
	case SQLITE_BLOB: {
		int size = sqlite3_column_bytes(this->stmt, col);
		const void* data = sqlite3_column_blob(this->stmt, col);
		Buffer buf((const char*) data, size);
		return buf;
	}
	case SQLITE_NULL: {
		return Variant();
	}
	}

	return Variant();
}

Variant Statment::getColumnValue(const string& columnName) const{
	return this->getColumnValue(this->indexOfColumn(columnName));
}

string Statment::getSql(){
	const char* sql = sqlite3_sql(this->stmt);
	if(NULL == sql){
		return "";
	}else{
		return sql;
	}
}

int Statment::reset() {
	return sqlite3_reset(this->stmt);
}

int Statment::clearbindings() {
	return sqlite3_clear_bindings(this->stmt);
}

int Statment::bindNull(int index) {
	return sqlite3_bind_null(this->stmt, index);
}

int Statment::bind(int index, const void* data, int size) {
	return sqlite3_bind_blob(this->stmt, index, data, size, 0);
}

int Statment::bind(int index, const string& value) {
	return sqlite3_bind_text(this->stmt, index, value.c_str(), value.length(), 0);
}

int Statment::bind(int index, int value) {
	return sqlite3_bind_int(this->stmt, index, value);
}

int Statment::bind(int index, double value) {
	return sqlite3_bind_double(this->stmt, index, value);
}

int Statment::bindNull(const string& paramName) {
	return sqlite3_bind_null(this->stmt, this->bindParamIndex(paramName));
}

int Statment::bind(const string& paramName, const void* data, int size) {
	return sqlite3_bind_blob(this->stmt, this->bindParamIndex(paramName), data, size, 0);
}

int Statment::bind(const string& paramName, const string& value) {
	return sqlite3_bind_text(this->stmt, this->bindParamIndex(paramName), value.c_str(), value.length(), 0);
}

int Statment::bind(const string& paramName, int value) {
	return sqlite3_bind_int(this->stmt, this->bindParamIndex(paramName), value);
}

int Statment::bind(const string& paramName, double value) {
	return sqlite3_bind_double(this->stmt, this->bindParamIndex(paramName), value);
}

string Statment::bindParamName(int index) const{
	return sqlite3_bind_parameter_name(this->stmt, index);
}

int Statment::step() {
	return sqlite3_step(this->stmt);
}

bool Statment::isReadOnly(){
	return sqlite3_stmt_readonly(this->stmt);
}

bool Statment::isBusy(){
	return sqlite3_stmt_busy(this->stmt);
}


/**
 * class ColumnInfo
 */
ColumnInfo::ColumnInfo() : notNull(false), primaryKey(false), autoInc(false){

}

ColumnInfo::ColumnInfo(const string& columnName, const string& dataType, bool primaryKey,
		bool notNull, bool autoInc, const string& tableName, const string& collate) {
	this->columnName = columnName;
	this->tableName = tableName;
	this->collate = collate;
	this->dataType = dataType;
	this->primaryKey = primaryKey;
	this->notNull = notNull;
	this->autoInc = autoInc;
}

bool ColumnInfo::isAutoInc() const {
	return autoInc;
}

void ColumnInfo::setAutoInc(bool autoInc) {
	this->autoInc = autoInc;
}

const string& ColumnInfo::getCollate() const {
	return collate;
}

void ColumnInfo::setCollate(const string& collate) {
	this->collate = collate;
}

const string& ColumnInfo::getColumnName() const {
	return columnName;
}

void ColumnInfo::setColumnName(const string& columnName) {
	this->columnName = columnName;
}

const string& ColumnInfo::getDataType() const {
	return dataType;
}

void ColumnInfo::setDataType(const string& dataType) {
	this->dataType = dataType;
}

bool ColumnInfo::isNotNull() const {
	return notNull;
}

void ColumnInfo::setNotNull(bool notNull) {
	this->notNull = notNull;
}

bool ColumnInfo::isPrimaryKey() const {
	return primaryKey;
}

void ColumnInfo::setPrimaryKey(bool primaryKey) {
	this->primaryKey = primaryKey;
}

const string& ColumnInfo::getTableName() const {
	return tableName;
}

void ColumnInfo::setTableName(const string& tableName) {
	this->tableName = tableName;
}

bool ColumnInfo::operator==(const ColumnInfo& colInfo) const{
	string colName = this->getColumnName();
	string colName2 = colInfo.getColumnName();
	Trim(colName);
	ToLower(colName);
	Trim(colName2);
	ToLower(colName2);
	return (colName == colName2);
}

bool ColumnInfo::operator<(const ColumnInfo& colInfo) const{
	string colName = this->getColumnName();
	string colName2 = colInfo.getColumnName();
	Trim(colName);
	ToLower(colName);
	Trim(colName2);
	ToLower(colName2);
	return (colName < colName2);
}

/**
 * class TableInfo
 */
TableInfo::TableInfo(){

}

TableInfo::TableInfo(const string& tableName){
	this->tableName = tableName;
}

const string& TableInfo::getTableName() const {
	return tableName;
}

void TableInfo::setTableName(const string& tableName) {
	this->tableName = tableName;
}

set<ColumnInfo> TableInfo::getColumns() const {
	return columns;
}

const ColumnInfo& TableInfo::getColumn(const string& columnName) const{
	string colName = ToLowerCopy(TrimCopy(columnName));
	for(set<ColumnInfo>::iterator it = this->columns.begin(); it != this->columns.end(); ++it){
		if(colName == ToLowerCopy(TrimCopy(it->getColumnName()))){
			return *it;
		}
	}
	THROW_EXCEPTION(Sqlite3Exception, "column[" + columnName + "] not found", 0);
}

const ColumnInfo& TableInfo::operator[](const string& columnName) const{
	return this->getColumn(columnName);
}

void TableInfo::addColumn(const ColumnInfo& colInfo){
	ColumnInfo col = colInfo;
	if(this->columns.count(col) > 0){
		THROW_EXCEPTION(Sqlite3Exception, "column[" + col.getColumnName() + "] has exists", 0);
	}
	col.setTableName(this->tableName);
	this->columns.insert(col);
}

/**
 * class Sqlite3DataBase
 */
Sqlite3DataBase::Sqlite3DataBase() :
		db(NULL) {
	sqlite3_initialize();
}

Sqlite3DataBase::~Sqlite3DataBase() {
	try{
		this->close();
	}catch(...){}
}

void Sqlite3DataBase::open(const string& dbFilePath, bool createIfNotExists) {
	this->dbFile = dbFilePath;
	int flags = SQLITE_OPEN_READWRITE;
	if(createIfNotExists){
		flags |= SQLITE_OPEN_CREATE;
	}
	int re = sqlite3_open_v2(this->dbFile.c_str(), &db, flags, 0);
	if (SQLITE_OK != re) {
		this->db = NULL;
		THROW_EXCEPTION(Sqlite3Exception, "DB \"" + this->dbFile + "\" open failed. " + this->errorMsg(), re);
	}
}

void Sqlite3DataBase::close() {
	if (this->db) {
		int re = sqlite3_close_v2(db);
		if (SQLITE_OK != re) {
			THROW_EXCEPTION(Sqlite3Exception, "DB \"" + this->dbFile + "\" close failed. " + this->errorMsg(), re);
		}
		this->db = NULL;
	}
}

Statment Sqlite3DataBase::prepare(const string& sql) {
	Statment stmt;
	int re = sqlite3_prepare_v2(db, sql.c_str(), -1, stmt, 0);
	if (SQLITE_OK != re) {
		THROW_EXCEPTION(Sqlite3Exception, "prepare SQL[" + sql + "] failed. " + this->errorMsg(), re);
	}

	return stmt;
}

string Sqlite3DataBase::errorMsg() const {
	return sqlite3_errmsg(this->db);
}

int Sqlite3DataBase::errorCode() const{
	return sqlite3_errcode(this->db);
}

int Sqlite3DataBase::beginTransaction(){
	return this->exec( "begin immediate transaction");
}

int Sqlite3DataBase::beginExclusiveTransaction(){
	return this->exec( "begin exclusive transaction");
}

int Sqlite3DataBase::commitTransaction(){
	return this->exec( "commit transaction");
}

int Sqlite3DataBase::rollbackTransaction(){
	return this->exec( "rollback transaction");
}

int Sqlite3DataBase::getChanges(){
	return sqlite3_changes(this->db);
}

int Sqlite3DataBase::exec(const string& sql, ExecCallback callbackFunc, void* callbackParam) {
	int re = sqlite3_exec(db, sql.c_str(), callbackFunc, callbackParam, 0);
	if (SQLITE_OK != re) {
		THROW_EXCEPTION(Sqlite3Exception, "exec SQL[" + sql + "] failed. " + this->errorMsg(), re);
	}
	return re;
}

bool Sqlite3DataBase::IsValidSqlStatement(const string& sql){
	return sqlite3_complete(sql.c_str());
}

string Sqlite3DataBase::GetVersion(){
	return sqlite3_libversion();
}

int Sqlite3DataBase::GetVersionNumber(){
	return sqlite3_libversion_number();
}

string Sqlite3DataBase::GetSouceId(){
	return sqlite3_sourceid();
}

list<string> Sqlite3DataBase::getTables(){
	list<string> tables;
	Statment stmt = this->prepare("SELECT * FROM sqlite_master WHERE type='table' order by name");
	while(SQLITE_DONE != stmt.step()){
		tables.push_back(stmt.getColumnValue("name"));
	}
	return tables;
}

TableInfo Sqlite3DataBase::getTableInfo(const string& tableName){
	TableInfo tbInfo;
	Statment stmt = this->prepare("select * from [" + tableName + "]");
	tbInfo.setTableName(tableName);
	int colCount = stmt.getDataCount();
	for(int col = 0; col < colCount; ++col){
		string colName = stmt.getColumnName(col);
		const char* pDataType  = NULL;
		const char* pCollSeq  = NULL;
		int notNull = 0;
		int primaryKey = 0;
		int autoInc = 0;
		int re = sqlite3_table_column_metadata(this->db, NULL,
				tableName.c_str(), colName.c_str(),
				&pDataType,
				&pCollSeq,
				&notNull,
				&primaryKey,
				&autoInc);
		if(SQLITE_OK != re){
			THROW_EXCEPTION(Sqlite3Exception, "Get column metadata failed. " + this->errorMsg(), re);
		}

		tbInfo.addColumn(
				ColumnInfo(colName, pDataType, primaryKey, notNull, autoInc, tableName, pCollSeq));
	}
	return tbInfo;
}

} /* namespace NS_FF */

#endif /* __USE_SQLITE3_DB__ */
