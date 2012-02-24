//
// C++ Interface: weaves
//
// Description: All the classes used to control weaves themselves are stored here.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef WEAVE_H
#define WEAVE_H

#define SOURCE_TYPE(ch) IS_CHANNELER(ch) ? ( GET_SEX(ch) == SEX_MALE ? "Saidin" : "Saidar" ) : (IS_FADE(ch) ? "True Power" : "the power")  
#define EVENT(function) void (function)(void *info)

#endif

typedef std::pair< Character *, affected_type* > wTuple;

class Weave
{
private:
	std::string Name;
	std::map< std::string, std::string > Attributes;
public:
	Weave();
	Weave( std::string _Name );

	std::pair< std::string, std::string > GetNthAttribute(int starting);

	void addToBatchInsertStatement( sql::BatchInsertStatement &batchInsertStatement );
	void setAttribute( std::string Key, std::string Value );
	void setName( std::string NewName );
	std::string getAttribute( std::string Key );
	std::string getName();
	std::string listAttributes(class Descriptor* d, std::string omit);
	int getVnum();
	byte minPosition();
	float getTimer();
	bool isViolent();
	bool levels();
	bool targetSet( const std::string &target );
	bool routineSet( const std::string &routine );
	bool classSet( const std::string &className );
	bool classSet( const int iClass );
	int getPrimaryClass();
	bool isWeave();
	bool isElement();
	bool disabled();
	bool invertable();

	bool operator < (Weave *OtherWeave) {return (this->getVnum() < OtherWeave->getVnum());}

	bool hasAttribute( std::string AttributeName, bool case_sensitive=true );
	void removeAttribute( std::string AttributeName, bool case_sensitive=true );
	void addAttribute( std::string AttributeName );
};

struct WeaveCallback
{
	EVENT (*callBack);
	int callTime;
	int effect;
};

class WeaveManager
{
private:
	std::vector< Weave* > WeaveVector;
	std::map< int, WeaveCallback > DefaultCallbacks;
	static WeaveManager* Self;

	WeaveManager();
	~WeaveManager();
public:

	static WeaveManager& GetManager();
	static bool SavingRoll(Character* caster, Character* victim);
	static int PercentForSkillLevel( int skill_level );

	const int GetWeaveVnum( const std::string Name );
	const std::string GetWeaveName( const int vnum );

	Weave* GetWeave( int vnum );
	Weave* GetWeave( const std::string &Name );
	void AddWeave( Weave* w );
	void SortWeaves();
	std::vector< Weave* >::iterator RemoveWeave( const int vnum );
	static bool IsEmpty( std::string &WeaveValue );

	std::string ListWeaves( int method, class Character* ch = 0 );
	std::list< int > GetListOfWeaveVnums();
	std::list< std::pair<int, std::string> > GetListOfWeavePairs();

	bool LoadWeaves();
	bool SaveWeaves();

	void Free();
	int NumWeaves();

	void SetupCallbacks();
	void AddCallback( const int eNum, const int cTime, EVENT(*cBack));
	WeaveCallback GetCallback( const int eNum );
};

EVENT(effMainCallback);
EVENT(effPerformBurn);
EVENT(effPerformDarkPlague);

typedef std::map<std::string, std::string>::iterator WeaveAttrIter;
typedef std::list<Weave*>::iterator WeaveIter;


/*********************** Gates *************************/

struct GateNotFoundException
{
	std::string message;
	GateNotFoundException(){}
	GateNotFoundException( std::string Msg ) {message = Msg;}
	void report() { std::cout << "SYSERR<GateNotFoundException>: " << message.c_str() << std::endl; }
};

class Gate
{
	public:
		Gate()
		{
			roomA = roomB = NULL;
			creator = NULL;
			time_of_creation = 0;
		}
		Gate(class Room *room, class Room *to_room, class Character *creator);
		~Gate();
		class Room *roomA, *roomB;
		class Character *creator;

		class Room* OtherEnd( class Room* ThisEnd );

		std::list<Gate*>::iterator Close();

		class Time time_of_creation;

};

class GateManager
{
private:
	static GateManager* Self;
	std::list<Gate*> GateList;

	GateManager();
	~GateManager();
public:

	static GateManager& GetManager();

	void AddGate( Gate* gate );
	std::list<Gate*>::iterator RemoveGate( Gate* gate );

	std::list< Gate* > GetGatesInRoom( const int room_vnum );
	std::list< Gate* > GetGatesInRoom( class Room* room );
	std::list< Gate* > GetGatesByCreator( class Character* creator );

	const size_t NumberOfGates() { return GateList.size(); }
	void UpdateGates();
	void KillAllGates( Character* creator );
	void Free();
};

//This class stores a shield. Basically, a 'shield' weave channeled from one player to another.
//We store this here so we can easily cycle through the list, because shields go off after the target
//moves a certain number of rooms away from the caster.
class Shield
{
private:
	Character* Caster;
	Character* Target;
public:
	Shield() { Caster = Target = NULL; }
	Shield( Character* _Caster, Character* _Target ) { Caster = _Caster; Target = _Target; }
	~Shield() {}

	Character* GetCaster() { return Caster; }
	Character* GetTarget() { return Target; }
};

class ShieldManager
{
private:
	static ShieldManager* Self;

	std::list< Shield* > ShieldList;
	ShieldManager();
	~ShieldManager();

	std::list< Shield* >::iterator RemoveShield( std::list< Shield* >::iterator sIter );

public:
	static ShieldManager& GetManager();

	Character* ShieldedBy( Character* Target );
	Character* IsShielding( Character* Caster );

	void KillShieldsByTarget( Character* Target );
	void KillShieldsByCaster( Character* Caster );

	void AddShield( Shield* s );
	void Free();
	std::list< Shield* >::iterator RemoveShield( Shield* s );
	std::list< Shield* >::iterator RemoveShield( Character* Caster, Character* Target );
};
