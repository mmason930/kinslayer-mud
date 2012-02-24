#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "db.h"
#include "handler.h"
#include "accounts.h"
#include "md5.h"
#include "MiscUtil.h"
#include "Descriptor.h"
#include "CharacterUtil.h"

extern Character *character_list;

int Character::GetAccount()
{
	if( this->PlayerData )
		return this->PlayerData->account_id;
	return 0;
}
void Character::SetAccount( const int id )
{
	if( this->PlayerData )
		this->PlayerData->account_id = id;
}

void SwitchManager::Free()
{
	if( Self )
		delete (Self);
}
void SwitchManager::AddSwitch( std::string IP, std::string Name, time_t switchTime, __int64 accountID )
{
	for( std::list<Switch*>::iterator iter = SwitchList.begin();iter != SwitchList.end();++iter ) {
		if( (*iter)->getName() == Name && (*iter)->GetIP() == IP ) {
			delete (*iter);
			SwitchList.erase( iter );
			break;
		}
	}
	SwitchList.push_back( new Switch( IP, Name, switchTime, accountID ) );
}
SwitchManager::SwitchManager() { Self = NULL; }
SwitchManager::~SwitchManager()
{
	for( std::list<Switch*>::iterator sIter = SwitchList.begin();sIter != SwitchList.end();++sIter )
	{
		delete (*sIter);
	}
	SwitchList.clear();
	Self = NULL;
}
SwitchManager& SwitchManager::GetManager()
{
	if( !Self ) Self = new SwitchManager();
	return (*Self);
}
std::list<Switch*>::iterator SwitchManager::RemoveSwitchByIP( std::string IP )
{
	for(std::list< Switch* >::iterator sIter = SwitchList.begin();sIter != SwitchList.end();)
	{
		if( (*sIter)->GetIP() == IP )
			sIter = SwitchList.erase( sIter );
		else
			++sIter;
	}
	return SwitchList.end();
}
std::list<Switch*>::iterator SwitchManager::RemoveSwitchByName( std::string Name )
{
	for(std::list< Switch* >::iterator sIter = SwitchList.begin();sIter != SwitchList.end();)
	{
		if( (*sIter)->getName() == Name )
			sIter = SwitchList.erase( sIter );
		else
			++sIter;
	}
	return SwitchList.end();
}
bool SwitchManager::HasWaitedLongEnough( std::string Name, std::string Host, Switch *sw )
{
	return (TimeRemaining(Name,Host,sw).Seconds() <= 0);
}

Account SwitchManager::GetAccountByName( const std::string &Name )
{
	std::stringstream QueryBuffer;
	sql::Query MyQuery;
	Account a;

	QueryBuffer << "SELECT * FROM accounts WHERE account_name='" << sql::escapeString(Name) << "';";

	try {
		MyQuery = gameDatabase->sendQuery( QueryBuffer.str() );
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Error loading account by name: %s", e.getMessage().c_str());
		return a;
	}
	if( !MyQuery->hasNextRow() )
	{
		return a;
	}
	a.BootFromSQL( MyQuery->getRow() );
	return a;
}
Account SwitchManager::GetAccountByID( const int id )
{
	std::stringstream QueryBuffer;
	sql::Query MyQuery;
	Account a;

	QueryBuffer << "SELECT * FROM accounts WHERE id='" << id << "';";

	try {
		MyQuery = gameDatabase->sendQuery( QueryBuffer.str() );
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Error loading account by id: %s", e.getMessage().c_str());
		return a;
	}
	if( !MyQuery->hasNextRow() )
	{
		return a;
	}
	a.BootFromSQL( MyQuery->getRow() );
	return a;
}
Account SwitchManager::GetAccountByPlayerName( const std::string &Name )
{
	std::stringstream QueryBuffer;
	sql::Query MyQuery;
	Account a;

	QueryBuffer << "SELECT a.* FROM accounts a JOIN users u ON u.account_id=a.id WHERE u.username='"
		<< sql::escapeString(Name) << "';";

	try {
		MyQuery = gameDatabase->sendQuery( QueryBuffer.str() );
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Error loading account by player name: %s", e.getMessage().c_str());
		return a;
	}
	if( !MyQuery->hasNextRow() )
	{
		return a;
	}
	a.BootFromSQL( MyQuery->getRow() );
	if( a.GetID() == 0 )
		a.SetValidity( false );//Default account ID...
	return a;
}
Account SwitchManager::GetAccountByPlayerID( const int id )
{
	std::stringstream QueryBuffer;
	sql::Query MyQuery;
	Account a;

	QueryBuffer << "SELECT a.* FROM accounts a JOIN users u ON u.account_id=a.id WHERE u.user_id='" << id << "';";

	try {
		MyQuery = gameDatabase->sendQuery( QueryBuffer.str() );
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Error loading account by player id: %s", e.getMessage().c_str());
		return a;
	}
	if( !MyQuery->hasNextRow() )
	{
		return a;
	}
	a.BootFromSQL( MyQuery->getRow() );
	if( a.GetID() == 0 )
		a.SetValidity( false );//Default account ID...
	return a;
}
void Account::BootFromSQL( sql::Row MyRow )
{
	this->aID		= atoi( MyRow["id"].c_str() );
	this->aName		= MyRow["account_name"];
	this->aCreated	= MiscUtil::Convert< time_t >( MyRow["created"] );
	this->SetValidity( true );
}
bool SwitchManager::WillBeMultiplaying( const std::string &Host, const std::string &Name )
{
	Character *ch, *t;
	std::list< Character * > MyAlts;
	std::list< pLogin > tLogins;
	std::string tHost;
	Account playerLoggingInAccount = GetAccountByPlayerName( Name );
	bool isAlt;

//	chA = this->GetAccountByPlayerName( Name );
	for( t = character_list;t;t = t->next )
	{
		if( t->IsPurged() || IS_NPC(t) || !str_cmp(Name, t->player.name) )
			continue;
		isAlt = false;
		if( t->PlayerData->account_id != -1 && playerLoggingInAccount.GetID() != -1 ) {
			if( t->PlayerData->account_id == playerLoggingInAccount.GetID() )
				isAlt=true;
			else
				continue;
		}

		if( isAlt == false ) {
			tHost.clear();//Clear the host buffer.
			if( t->desc )
				tHost = t->desc->host;
			else
			{
				//We know there is no link. We will now load the host via database.
				Character::LoadLogins(tLogins, t->player.name, 1, -1, -1);
				if( !tLogins.empty() )
				{
					tHost = tLogins.front().Host;

					//Erase any logins.
					while( !tLogins.empty() )
					{
						tLogins.pop_front();
					}
				}
			}
			if( tHost.empty() || str_cmp(Host, tHost) )
				continue;//IPs do not match.
		}
		MyAlts.push_back( t );
	}
	if( IsExemptFromMultiplayRestriction(Host, Name, MyAlts) )
		return false;
	return true;
}
Time SwitchManager::TimeRemaining(std::string Name, std::string Host, Switch *sw)
{
	return Time( MAX(0, (time(0) - (TimeToWait(Name,Host,sw) - sw->GetTime().Seconds()))) );
}
int SwitchManager::TimeToWait(std::string Name, std::string Host, Switch *sw)
{
	const static int iTimeForSameRace			= 3*60;
	const static int iTimeForOppositeRace		= 8*60;
	const static int iTimeForMortalToImmortal	= 0*60;
	const static int iTimeForImmortalToMortal	= 8*60;
	const static int iTimeForNewAlt				= 8*60;

	if( !str_cmp(Name, sw->getName()) )
		return 0;

	int iTimeToWait = 0;
	Character *currentCharacter = NULL;
	Character *lastCharacter = NULL;

	if( IsExemptFromWaiting( sw->GetIP(), sw->getName(), Host, Name ) )
	{
		iTimeToWait = 0;
	}
	else if( (lastCharacter = CharacterUtil::loadCharacter(sw->getName())) == NULL )
	{//Somehow, the alt we're switching FROM no longer exists. In this case, we'll wipe the slate clean.
		iTimeToWait = 0;
	}
	else if( (currentCharacter = CharacterUtil::loadCharacter(Name)) == NULL )
	{//If we get here, this means that they are trying to switch to a new character.
		iTimeToWait = iTimeForNewAlt;
	}
	else if( currentCharacter && lastCharacter && currentCharacter->PlayerData->account_id != -1 &&
		lastCharacter->PlayerData->account_id != -1
		&& currentCharacter->PlayerData->account_id != lastCharacter->PlayerData->account_id )
	{
		iTimeToWait = 0;
	}
	else if( GET_LEVEL(currentCharacter) >= 100 )
	{//Switching TO an immortal.
		iTimeToWait = iTimeForMortalToImmortal;
	}
	else if( GET_LEVEL(lastCharacter) >= 100 )
	{//Switching FROM an immortal.
		iTimeToWait = iTimeForImmortalToMortal;
	}
	else if( GET_RACE(lastCharacter) != GET_RACE(currentCharacter) )
	{//Cross race switch.
		iTimeToWait = iTimeForOppositeRace;
	}
	else
	{//Same race, mortal to mortal.
		iTimeToWait = iTimeForSameRace;
	}
	if(currentCharacter)
		delete currentCharacter;
	if(lastCharacter)
		delete lastCharacter;

	return iTimeToWait;
}
bool SwitchManager::AuthCheck( const std::string sFromName, const std::string &sToName, const std::string &sAuthKey )
{
	std::stringstream streamBuffer;
	std::string::size_type iSizePrior = -1;
	for(std::string::size_type index = 0;iSizePrior != streamBuffer.str().size();++index)
	{
		iSizePrior = streamBuffer.str().size();
		if( index < sFromName.size() )
			streamBuffer << sFromName[ index ];
		if( index < sToName.size() )
			streamBuffer << sToName[ index ];
	}
	return MD5::getHashFromString( streamBuffer.str().c_str() ) == sAuthKey;
}
bool SwitchManager::IsExemptFromWaiting( const std::string &sHostFrom, const std::string &sNameFrom, const std::string &sHostTo, const std::string &sNameTo )
{
	std::stringstream QueryBuffer;
	sql::Query MyQuery;
	sql::Row MyRow;

	QueryBuffer 
		<< "SELECT * FROM switch_exemptions"
		<< " WHERE type='" << SwitchManager::SWITCH_EXEMPTION_WAITING << "'"
		<< " AND (from_username='*' OR from_username='" << sql::escapeString(sNameFrom) << "')"
		<< " AND (from_host='*' OR from_host='" << sql::escapeString(sHostFrom) << "')"
		<< " AND (to_username='*' OR to_username='" << sql::escapeString(sNameTo) << "')"
		<< " AND (to_host='*' OR to_host='" << sql::escapeString(sHostTo) << "')"
		<< ";";

	try {
		MyQuery = gameDatabase->sendQuery(QueryBuffer.str());

		if( MyQuery->numRows() == 0 )
			return false;
		MyRow = MyQuery->getRow();
		return AuthCheck(MyRow["from_username"],MyRow["to_username"], MyRow["auth_key"] );
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_IMPL, TRUE, "IsExemptFromWaiting() - %s", e.getMessage().c_str());
		return false;
	}
	return true;
}
bool SwitchManager::IsExemptFromMultiplayRestriction( const std::string &sHost, const std::string &sName, const std::list< std::string > lAlreadyLoggedInNames )
{
	std::stringstream QueryBuffer;
	sql::Query MyQuery;
	sql::Row MyRow;

	QueryBuffer 
		<< "SELECT * FROM switch_exemptions"
		<< " WHERE type='" << SwitchManager::SWITCH_EXEMPTION_MULTIPLAYING << "'"
		<< ";";

	try {
		MyQuery = gameDatabase->sendQuery( QueryBuffer.str() );
		for( std::list<std::string>::const_iterator iter = lAlreadyLoggedInNames.begin();iter != lAlreadyLoggedInNames.end();++iter)
		{
			MyQuery->resetRowQueue();
			bool bFound=false;
			while( MyQuery->hasNextRow() )
			{
				MyRow = MyQuery->getRow();

				std::string sIterName = (*iter);
				std::string sFromUsername = MyRow["from_username"];
				std::string sToUsername = MyRow["to_username"];

				if(
					((!str_cmp(MyRow["from_username"],sName) && !str_cmp(MyRow["to_username"],(*iter)))
				||	(!str_cmp(MyRow["to_username"],sName) && !str_cmp(MyRow["from_username"],(*iter))))
				&&  (AuthCheck(MyRow["from_username"],MyRow["to_username"], MyRow["auth_key"]))
				)
				{
					bFound=true;
					break;
				}
			}
			if( !bFound )
				return false;
		}

	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_IMPL, TRUE, "IsExemptFromMultiplayRestriction() - %s", e.getMessage().c_str());
	}
	return true;
}
bool SwitchManager::IsExemptFromMultiplayRestriction( const std::string &sHost, const std::string &sName, const std::list< Character* > lAlreadyLoggedIn )
{
	std::list<std::string> lAlreadyLoggedInNames;
	for(std::list<Character*>::const_iterator iter = lAlreadyLoggedIn.begin();iter != lAlreadyLoggedIn.end();++iter)
	{
		lAlreadyLoggedInNames.push_back( GET_NAME( (*iter) ) );
	}
	return IsExemptFromMultiplayRestriction( sHost, sName, lAlreadyLoggedInNames );
}
Switch *SwitchManager::GetSwitchByIP( const std::string &IP )
{
	std::list<Switch*> MySwitches = GetSwitchesByIP( IP );
	return MySwitches.empty() ? NULL : SwitchList.front();
}
Switch *SwitchManager::GetSwitchByName( const std::string &Name )
{
	std::list<Switch*> MySwitches = GetSwitchesByName( Name );
	return MySwitches.empty() ? NULL : SwitchList.front();
}
Switch *SwitchManager::GetSwitchByKnownAlt( const std::string &sAltName )
{
	std::list<Switch*> MySwitches = GetSwitchesByKnownAlt( sAltName );
	return MySwitches.empty() ? NULL : SwitchList.front();
}
std::list<Switch*> SwitchManager::GetSwitchesByName( const std::string &Name )
{
	std::list<Switch*> MySwitches;
	for(std::list< Switch* >::iterator sIter = SwitchList.begin();sIter != SwitchList.end();++sIter)
	{
		if( (*sIter)->getName() == Name )
			MySwitches.push_back( (*sIter) );
	}
	return MySwitches;
}
std::list<Switch*> SwitchManager::GetSwitchesByIP( const std::string &IP )
{
	std::list<Switch*> MySwitches;
	for(std::list< Switch* >::iterator sIter = SwitchList.begin();sIter != SwitchList.end();++sIter)
	{
		if( (*sIter)->GetIP() == IP )
			MySwitches.push_back( (*sIter) );
	}
	return MySwitches;
}
std::list<Switch*> SwitchManager::GetSwitchesByKnownAlt( const std::string &sAltName )
{
	std::list<Switch*> MySwitches;
	Account account = SwitchManager::GetManager().GetAccountByPlayerName( sAltName );

	//If the account is invalid, we're not going to find a match.
	if( account.IsValid() == false )
		return MySwitches;

	//Using the account, return the first switch that has a match, if any.
	for(std::list< Switch* >::iterator sIter = SwitchList.begin();sIter != SwitchList.end();++sIter)
	{
		if( (*sIter)->GetAccountID() != -1 && (*sIter)->GetAccountID() == account.GetID() )
			MySwitches.push_back( (*sIter) );
	}
	return MySwitches;
}
Switch *SwitchManager::GetGreatestSwitch( const std::string &Host, const std::string &UserName )
{
	std::list< Switch* > MySwitches = GetSwitchesByIP( Host );
	std::list< Switch* > Temp = GetSwitchesByName( UserName );

	while( Temp.empty() == false )
	{
		MySwitches.push_back( Temp.front() );
		Temp.pop_front();
	}

	Temp = GetSwitchesByKnownAlt( UserName );
	while( Temp.empty() == false )
	{
		MySwitches.push_back( Temp.front() );
		Temp.pop_front();
	}

	Switch *Greatest = NULL;
	time_t greatestTime = 0;

	for(std::list<Switch*>::iterator iter = MySwitches.begin();iter != MySwitches.end();++iter)
	{
		Time RemainingTime = TimeRemaining(UserName,Host,(*iter));

		if( RemainingTime.Seconds() > greatestTime ) {
			greatestTime = RemainingTime.Seconds();
			Greatest = (*iter);
		}
	}
	return Greatest;
}
void SwitchManager::PrintSwitchesToBuffer( std::stringstream &Buffer )
{
	Buffer << "   Host           Name                   Time" << std::endl;
	for( std::list< Switch* >::iterator iter = SwitchList.begin();iter != SwitchList.end();++iter )
	{
		Buffer << std::left << std::setw(15) << (*iter)->GetIP();
		Buffer << std::left << std::setw(20) << (*iter)->getName();
		Buffer << std::left << std::setw(30) << Time::FormatDate("%Y-%m-%d %H:%M:%S", time(0) - (*iter)->GetTime().Seconds());
		Buffer << std::endl;
	}
}

/******* Switch Class ********/
Switch::Switch() {
	accountID = -1;
}
Switch::Switch( std::string _IP, std::string _Name, const time_t _switchTime, __int64 _accountID )
{
	IP = _IP;
	Name = _Name;
	this->SwitchTime.SetTime( _switchTime );
	accountID = _accountID;
}
Switch::~Switch() {}


Account::Account()
{
	this->aValid	= false;
	this->aCreated	= 0;
	this->aID		= -1;
}
Account::~Account()
{
	//...
}

std::string Account::getName()
{
	return aName;
}
int Account::GetID()
{
	return aID;
}
time_t Account::GetCreated()
{
	return aCreated;
}

bool Account::IsValid()
{
	return aValid;
}
void Account::SetValidity( bool v )
{
	aValid = v;
}
