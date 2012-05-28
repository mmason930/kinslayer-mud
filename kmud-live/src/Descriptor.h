#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "DateTime.h"

class Descriptor
{
private:
	void cleanup();
public:
	Descriptor();
	~Descriptor();

	bool loggedIn;					//Has this descriptor successfully logged in during this session?
	class kuDescriptor *descriptor;
	char	host[ 31 ];				// Hostname
	byte	bad_pws;				// Number of bad pw attemps this login
	byte	idle_tics;				// Tics idle at password prompt
	int	connected;					// Mode of 'connectedness'
	sh_int	wait;					// Wait for how many loops
	int	desc_num;					// Unique num assigned to desc
	DateTime	loginTime;			// When the player connected
	char	*showstr_head;			// For keeping track of an internal str
	char	**showstr_vector;		// For paging through texts
	int	showstr_count;				// Number of pages to page through
	int	showstr_page;				// Which page are we currently showing?
	char	**str;					// For the modify-str system
	size_t	max_str;
	char	*backstr;				// Added for handling abort buffers
	class Character *character;		// linked to char
	class Character *original;		// original char if switched
	class Descriptor *snooping;		// Who is this char snooping
	class Descriptor *snoop_by;		// And who is snooping this char
	class Descriptor *next;			// Link to next descriptor
	class OLC *olc;					// OLC info - defined in olc.h
	bool hadInput;
	bool hadOutput;

	const char *MakePrompt();
	void Send( const char *messg, ... );
	void Send( const std::string s );
	void SendRaw( const char *messg );
	void NewbieMenuFinish();
	void EchoOn();
	void EchoOff();
	void Nanny( char* arg);
	bool AddToString( const char *txt );
	bool HasPermissionToSnoop();

	void disconnect();
	void persistentDisconnect();
	void closeSocketClean();

	std::string session;

	Descriptor& operator<< ( const std::string &s );
	Descriptor& operator<< ( const int );
	Descriptor& operator<< ( const float );
	Descriptor& operator<< ( const double );
	Descriptor& operator<< ( const bool );
	Descriptor& operator<< ( const char * s );
	Descriptor& operator<< ( const char s );
};

#endif
