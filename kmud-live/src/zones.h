#ifndef ZONES_H
#define ZONES_H

class ResetCommand
{
	public:
		char command;   /* current command                      */
		bool altered;
		bool deleted;
		int dbID;

		bool if_flag;	/* if TRUE: exe only if preceding exe'd */
		int	arg1;		/*                                      */
		int	arg2;		/* Arguments to the command             */
		int	arg3;		/*                                      */
		int arg4;
		int	arg5;
		int	arg6;
		int	arg7;
		int	line;		/* line number this command appears on  */

		/* whenever a mob is loaded with 'M', this is that mob */
		Character *mob;
		Object *obj;

		int GetRealArg1();
		int GetRealArg2();
		int GetRealArg3();
		int GetRealArg4();
		int GetRealArg5();
		int GetRealArg6();
		int GetRealArg7();

		ResetCommand();
		ResetCommand(ResetCommand *Source);
		void Boot( sql::Row &CmdRow );
		/*
		 *  Commands:              *
		 *  'M': Read a mobile     *
		 *  'O': Read an object    *
		 *  'G': Give obj to mob   *
		 *  'P': Put obj in obj    *
		 *  'G': Obj to char       *
		 *  'E': Obj to char equip *
		 *  'D': Set state of door *
		 */
};

class ZoneManager
{
private:
	boost::recursive_mutex ZoneListMutex;

	std::vector< Zone* > ZoneList;
	static ZoneManager *Self;
public:
	static boost::recursive_mutex SingletonMutex;
	ZoneManager() {};
	~ZoneManager();
	static ZoneManager& GetManager();

	const size_t NumZones(); //Thread safe.
	Zone* AddNewZone( const unsigned int vnum ); //Thread safe.
	Zone *GetZoneByVnum( const unsigned int vnum ); //Thread safe.
	Zone *GetZoneByRnum( const unsigned int rnum ); //Thread safe.
	Zone *GetZoneByRoomVnum( const unsigned int rvnum ); //Thread safe.

	void LoadThreadedZoneBatch( sql::Connection connection, const int zoneIndexOffset, const int zoneIndexFetchSize ); //Thread safe.

	void RenumberZones(); //Thread safe.
	void BootZones(); //Thread safe.
	void SaveZones(); //Thread safe.
	void Free(); //Thread safe.
};

/* zone definition structure. for the 'zone-table'					*/
class Zone
{
private:
	std::string Name;		/* name of this zone					*/
	int	lifespan;			/* how long between resets (minutes)	*/
	int	age;				/* current age of this zone (minutes)	*/
	int	x;					/* X-axis for zone location				*/
	int y;					/* Y-axis for zone location				*/

	unsigned int bot;		/* starting room number for this zone	*/
	unsigned int top;		/* upper limit for rooms in this zone	*/
	int	reset_mode;         /* conditions for reset (see below)		*/
	unsigned int vnum;				/* virtual number of this zone			*/
	unsigned int rnum;

	std::string Sunrise;
	std::string Sunset;
	std::string Builders;	/* for OLC.  OBuild like extention,		*/
	bool deleted;
	bool inDB;
	bool closed;

	class Weather *weather;
public:

	Zone();
	Zone( const unsigned int vnum );
	Zone( Zone *Source );
	~Zone();

	std::vector< class ResetCommand *> cmd;	/* command table for reset				*/

	Weather *GetWeather() { return weather; }
	void SetWeather( Weather *NewWeather ) { weather = NewWeather; }

	std::string getName() { return Name; }
	void setName( const std::string &Name ) { this->Name = Name; }

	const int GetLifespan() { return lifespan; }
	void SetLifespan( const int _lifespan ) { lifespan = _lifespan; }

	const int GetAge() { return age; }
	void SetAge( const int _age ) { age = _age; }

	const int GetX() { return x; }
	void SetX( const int _x ) { x = _x; }

	const int GetY() { return y; }
	void SetY( const int _y ) { y = _y; }

	const unsigned int GetTop() { return top; }
	void SetTop( const int _top ) { top = _top; }

	const unsigned int GetBottom() { return bot; }
	void SetBottom( const int _bot ) { bot = _bot; }

	const int GetResetMode() { return reset_mode; }
	void SetResetMode( const int rm ) { reset_mode = rm; }

	std::string GetSunrise() { return Sunrise; }
	void SetSunrise( const std::string &Sunrise ) { this->Sunrise = Sunrise; }

	std::string GetSunset() { return Sunset; }
	void SetSunset( const std::string &Sunset ) { this->Sunset = Sunset; }

	std::string GetBuilders() { return Builders; }
	void SetBuilders( const std::string &Builders ) { this->Builders = Builders; }

	bool IsClosed() { return closed; }
	bool IsDeleted() { return deleted; }
	bool IsInDB() { return inDB; }

	void SetDeleted() { deleted = true; }
	void UnsetDeleted() { deleted = false; }

	bool CanEdit( std::string &Name );
	bool CanEdit( Character *Editor );

	void SetClosed( bool c ) { closed = c; }

	unsigned int CountObjects( const int obj_nr );
	unsigned int CountMobs( const int mob_nr );

	void LogError( int cmd_no, const char *message );
	void PrintToBuffer( std::string &Buffer );
	void CopyFrom( Zone *Source );

	bool AddCommand( const unsigned int pos );
	bool AddCommand( ResetCommand *RC, const unsigned int pos );
	bool RemoveCommand( const unsigned int pos, bool purge=false );
	bool RemoveCommand( ResetCommand *RC, bool purge=false );

	room_vnum FindUnusedRoomVnum();
	void Extract();
	void Reset();
	unsigned int getVnum() { return vnum; }
	void setVnum( const int _vnum ) { vnum = _vnum; }
	unsigned int GetRnum() { return rnum; }
	void SetRnum( const int _rnum ) { rnum = _rnum; }

	void Boot( const sql::Row &ZoneRow, std::list< sql::Row > &RowList );
	void Save();
	int Distance( Zone *OtherZone );

	/*
	 *  Reset mode:                              *
	 *  0: Don't reset, and don't update age.    *
	 *  1: Reset if no PC's are located in zone. *
	 *  2: Just reset.                           *
	 */
};

/* for queueing zones for update   */
struct reset_q_element

{
	int	zone_to_reset;            /* ref to Zone */
	struct reset_q_element *next;
};

/* structure for the update queue     */
struct reset_q_type

{
	struct reset_q_element *head;
	struct reset_q_element *tail;
};

#endif
