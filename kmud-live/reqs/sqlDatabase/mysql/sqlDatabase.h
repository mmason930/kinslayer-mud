#ifndef SQL_DATABASE_H
#define SQL_DATABASE_H

/*******************************************************************

File: sqlDatabase.h

Usage:	This file declares the main data structures..

Author: Michael Mason - mikemason930@gmail.com (C) 01-26-2007

*******************************************************************/
#include <string>
#include <map>
#include <list>
#include <mysql/mysql.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <memory>
#include <cstring>

namespace sql
{

class _Connection;
class _Query;
class _Context;
class Row;
typedef std::shared_ptr< _Query > Query;
typedef std::shared_ptr< _Context > Context;
typedef std::shared_ptr< _Connection > Connection;

std::string escapeString( const std::string &str );
std::string escapeQuoteString( const std::string &str );
Context createContext( const std::string &Host, const std::string &User, const std::string &Password, const std::string &DatabaseName );
std::string encodeDate(const time_t unix_timestamp);
std::string encodeQuoteDate(const time_t unix_timestamp);

//Define an Row as an std::map . The index is the field name,
//	the resulting element is the data from the row.
typedef std::map< std::string, int > sqlFieldSet;

struct Exception
{
	virtual const char *what() {
		return getMessage().c_str();
	}
	std::string message;
	Exception(){}
	Exception( std::string _Message ) { message = _Message; }
	std::string getMessage() { return message; }
	virtual void report(){};
};

struct QueryException:Exception
{
	std::string errorMessage;
	int err;
	QueryException( std::string message, MYSQL *mysql=NULL, const char*queryBuffer=NULL ) {
		this->message = message;
		if( mysql != NULL ) {
			err = mysql_errno( mysql );
			errorMessage = mysql_error( mysql );
			this->message += std::string("\n");
			this->message += errorMessage;
			this->message += std::string(". ");
			std::stringstream buff;
			buff << err;
			this->message += (std::string(" (#")+buff.str()+std::string(")")) + std::string("\n");
			if( queryBuffer ) {
				this->message += std::string("Original query: ") + std::string(queryBuffer);
			}
		} else {
			err = (-1);
		}
	}
	virtual void report() { std::cout << "Query exception: " << message << std::endl; }
	std::string getErrorMessage() { return errorMessage; }
	const int getErrno() { return err; }
};

struct FieldException:Exception
{
	FieldException( std::string message ) { this->message = message; }
	virtual void report() { std::cout << "Field exception: " << message << std::endl; }
};
struct ConnectionException:Exception
{
	ConnectionException( std::string message ) { this->message = message; }
	virtual void report() { std::cout << "Connection exception: " << message << std::endl; }
};

class BatchInsertStatement
{
	unsigned int numberOfInserts, insertsPerFlush, numberOfFieldsLoaded;
	std::string tableName;
	std::string header;
	std::stringstream sql;
	bool firstFieldThisEntry;
	bool firstField;
	bool hasStarted;
	Connection connection;

	void init( Connection connection, const std::string &tableName, const unsigned int insertsPerFlush, bool insertIgnore );
public:

	BatchInsertStatement();
	BatchInsertStatement( Connection connection, const std::string &tableName, const unsigned int insertsPerFlush );
	BatchInsertStatement( _Connection *connection, const std::string &tableName, const unsigned int insertsPerFlush );
	BatchInsertStatement( Connection connection, const std::string &tableName, const unsigned int insertsPerFlush, bool insertIgnore );
	BatchInsertStatement( _Connection *connection, const std::string &tableName, const unsigned int insertsPerFlush, bool insertIgnore );

	void flush();
	void finish();

	void addField( const std::string &field );

	void start();

	void beginEntry();
	void endEntry();

	void addFieldValue( const std::string &value );
	void putString( const std::string &value );
	void putString( const char *value );
	void putInt( const int value );
	void putLong( const long long value );
	void putChar( char value );
	void putBool( bool value );
	void putDouble( double value );
};

class _Context
{
	std::string user;
	std::string host;
	std::string password;
	std::string databaseName;
	unsigned int port;
	int flags;
public:
	_Context( const std::string &host, const std::string &user, const std::string &password, const std::string &databaseName );
	_Context( const std::string &host, const std::string &user, const std::string &password, const std::string &databaseName, const int port );
	_Context( const std::string &host, const std::string &user, const std::string &password, const std::string &databaseName, const int port, const int flags );

	Connection createConnection();
};

class _Query
{
private:
	_Connection* server;
	MYSQL_RES* resultSet;	//Current query result

	sqlFieldSet			fields;
	std::list< MYSQL_ROW > rows;
	std::list< MYSQL_ROW >::iterator rowPosition;

	std::string request;
	std::weak_ptr< _Query > sPtr;
	
	MYSQL_RES* getResultSet();

	static int numberOfAllocations;
	static int numberOfDeallocations;

	friend class _Connection;

	void setResultSet( MYSQL_RES* Result );
	void addRow( MYSQL_ROW NewRow );
	void setupFields();
	void clearResultSet();
public:
	_Query();
	_Query( const std::string &request, _Connection* connection );
	~_Query();

	static int getNumberOfAllocations();
	static int getNumberOfDeallocations();
	static int getRemainder();

	unsigned int numRows();
	unsigned int numFields();
	Query send();
	void resetRowQueue();
	void reverseRows();
	void skipRow();
	bool hasNextRow();
	const int getIndexByField( const std::string &Field );
	Row getRow();
	Row peekRow();
	std::string getFieldByIndex( const int index );
	std::string getQueryBuffer() { return request; }
	Query getSharedPtr() { return Query( this ); }
};

class Row
{
private:
	MYSQL_ROW row;
	std::shared_ptr<_Query> query;
	static int numberOfAllocations;
	static int numberOfDeallocations;
public:
	static int getNumberOfAllocations()
	{
		return numberOfAllocations;
	}
	static int getNumberOfDeallocations()
	{
		return numberOfDeallocations;
	}
	static int getRemainder()
	{
		return numberOfAllocations-numberOfDeallocations;
	}
	Row() 
	{
		++numberOfAllocations;
	}
	Row( const Row &original )
	{
		++numberOfAllocations;
		this->row = original.row;
		this->query = original.query;
	}
	Row( std::weak_ptr<_Query> query, MYSQL_ROW row )
	{
		++numberOfAllocations;
		this->query = query.lock();
		this->row = row;
	}
	~Row()
	{
		++numberOfDeallocations;
	}

	bool isFieldNull(int fieldIndex) const
	{
		return row[ fieldIndex ] == NULL;
	}

	bool isFieldNull(const std::string &fieldName) const
	{
		int fieldIndex = query->getIndexByField( fieldName );
		return isFieldNull(fieldIndex);
	}

	std::string operator [] ( const int fieldIndex ) const
	{
		return row[ fieldIndex ];
	}
	std::string operator [] ( const std::string &fieldName ) const
	{
		int index = query->getIndexByField( fieldName );
		return (row[ index ] ? row[ index ] : (""));
	}
	const int getIndexByField( const std::string &fieldName ) const
	{
		return query->getIndexByField( fieldName );
	}
	std::string getFieldByIndex( const int fieldIndex ) const
	{
		if(row[fieldIndex] == NULL)
			return "NULL";
		return query->getFieldByIndex( fieldIndex );
	}

	const int getInt( const std::string &field ) const
	{
		return getInt( getIndexByField(field) );
	}
	const int getInt( const int i ) const
	{
		if(row[i] == NULL)
			return 0;
		return atoi(row[i]);
	}
	const char getChar( const std::string &field ) const
	{
		return getChar(getIndexByField(field));
	}
	const char getChar( const int i ) const
	{
		if(row[i] == NULL)
			return '\0';
		return (char)(row[i][0]);
	}
	const long long getLongLong( const std::string &field ) const
	{
		return getLongLong(getIndexByField(field));
	}
	const long long getLongLong( const int i ) const
	{
		if(row[i] == NULL)
			return 0;
		static std::stringstream buffer;
		static long long retval;
		buffer.str("");
		buffer << row[i];
		buffer >> retval;
		return retval;
	}
	const std::string getString( const std::string &Field ) const
	{
		return getString( getIndexByField(Field) );
	}
	const std::string getString( const int i ) const
	{
		return row[i];
	}
	const float getFloat( const std::string &Field ) const
	{
		return getFloat( getIndexByField(Field) );
	}
	const float getFloat( const int i ) const
	{
		if(row[i] == NULL)
			return 0;
		static float f;
		sscanf(row[i], "%f", &f);
		return f;
	}
	const double getDouble( const std::string &Field ) const
	{
		return getDouble( getIndexByField(Field) );
	}
	const double getDouble( const int i ) const
	{
		if(row[i] == NULL)
			return 0;
		return atof(row[i]);
	}
	const time_t getTimestamp( const int i ) const
	{
		tm t;
		memset(&t, 0, sizeof(tm));
		t.tm_isdst = -1;
		if(row[i] == NULL || sscanf(row[i], "%d-%d-%d %d:%d:%d",
		&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec) != 6)
		{
			return 0;
		}
		t.tm_mon -= 1;
		t.tm_year -= 1900;

		return mktime(&t);
	}
	const time_t getTimestamp( const std::string &field ) const
	{
		return getTimestamp( getIndexByField(field) );
	}
};

class _Connection
{
private:
	std::string databaseName;	//Name of the database
	MYSQL* server;		//The SQL server

public:
	_Connection( const std::string &host, const std::string &user, const std::string &password, const std::string &name );
	_Connection();
	~_Connection();

	void connect( const std::string &host, const std::string &user, const std::string &password, const std::string &name );

	bool isConnected();
	void reportError();
	void reportError( const std::string &logMessage );
	void sendQuery( Query query );
	void sendRawQuery( const std::string &query );
	my_ulonglong lastInsertID();

	std::list< std::string > getTableList();

	Query sendQuery( const std::string &queryBuffer );
};

}

#endif
