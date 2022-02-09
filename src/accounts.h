#ifndef ACCOUNTS_H
#define ACCOUNTS_H

class Switch
{
private:
	std::string IP;
	std::string Name;
	__int64 accountID;
	Time SwitchTime;
public:
	Switch();
	Switch( std::string _IP, std::string _Name, const time_t _switchTime, __int64 _accountID );
	~Switch();

	std::string GetIP() { return IP; }
	std::string getName() { return Name; }
	__int64 GetAccountID() { return accountID; }
	Time& GetTime() { return SwitchTime; }
};
class Account
{
private:
	std::string aName;
	int aID;
	time_t aCreated;
	bool aValid;

public:
	Account();
	~Account();

	std::string getName();
	int GetID();
	time_t GetCreated();
	void BootFromSQL( sql::Row MyRow );

	bool IsValid();
	void SetValidity( bool v );
};

class SwitchManager
{
private:
	SwitchManager();
	~SwitchManager();

	static SwitchManager* Self;
	std::list< Switch* > SwitchList;
public:
	static SwitchManager& GetManager();

	void AddSwitch( std::string IP, std::string Name, time_t switchTime, __int64 accountID );
	std::list<Switch*>::iterator RemoveSwitchByIP( std::string IP );
	std::list<Switch*>::iterator RemoveSwitchByName( std::string Name );

	Account GetAccountByName( const std::string &Name );
	Account GetAccountByID( const int id );
	Account GetAccountByPlayerName( const std::string &Name );
	Account GetAccountByPlayerID( const int id );
	bool WillBeMultiplaying( const std::string &Host, const std::string &Name );
	bool HasWaitedLongEnough( std::string Name, std::string Host, Switch* sw );
	bool IsExemptFromWaiting( const std::string &sHostFrom, const std::string &sNameFrom, const std::string &sHostTo, const std::string &sNameTo );
	bool IsExemptFromMultiplayRestriction( const std::string &userName, const std::list< Character* > lAlreadyLoggedIn );
	bool IsExemptFromMultiplayRestriction( const std::string &userName, const std::list< std::string > lAlreadyLoggedInNames );
	Time TimeRemaining(std::string Name, std::string Host, Switch *sw);
	int TimeToWait(std::string Name, std::string Host, Switch *sw);
	void Free();
	bool AuthCheck( const std::string sFromName, const std::string &sToName, const std::string &sAuthKey );

	Switch* GetSwitchByName( const std::string &Name );
	Switch* GetSwitchByIP( const std::string &IP );
	Switch* GetSwitchByKnownAlt( const std::string &sAltName );
	std::list< Switch* > GetSwitchesByKnownAlt( const std::string &sAltName );
	std::list< Switch* > GetSwitchesByIP( const std::string &IP );
	std::list< Switch* > GetSwitchesByName( const std::string &Name );
	Switch* GetGreatestSwitch( const std::string &Host, const std::string &UserName );

	void PrintSwitchesToBuffer( std::stringstream &Buffer );

	enum eSwitchExemption {
		SWITCH_EXEMPTION_WAITING=0,
		SWITCH_EXEMPTION_MULTIPLAYING=1
	};
};

#endif
