/*******************************************************************

File: sqlDatabase.cpp

Usage:	Provides implementation for all SQL data strucures.

Author: Michael Mason - mikemason930@gmail.com (C) 01-26-2007

*******************************************************************/

#include "sqlDatabase.h"

void insertLetterBeforeLetter(std::string &str, const char letter_to_find, const char letter_to_lead);

namespace sql
{

int _Query::numberOfAllocations=0;
int _Query::numberOfDeallocations=0;
int Row::numberOfAllocations=0;
int Row::numberOfDeallocations=0;

Context createContext( const std::string &host, const std::string &user, const std::string &password, const std::string &databaseName )
{
	return Context( new _Context(host, user, password, databaseName) );
}

_Context::_Context( const std::string &host, const std::string &user, const std::string &password, const std::string &databaseName )
{
	this->host = host;
	this->user = user;
	this->password = password;
	this->databaseName = databaseName;
	this->port = 0;
	this->flags = 0;
}
_Context::_Context( const std::string &host, const std::string &user, const std::string &password, const std::string &databaseName, const int port )
{
	this->host = host;
	this->user = user;
	this->password = password;
	this->databaseName = databaseName;
	this->port = port;
	this->flags = flags;
}
_Context::_Context( const std::string &Host, const std::string &user, const std::string &password, const std::string &databaseName, const int port, const int flags )
{
	this->host = host;
	this->user = user;
	this->password = password;
	this->databaseName = databaseName;
	this->port = port;
	this->flags = flags;
}
Connection _Context::createConnection()
{
	return Connection( new _Connection(host, user, password, databaseName) );
}

_Connection::_Connection()
{
	server = mysql_init( 0 );
}

_Connection::_Connection( const std::string &host, const std::string &user, const std::string &password, const std::string &name )
{
	server = mysql_init( 0 );

	connect( host, user, password, name );
}
_Connection::~_Connection()
{
	if ( server ) mysql_close( server );
}

void _Connection::connect( const std::string &host, const std::string &user, const std::string &password, const std::string &name )
{
	if(!mysql_real_connect(server, host.c_str(), user.c_str(), password.c_str(), name.c_str(),
	0, 0, CLIENT_MULTI_STATEMENTS))
	{
		std::stringstream errorMessage;
		errorMessage << "Failed to connect to database '" << name << "' as '" << user << "' @" << host;
		throw ConnectionException( errorMessage.str() );
	}
}

void _Connection::reportError()
{
	std::cout << "SQL Error : " << mysql_error( server ) << std::endl;
}

void _Connection::reportError( const std::string &logMessage )
{
	std::cout << "SQL Error( " << mysql_error( server ) << " ) : " << logMessage << std::endl;
}

bool _Connection::isConnected()
{
	return (server && mysql_ping(server) == 0);
}

boost::shared_ptr<_Query> _Connection::sendQuery( const std::string &queryBuffer )
{
	_Query *query = new _Query( queryBuffer, this );
	Query wrappedQuery;
	try {
		wrappedQuery = query->send();
	} catch( QueryException e ) {
		throw;
	}

	return wrappedQuery;
}
void _Connection::sendQuery( Query query )
{
	int retval;
	//Nonzero return value means there was an error.
	if( (retval = mysql_query( server, query->getQueryBuffer().c_str() )) != 0 )
	{
		std::stringstream errorMessage;
		errorMessage << "Failed to send query. Errno: " << retval;
		throw QueryException(errorMessage.str(),this->server, query->getQueryBuffer().c_str());
	}
	//There is no error reporting, because many queries do not store a result.
	query->setResultSet( mysql_store_result( server ) );
}

//This method will simply send the query without storing a result, or an Query object.
void _Connection::sendRawQuery( const std::string &query )
{
	int retval;
	//Nonzero return value means there was an error.
	if( (retval = mysql_query( server, query.c_str() )) != 0 )
	{
		std::stringstream errorMessage;
		errorMessage << "Failed to send query. Errno: " << retval;
		throw QueryException(errorMessage.str(), this->server, query.c_str());
	}
}
my_ulonglong _Connection::lastInsertID()
{
	return mysql_insert_id( server );
}

//Grab a list of tables in the current database.
std::list< std::string > _Connection::getTableList()
{		
	std::list< std::string > Tables;

	Query query = sendQuery("SHOW TABLES");
	if ( !(query) )
		return (Tables);

	for(unsigned int i = 0;i < query->numRows();++i)
	{
		Tables.push_back( query->getRow()[ std::string("Tables_in_" + this->databaseName).c_str() ] );
	}
	return (Tables);
}

_Query::_Query()
{
	++numberOfAllocations;
	resultSet = 0;
	server = 0;
}

//Construct and send query automatically.
_Query::_Query(const std::string &request, _Connection* connection)
{ 
	++numberOfAllocations;
	this->request = request;
	this->server = connection;
	this->resultSet = 0;//No result yet.
}

_Query::~_Query()
{
	++numberOfDeallocations;
	clearResultSet();//Just clear the result if one exists.
}
int _Query::getNumberOfAllocations()
{
	return numberOfAllocations;
}
int _Query::getNumberOfDeallocations()
{
	return numberOfDeallocations;
}
int _Query::getRemainder()
{
	return numberOfAllocations-numberOfDeallocations;
}

/************* Query Member Function Implementations ************/

//Set the API sql result
void _Query::setResultSet( MYSQL_RES* resultSet )
{
	clearResultSet();
	this->resultSet = resultSet;
}
//Return the API sql result
MYSQL_RES* _Query::getResultSet()
{
	return resultSet;
}
//Add a row to the 'queue'.
void _Query::addRow( MYSQL_ROW newRow )
{
	rows.push_back( newRow );//Add to the end of the 'queue'
}
//Reset the queue iterator to the beginning of the list.
void _Query::resetRowQueue()
{
	rowPosition = rows.begin();
}
//Reverse the rows in the queue.
void _Query::reverseRows()
{
	rows.reverse();
	resetRowQueue();
}
//Is there another row in the 'queue'?
bool _Query::hasNextRow()
{
	return ( rowPosition != rows.end() );
}
//Grab all fields from the sql query result.
void _Query::setupFields()
{
	if( fields.size() )
		return;//Fields are already set up.

	MYSQL_FIELD* mysqlFields = mysql_fetch_fields( getResultSet() ); //This is deleted in mysql_free_result()
	unsigned int nr_fields = mysql_num_fields( getResultSet() );

	if( !mysqlFields )
	{
		throw FieldException("No fields found");
	}
	for(unsigned int i = 0;i < nr_fields;++i)
	{
		if(mysqlFields[i].name)
			fields[ mysqlFields[i].name ]		=	i;
		else if(mysqlFields[i].org_name)
			fields[ mysqlFields[i].org_name ]	=	i;
	}

}
//Send the query to the database server.
Query _Query::send()
{
	Query query( this );
	this->sPtr = boost::weak_ptr< _Query >( query );

	if(this->server) server->sendQuery( query );

	//The following only occurs if there was a result from the query.
	//Non-resulting queries have no data to store.
	if ( getResultSet() )
	{
		unsigned int nr_rows = (unsigned int)mysql_num_rows( getResultSet() );
		setupFields();

		for(unsigned int i = 0;i < nr_rows;++i)
		{
			addRow( mysql_fetch_row( getResultSet() ) );
		}
	}
	rowPosition = rows.begin();
	return query;
}
//Clear out the result data & all corresponding data.
void _Query::clearResultSet()
{
	if(this->resultSet)//If no result, then there is nothing being stored!
	{
		mysql_free_result( resultSet );//Free API sql result
		resultSet = 0;	

		rows.clear();//Clear rows
		fields.clear();//Clear field names & indexes
	}
}

//Number of data entries(rows) returned by the sql query.
unsigned int _Query::numRows()
{
	return (unsigned int)rows.size();
}
//Number of fields returned by the sql query
unsigned int _Query::numFields()
{
	return (unsigned int)fields.size();
}
//Grab the numerical index for a field.
const int _Query::getIndexByField( const std::string &field )
{
	return fields[ field ];
}
std::string _Query::getFieldByIndex( const int index )
{
	for(sqlFieldSet::iterator fieldIterator = fields.begin();fieldIterator != fields.end();++fieldIterator)
	{
		if( (*fieldIterator).second == index )
			return (*fieldIterator).first;
	}
	return std::string("");
}
//A bit redundant, but more efficient than grabbing a row iterate when un-needed.
void _Query::skipRow()
{
	if( rowPosition != rows.end() ) ++rowPosition;//Iterate only if not at the end of the list.
}
//Grab the next row in the 'queue' and move on to the next.
Row _Query::getRow()
{
	Row Data;//Data stored & returned from here.

	if( !server )
		throw QueryException("There is no MySQL server connection for this query object.");
	if( !resultSet )
		throw QueryException("There is no MySQL query result stored.");
	if( rowPosition == rows.end() )//We're at the end of the queue. Must be reset.
		throw QueryException("The cursor is at the end of the row queue.");

	Data = Row(this->sPtr, *(rowPosition++));//Grab this row & iterate
	return Data;
}
//Grab the next row in the 'queue' without iterating.
Row _Query::peekRow()
{
	Row row;//Data stored & returned from here.

	if( !server )
		throw QueryException("There is no MySQL server connection for this query object.");
	if( !resultSet )
		throw QueryException("There is no MySQL query result stored.");
	if( rowPosition == rows.end() )//We're at the end of the queue. Must be reset.
		throw QueryException("The cursor is at the end of the row queue.");

	row = Row( this->sPtr, *(rowPosition) );//Grab this row
	return row;
}
void BatchInsertStatement::init( Connection connection, const std::string &tableName, const unsigned int insertsPerFlush, bool insertIgnore )
{
	this->connection = connection;
	this->tableName = tableName;
	this->numberOfInserts = 0;
	this->numberOfFieldsLoaded = 0;
	this->insertsPerFlush = insertsPerFlush;
	this->firstFieldThisEntry = false;
	this->hasStarted = false;

	sql << "INSERT" << (insertIgnore ? " IGNORE" : "") << " INTO ";
	sql << tableName;
	sql << "(";

	this->firstField = true;
}
BatchInsertStatement::BatchInsertStatement()
{
	init( Connection(reinterpret_cast<_Connection*>(NULL)), "", 1000, false );
}
BatchInsertStatement::BatchInsertStatement( _Connection *connection, const std::string &tableName, const unsigned int insertsPerFlush )
{
	init( Connection(connection), tableName, insertsPerFlush, false );
}
BatchInsertStatement::BatchInsertStatement( Connection connection, const std::string &tableName, const unsigned int insertsPerFlush )
{
	init( connection, tableName, insertsPerFlush, false );
}

BatchInsertStatement::BatchInsertStatement( Connection connection, const std::string &tableName, const unsigned int insertsPerFlush, bool insertIgnore )
{
	init( connection, tableName, insertsPerFlush, insertIgnore );
}

BatchInsertStatement::BatchInsertStatement( _Connection *connection, const std::string &tableName, const unsigned int insertsPerFlush, bool insertIgnore )
{
	init( Connection(connection), tableName, insertsPerFlush, insertIgnore );
}

void BatchInsertStatement::start()
{
	sql << ")VALUES";
	header = sql.str();
	hasStarted = true;
}
void BatchInsertStatement::flush()
{
	if( numberOfInserts > 0 ) {
		connection->sendRawQuery( sql.str() );
	}
	sql.str("");
	sql << header;

	numberOfInserts = 0;
	firstField = false;
}
void BatchInsertStatement::finish()
{
	this->flush();

	this->hasStarted = false;
	this->sql.str("");
	this->header.clear();
	this->tableName.clear();
	this->numberOfInserts = 0;
	this->numberOfFieldsLoaded = 0;
	this->insertsPerFlush = 0;
	this->firstFieldThisEntry = false;
	this->hasStarted = false;
}
void BatchInsertStatement::addField( const std::string &field )
{
	if( hasStarted ) {
		throw QueryException("Attempting to add field to a batch insert statement that has already started.");
	}
	if( !firstField )
		sql << ",";
	else
		firstField = false;
	sql << field;
}
void BatchInsertStatement::beginEntry()
{
	this->firstFieldThisEntry = true;

	if( numberOfInserts > 0 )
		sql << ",";
	sql << "(";
}
void BatchInsertStatement::endEntry()
{
	++numberOfInserts;
	sql << ")";

	if( numberOfInserts >= insertsPerFlush )
		flush();
}
void BatchInsertStatement::addFieldValue( const std::string &value )
{
	if( !firstFieldThisEntry )
		sql << ",";
	else
		firstFieldThisEntry = false;
	sql << value;
	++numberOfFieldsLoaded;
}
void BatchInsertStatement::putString( const char *value )
{
	if(value) {
		putString(std::string(value));
	}
	else {
		putString(std::string(""));
	}
}
void BatchInsertStatement::putString( const std::string &value )
{
	addFieldValue(escapeQuoteString(value));
}
void BatchInsertStatement::putInt( const int value )
{
	std::stringstream buf;
	buf << value;
	addFieldValue( buf.str() );
}
void BatchInsertStatement::putLong( const long long value )
{
	std::stringstream buf;
	buf << value;
	addFieldValue( buf.str() );
}
void BatchInsertStatement::putChar( char value )
{
	static char buf[2];
	sprintf(buf, "%c", value);
	putString(&value);
}
void BatchInsertStatement::putBool( bool value )
{
	addFieldValue( value ? "1" : "0" );
}
void BatchInsertStatement::putDouble( double value )
{
	std::stringstream buf;
	buf << value;
	addFieldValue( buf.str() );
}

std::string escapeString(const std::string &str)
{
	std::string escapedString = str;
	::insertLetterBeforeLetter(escapedString, '\\', '\\');
	::insertLetterBeforeLetter(escapedString, '\'', '\\');
	return escapedString;
}

std::string escapeQuoteString(const std::string &str)
{
	return "'" + escapeString(str) + "'";
}

std::string encodeQuoteDate(const time_t unix_timestamp)
{
	return std::string("'") + encodeDate(unix_timestamp) + std::string("'");
}

std::string encodeDate(const time_t unix_timestamp)
{
	struct tm *timeInfo;
	static char buffer[32];
	timeInfo = localtime(&unix_timestamp);
	strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", timeInfo);

	return std::string(buffer);
}

}//Namespace

void insertLetterBeforeLetter(std::string &str, const char letter_to_find, const char letter_to_lead)
{
	std::string::size_type pos;
	for(pos = str.find(letter_to_find, 0);pos != std::string::npos;
		pos = str.find(letter_to_find, pos + 1))
	{
		str.insert(pos, 1, letter_to_lead);
		++pos;
	}
}
