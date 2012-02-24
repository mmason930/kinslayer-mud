#include <mysql/sqlDatabase.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#define LOCAL_DATABASE_USER		"root"
#define LOCAL_DATABASE_PASS		""
#define LOCAL_DATABASE_DBNAME	"livemud"
#define LOCAL_DATABASE_HOST		"localhost"

#define REMOTE_DATABASE_USER	"livemud"
#define REMOTE_DATABASE_DBNAME	"livemud"
#define REMOTE_DATABASE_HOST	"kinslayermud.org"

sql::Context dbRemoteContext;
sql::Context dbLocalContext;
sql::Connection localConnection;
sql::Connection remoteConnection;

#define FETCH_SIZE	1000

int getTableSize( const std::string &tableName )
{
	std::string query;
	query = "SELECT COUNT(*) AS size FROM " + tableName;
	return atoi(remoteConnection->SendQuery( query )->GetRow()["size"].c_str());

}

void createTempTable( const std::string &originalTableName )
{
	std::string tableName = std::string("temp") + originalTableName;
	std::string schema;
	std::string::size_type pos;
	std::stringstream queryBuffer;
	sql::Query query;
	sql::Row row;

	//Get the table schema from the MUD server.
	queryBuffer << "SHOW CREATE TABLE " << originalTableName;
	query = remoteConnection->SendQuery(queryBuffer.str());
	row = query->GetRow();

	schema = row[1];
	//Swap the schema table name with the temp table name.
	pos = schema.find( originalTableName );
	schema.replace( schema.begin()+pos, schema.begin()+pos+originalTableName.size(), sql::EscStr(tableName) );

	//The query is now ready to go.
	localConnection->SendRawQuery( schema );
}
void dropTempTable( const std::string &originalTableName )
{
	std::string tableName = std::string("temp") + originalTableName;
	std::stringstream queryBuffer;

	queryBuffer << "DROP TABLE IF EXISTS " << tableName;
	localConnection->SendRawQuery(queryBuffer.str());
}
void swapTables( const std::string &originalTableName )
{
	std::string tempTableName = std::string("temp") + originalTableName;
	std::string swapTableName = std::string("swap") + originalTableName;
	std::stringstream queryBuffer;

	queryBuffer << "DROP TABLE IF EXISTS " << swapTableName;
	localConnection->SendRawQuery(queryBuffer.str());

	try {
		queryBuffer.str("");
		queryBuffer << "RENAME TABLE " << originalTableName << " TO " << swapTableName;
		localConnection->SendRawQuery(queryBuffer.str());
	} catch( sql::QueryException e ) {
		//The original table does not exist. Not a problem.
	}

	queryBuffer.str("");
	queryBuffer << "RENAME TABLE " << tempTableName << " TO " << originalTableName;
	localConnection->SendRawQuery(queryBuffer.str());

	try {
		//Swap table, previously original table, does not exist. Again, not a problem.
		queryBuffer.str("");
		queryBuffer << "RENAME TABLE " << swapTableName << " TO " << tempTableName;
		localConnection->SendRawQuery(queryBuffer.str());
	} catch( sql::QueryException e ) {
		//...
	}

}
void syncTable( const std::string &tableName, int batchSize )
{
	sql::Query query;
	std::stringstream queryBuffer;
	std::string tempTableName = std::string("temp") + tableName;
	std::list< std::string > fieldNames;
	sql::Row row;

	dropTempTable( tableName );

	createTempTable( tableName );

	int tableSize = getTableSize( tableName );

	int offset = 0;

	sql::BatchInsertStatement batchInsertStatement( localConnection, tempTableName, batchSize );

	queryBuffer << "SHOW COLUMNS FROM " << tempTableName;

	query = localConnection->SendQuery( queryBuffer.str() );

	while( query->HasNextRow() ) {
		fieldNames.push_back( query->GetRow()["Field"] );
		batchInsertStatement.AddField( fieldNames.back() );
	}

	batchInsertStatement.Start();

	std::cout << "...Synchronizing " << tableName << "..." << std::endl;
	while( offset < tableSize )
	{
		if( offset )
			std::cout << "Offset: " << offset << std::endl;
		queryBuffer.str("");//Clear.
		queryBuffer << "SELECT * FROM " << tableName << " LIMIT " << offset << "," << batchSize;
		query = remoteConnection->SendQuery( queryBuffer.str() );

		while( query->HasNextRow() )
		{
			row = query->GetRow();
			batchInsertStatement.BeginEntry();
			for(std::list<std::string>::iterator fieldNameIter = fieldNames.begin();fieldNameIter != fieldNames.end();++fieldNameIter)
			{
				std::string fieldName = (*fieldNameIter);
				batchInsertStatement.PutString( sql::EscStr(row[ fieldName ]) );
			}
			batchInsertStatement.EndEntry();
		}

		batchInsertStatement.Flush();

		offset += batchSize;
	}

	batchInsertStatement.Finish();

	swapTables( tableName );

	dropTempTable( tableName );
}
void syncTable( const std::string &tableName )
{
	syncTable( tableName, FETCH_SIZE );
}

int main( void )
{
	std::string remotePassword;

//	std::cout << "Enter the MUD's database password: ";
//	std::getline(std::cin, remotePassword);

	std::ifstream InFile( "pw.txt" );
	if( !InFile.is_open() ) {

		std::cout << "Create a file named 'pw.txt' and put the remote database password inside. Be sure to add this to the svn ignore list." << std::endl;
		return 0;
	}
	std::getline(InFile, remotePassword);
	InFile.close();

	dbRemoteContext = sql::CreateContext(REMOTE_DATABASE_HOST,
													REMOTE_DATABASE_USER,
													remotePassword,
//													REMOTE_DATABASE_PASS,
													REMOTE_DATABASE_DBNAME);
	dbLocalContext = sql::CreateContext( LOCAL_DATABASE_HOST,
													LOCAL_DATABASE_USER,
													LOCAL_DATABASE_PASS,
													LOCAL_DATABASE_DBNAME);

	try {
		localConnection = dbLocalContext->CreateConnection();
		remoteConnection = dbRemoteContext->CreateConnection();

		syncTable( "users" );
		syncTable( "userArchive" );
		syncTable( "userDeletion" );
		syncTable( "userRestore" );
		syncTable( "userLogout" );
		syncTable( "userLogin" );
		syncTable( "js_scripts" );
		syncTable( "js_attachments" );
		syncTable( "rooms" );
		syncTable( "room_exits" );
		syncTable( "zone_index" );
		syncTable( "zone_commands" );

	} catch( sql::QueryException e ) {
		e.Report();
		exit(1);
	} catch( sql::ConnectionException e ) {
		e.Report();
		exit(1);
	}

	std::cout << "Synchronization complete." << std::endl;

	return 0;
}
