/* ************************************************************************
*   File: ban.cpp                                       Part of CircleMUD *
*  Usage: banning/unbanning/checking sites and player names               *
*                                                                         *
*                                                                         *
*  Completely rewritten in April 2009 by Galnor                           *
*                                                                         *
*                                                                         *
************************************************************************ */

#include "conf.h"


#include "utils.h"
#include "interpreter.h"
#include "db.h"

extern Descriptor *descriptor_list;

BanManager *BanManager::Self = NULL;//Initialize singleton's copy of self.

BanManager &BanManager::GetManager()
{
	if( !Self ) Self = new BanManager();
	return (*Self);
}
BanManager::BanManager()
{
	BanTypes.push_back("no");
	BanTypes.push_back("new");
	BanTypes.push_back("select");
	BanTypes.push_back("all");
	BanTypes.push_back("name");
	BanTypes.push_back("switch");
}
std::string BanManager::GetStringByType(const BanType Type)
{
	for(unsigned int i = 0;i < BanTypes.size();++i)
	{
		if( (BanType)i == Type )
			return BanTypes[i];
	}
	return "Unknown";
}
BanType BanManager::GetTypeByString(const std::string &Type)
{
	for(unsigned int i = 0;i < BanTypes.size();++i)
	{
		if( !str_cmp(BanTypes[i], Type) )
			return (BanType)i;
	}
	return BAN_NOT;//Default?

}
BanManager::~BanManager()
{
	while( !BanList.empty() )
	{
		delete (BanList.front());
		RemoveBan( BanList.front() );
	}
	Self = NULL;
}
void BanManager::Boot()
{
	sql::Query MyQuery;
	sql::Row MyRow;
	std::stringstream Query;

	Query << "SELECT * FROM banlist ORDER BY id ASC;";
	try {
		MyQuery = gameDatabase->sendQuery(Query.str());
	} catch(sql::QueryException &e) {
		e.report();
		return;
	}

	while( MyQuery->hasNextRow() )
	{
		MyRow = MyQuery->getRow();
		AddBan( new BanElement((BanType)atoi(MyRow["type"].c_str()), atoi(MyRow["date"].c_str()),
			MyRow["name"], MyRow["site"], atoi(MyRow["id"].c_str())));
	}
}
void BanManager::save()
{
	std::stringstream Query;

	for( std::list< BanElement * >::iterator bIter = BanList.begin();bIter != BanList.end();bIter++ )
	{
		//TODO: Finish this...
	}
}
void BanManager::Reload()
{
	BanManager::GetManager().Free();
	BanManager::GetManager().Boot();
}
bool BanManager::IsValidName(const std::string &Name)
{
	return (IsBanned(Name) != BAN_NAME);
}
BanType BanManager::IsBanned( const std::string &Host )
{
	for( std::list<BanElement *>::iterator bIter = BanList.begin();bIter != BanList.end();bIter++ )
	{
		if( !str_cmp((*bIter)->GetSite(), Host) )
			return (*bIter)->GetType();
	}
	return BAN_NOT;
}
void BanManager::AddBan( BanElement *MyNewElement )
{
	BanList.push_back( MyNewElement );
}
void BanManager::AddBan( const std::string Name )
{
	BanElement *be = new BanElement();
	be->SetType( BAN_NAME );
	be->SetDate( time(0) );
	be->SetSite( Name );
	AddBan( be );
}
std::list< BanElement * >::iterator BanManager::RemoveBan( BanElement *MyElement )
{
	std::list< BanElement * >::iterator bIter = std::find(BanList.begin(),BanList.end(),MyElement);
	return BanList.erase(bIter);
}
std::string BanManager::GrabBanList()
{
	if( BanList.empty() )
		return "There are no banned sites...\r\n";
	std::stringstream Buffer;
	Buffer << "Num        Date          Site           Banned By       Type" << std::endl;
	Buffer << "-----------------------------------------------------------------" << std::endl;
	int i = 1;
	for( std::list< BanElement * >::iterator bIter = BanList.begin();bIter != BanList.end();++bIter, ++i )
	{
		std::string DateStr = Time::FormatDate( "%m-%d-%Y", (*bIter)->GetDate() );
		Buffer << std::setw(3) << std::right << i << ")    " << std::setw(11) << std::left
			<< DateStr << "    " << std::setw(19) << std::left << (*bIter)->GetSite()
			<< std::setw(13) << std::left << (*bIter)->getName() << (*bIter)->GetTypeStr() << std::endl;
	}
	return Buffer.str();
}
std::list< BanElement * > BanManager::GetBansByName(const std::string &Name)
{
	std::list< BanElement * > TheBanList;
	for( std::list< BanElement * >::iterator bIter = BanList.begin();bIter != BanList.end();++bIter )
	{
		if( (*bIter)->GetSite() == Name )
			TheBanList.push_back( (*bIter) );
	}
	return TheBanList;
}
std::list< BanElement * > BanManager::GetBansByType(const BanType type)
{
	std::list< BanElement * > TheBanList;
	for( std::list< BanElement * >::iterator bIter = BanList.begin();bIter != BanList.end();++bIter )
	{
		if( (*bIter)->GetType() == type )
			TheBanList.push_back( (*bIter) );
	}
	return TheBanList;
}
BanElement *BanManager::GetBanBySite( const std::string &Site )
{
	for( std::list< BanElement * >::iterator bIter = BanList.begin();bIter != BanList.end();++bIter )
	{
		if( !str_cmp( (*bIter)->GetSite(), Site ) )
			return (*bIter);
	}
	return NULL;
}
/*** Galnor, 04/27/2009 - Free all of the memory from the singleton ***/
void BanManager::Free()
{
	if( Self ) {
		delete (Self);
	}
}

BanElement::BanElement()
{
	type = BAN_NOT;
	date = 0;
	dbID = -1;
}
BanElement::BanElement( const BanType t, const time_t d, std::string N, std::string S, const int id )
{
	type = t;
	date = d;
	Name = N;
	Site = S;
	dbID = id;
}
BanElement::~BanElement() {}//Do nothing?

BanType BanElement::GetType()
{
	return type;
}
time_t BanElement::GetDate()
{
	return date;
}
std::string BanElement::getName()
{
	return Name;
}
std::string BanElement::GetSite()
{
	return Site;
}
/*** Accessor Methods ***/
void BanElement::SetType(const BanType t)
{
	type = t;
}
void BanElement::SetDate(const time_t d)
{
	date = d;
}
void BanElement::setName(const std::string N)
{
	Name = N;
}
void BanElement::SetSite(const std::string S)
{
	Site = S;
}

std::string BanElement::GetTypeStr()
{
	return BanManager::GetManager().GetStringByType( GetType() );
}

/*** Add the ban element to the database and update its dbID variable ***/
void BanElement::AddToDatabase()
{
	std::stringstream Query;

	Query << "INSERT INTO banlist(type,date,name,site) VALUES('" << (int)GetType() << "','" << GetDate()
		<< "','" << sql::escapeString(getName()) << "','" << sql::escapeString(GetSite()) << "');";

	try {
		gameDatabase->sendRawQuery(Query.str());
	} catch( sql::QueryException &e ) {
		MudLog(NRM, LVL_GOD, TRUE, "Unable to add ban element(site:%s) to database : %s",
			GetSite().c_str(), e.message.c_str());
		return;//Get outta here!
	}
	//We made it through the query. Now just update the dbID...
	this->dbID = gameDatabase->lastInsertID();
	MudLog(NRM, LVL_GOD, TRUE, "Adding ban to banlist table. Primary: %d", dbID);
}
/*** Remove the ban element from the database ***/
void BanElement::RemoveFromDatabase()
{
	if( dbID == -1 )//Invalid ID. Get out.
		return;
	std::stringstream Query;
	Query << "DELETE FROM banlist WHERE id='" << dbID << "';";
	try {
		gameDatabase->sendRawQuery(Query.str());
	} catch( sql::QueryException &e ) {
		MudLog(NRM, LVL_GOD, TRUE, "Unable to remove ban element(site:%s) from database : %s",
			GetSite().c_str(), e.message.c_str());
		return;
	}
	MudLog(NRM, TRUE, LVL_GOD, "Removed ban with primary %d from banlist table.", dbID);
	dbID = -1;
}

CommandHandler do_ban = DEFINE_COMMAND
{
	char flag[MAX_INPUT_LENGTH], site[MAX_INPUT_LENGTH];

	if (!*argument)
	{
		ch->send(BanManager::GetManager().GrabBanList().c_str());
		return;
	}
	TwoArguments(argument, flag, site);

	if (!*site || !*flag)
	{
		ch->send("Usage: ban {all | select | new | name} site_name\r\n", ch);
		return;
	}

	if (!(!str_cmp(flag, "select") || !str_cmp(flag, "all") || !str_cmp(flag, "new") || !str_cmp(flag, "name")))
	{
		ch->send("Flag must be ALL, SELECT, NEW, or NAME.\r\n");
		return;
	}

	//Does the ban already exist?
	if( BanManager::GetManager().GetBanBySite( site ) != NULL ) 
	{
		ch->send("That site has already been banned -- unban it to change the ban type.\r\n");
		return;
	}
	BanElement *NewElement = new BanElement(BanManager::GetManager().GetTypeByString(flag), time(0), GET_NAME(ch), site );
	BanManager::GetManager().AddBan( NewElement );

	if( NewElement->GetType() == BAN_NAME )
	{
		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "%s has blacklisted the player-name %s.",
			ch->player.name.c_str(), NewElement->GetSite().c_str());
	}
	else
	{
		MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "%s has banned %s for %s players.",
			GET_NAME(ch), NewElement->GetSite().c_str(), NewElement->GetTypeStr().c_str());
	}
	ch->send("Site banned.\r\n");
	NewElement->AddToDatabase();
};

CommandHandler do_unban = DEFINE_COMMAND
{
	char site[MAX_INPUT_LENGTH];

	OneArgument(argument, site);

	if (!*site)
	{
		ch->send("A site to unban might help.\r\n");
		return;
	}
	BanElement *MyElement = BanManager::GetManager().GetBanBySite(site);

	if( MyElement == NULL )//Not found!
	{
		ch->send("That site is not currently banned.\r\n");
		return;
	}

	ch->send("Site unbanned.\r\n");
	MudLog(NRM, MAX(LVL_GOD, GET_INVIS_LEV(ch)), TRUE, "%s removed the %s-ban on %s.",
		GET_NAME(ch), MyElement->GetTypeStr().c_str(), MyElement->GetSite().c_str());

	BanManager::GetManager().RemoveBan( MyElement );
	MyElement->RemoveFromDatabase();
	delete MyElement;
};
