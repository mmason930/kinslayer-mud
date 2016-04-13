/********************************************************************
 *	mobs.cpp - Implements the WeaveManager & Weave classes			*
 *																	*
 *	Coded by Galnor in March of 2009 (Sortak@gmail.com)				*
 *																	*
 ********************************************************************/

#include "conf.h"
#include "sysdep.h"
#include <functional>

#include "structs.h"
#include "spells.h"
#include "utils.h"
#include "interpreter.h"
#include "handler.h"
#include "comm.h"
#include "db.h"
#include "constants.h"
#include "weaves.h"
#include "clans.h"
#include "weather.h"
#include "stats.h"
#include "utils.h"
#include "screen.h"

#include "StringUtil.h"
#include "Descriptor.h"
#include "rooms/Room.h"

WeaveManager * WeaveManager::Self  = NULL;
GateManager  * GateManager::Self   = NULL;
ShieldManager* ShieldManager::Self = NULL;

/************* Class Weave *************/
Weave::Weave()
{//no defaults yet.
}

Weave::Weave( std::string _Name )
{
	Name = _Name;
}

void Weave::setAttribute( std::string Key, std::string Value )
{
	this->Attributes[ Key ] = (Value);
}
void Weave::setName( std::string NewName )
{
	this->Name = NewName;
}

std::string Weave::getAttribute( std::string Key )
{
	WeaveAttrIter iter;
	if( (iter = this->Attributes.find( Key )) != this->Attributes.end() )
		return (*iter).second;//Grab the value from the map.
	return std::string("<None>");//This value represents an empty attribute. User can use WeaveManager::IsEmpty to
								//check to see if the value is empty or not.
}

std::string Weave::getName()
{
	return Name;//Grabbed directly from the class.
}

//Return the weave's vnum from the attribute list.
int Weave::getVnum()
{
	//This is grabbed directly from the attributes.
	std::string Value = getAttribute( "Vnum" );
	return (WeaveManager::IsEmpty( Value )) ? (-1) : (atoi(Value.c_str()));
}

//This function will send all the weave data to the save file.
void Weave::addToBatchInsertStatement( sql::BatchInsertStatement &batchInsertStatement )
{
	for( WeaveAttrIter attr = Attributes.begin();attr != Attributes.end();++attr )
	{
		batchInsertStatement.beginEntry();

		batchInsertStatement.putString(getName());

		batchInsertStatement.putString((*attr).first);

		batchInsertStatement.putString((*attr).second);

		batchInsertStatement.endEntry();
	}
}

/* Return a string containing the attributes in a numbered list.
 * The 'omit' variable contains a list of attributes that will not be displayed, separated by a comma.
 * Ex: omit=std::string("Vnum, Cost") will omit those attributes from the list.
 */
std::string Weave::listAttributes(Descriptor* d=0, std::string omit="")
{
	std::stringstream Buffer;
	Buffer.str("");//clear the stream.
	std::list< std::string > OmitList;
	OmitList = StringUtil::splitToList(omit, ',');

	//Cycle through the attributes and number them in the display...
	int i = 0;
	for( WeaveAttrIter attr = Attributes.begin();attr != Attributes.end();++attr)
	{
		//We do not want to display any attributes on the omit list.
		if( std::find( OmitList.begin(), OmitList.end(), (*attr).first ) == OmitList.end() )
		{
			Buffer << std::setw(3) << std::right << (++i) << ") ";
			if( d->character ) Buffer << COLOR_GREEN(d->character, CL_COMPLETE);
			Buffer << std::left << std::setw(12) << (*attr).first;
			if( d->character ) Buffer << COLOR_NORMAL(d->character, CL_COMPLETE);
			Buffer << " : " << (*attr).second << std::endl;
		}
	}
	return Buffer.str();
}

bool Weave::hasAttribute( std::string AttributeName, bool case_sensitive )
{
	for( WeaveAttrIter aIter = Attributes.begin();aIter != Attributes.end();++aIter )
		if( (!case_sensitive && !str_cmp((*aIter).first, AttributeName)) || (case_sensitive && !(*aIter).first.compare(AttributeName)) )
			return true;
	return false;
}
void Weave::removeAttribute( std::string AttributeName, bool case_sensitive )
{
	for( WeaveAttrIter aIter = Attributes.begin();aIter != Attributes.end();++aIter )
		if( (!case_sensitive && !str_cmp((*aIter).first, AttributeName)) || (case_sensitive && !(*aIter).first.compare(AttributeName)) )
		{
			Attributes.erase( aIter );
			return;
		}
}
void Weave::addAttribute( std::string AttributeName )
{
	Attributes[AttributeName] = "<None>";
}

byte Weave::minPosition()
{
	std::string sPos =  this->getAttribute("Min Position");
		 if( sPos == "Flying"   )		return POS_FLYING;
	else if( sPos == "Standing" )		return POS_STANDING;
	else if( sPos == "Fighting" )		return POS_FIGHTING;
	else if( sPos == "Sitting"  )		return POS_SITTING;
	else if( sPos == "Resting"	)		return POS_RESTING;
	else if( sPos == "Sleeping"	)		return POS_SLEEPING;
	else if( sPos == "Stunned"	)		return POS_STUNNED;
	else if( sPos == "Incapacitated")	return POS_INCAP;
	else if( sPos == "Mortally Wounded")return POS_MORTALLYW;
	else if( sPos == "Dead"		)		return POS_DEAD;
	MudLog(BRF, LVL_APPR, TRUE, "SYSERR: Weave #%d has invalid position state: '%s'.", this->getVnum(), sPos.c_str());
	return -1;
}
float Weave::getTimer()
{
	std::string sTimer = this->getAttribute("Timer");

	if( !sTimer.compare("<None>") )
	{
		MudLog(BRF, LVL_APPR, TRUE, "SYSERR: Weave #%d has no timer set.", getVnum());
		return 0;
	}
	return (float)(atof(sTimer.c_str()));
}
bool Weave::isViolent()
{
	std::string sViolent = this->getAttribute("Violent");
	if( sViolent == "Yes" ) return true;
	else if( sViolent == "No" ) return false;
	MudLog(BRF, LVL_APPR, TRUE, "SYSERR: Weave #%d has invalid value set for the 'Violent' attribute.", getVnum());
	return false;
}
bool Weave::disabled()
{
	return ( this->getAttribute("Disabled") == "Yes" );
}

bool Weave::invertable()
{
	return ( this->getAttribute("invertable") == "Yes" );
}

bool Weave::levels()
{
	std::string sLevels = this->getAttribute("Levels");
	if( sLevels == "Yes" ) return true;
	else if( sLevels == "No" ) return false;
	MudLog(BRF, LVL_APPR, TRUE, "SYSERR: Weave #%d has invalid value set for the 'Levels' attribute.", getVnum());
	return false;
}
bool Weave::targetSet( const std::string &target )
{
	std::string sTargets = this->getAttribute("Targeting");
	std::list<std::string> lTargets = StringUtil::splitToContainer< std::list<std::string> >( sTargets, ',' );
	for( std::list< std::string >::iterator sChunk = lTargets.begin();sChunk != lTargets.end();++sChunk )
	{
		if( eatwhite((*sChunk)) == target ) return true;
	}
	return false;
}

bool Weave::routineSet( const std::string &routine )
{
	std::string sRoutines = this->getAttribute("Routines");
	std::list<std::string> lRoutines = StringUtil::splitToContainer< std::list<std::string> >( sRoutines, ',' );
	for( std::list< std::string >::iterator sChunk = lRoutines.begin();sChunk != lRoutines.end();++sChunk )
	{
		if( (*sChunk) == routine ) return true;
	}
	return false;
}

bool Weave::classSet( const std::string &className )
{
	std::string sClasses = this->getAttribute("Class");
	std::list<std::string> lClasses = StringUtil::splitToContainer< std::list<std::string> >( sClasses, ' ' );
	for( std::list< std::string >::iterator sChunk = lClasses.begin();sChunk != lClasses.end();++sChunk )
	{
		if( (*sChunk) == className ) return true;
		if( (*sChunk) == "All" ) return true;
	}
	return false;
}
bool Weave::classSet( const int iClass )
{
	if( iClass < 0 || iClass >= NUM_CLASSES ) return false;
	std::string sClass = StringUtil::cap(StringUtil::allLower( class_types[iClass] ));
	return classSet( sClass );
}
int Weave::getPrimaryClass()
{
	std::string sClasses = this->getAttribute("Class");
	std::list<std::string> lClasses = StringUtil::splitToContainer< std::list<std::string> >( sClasses, ' ' );
	for( std::list< std::string >::iterator sChunk = lClasses.begin();sChunk != lClasses.end();++sChunk )
	{
		for(int i = 0;i < NUM_CLASSES;++i)
		{
			if( !str_cmp( (*sChunk), class_types[i] ) )
				return i;
		}
	}
	return -1;
}
bool Weave::isWeave()
{
	return (getAttribute("Cost").compare("<None>"));
}
bool Weave::isElement()
{
	return (getName()=="Fire" || getName()=="Earth" || getName()=="Water" || getName()=="Air" || getName()=="Spirit");
}

/****************** Weave Manager ******************/

WeaveManager::WeaveManager()
{
	SetupCallbacks();
}
WeaveManager::~WeaveManager()
{
	for( std::vector< Weave * >::iterator weaveIter = WeaveVector.begin();weaveIter != WeaveVector.end();++weaveIter )
	{
		delete ( (*weaveIter) );
	}
	Self = NULL;
}
void WeaveManager::Free()
{
	if( Self != NULL )
		delete (Self);
	Self = NULL;
}
int WeaveManager::NumWeaves()
{
	return (int)WeaveVector.size();
}

WeaveManager& WeaveManager::GetManager()
{
	if( !Self )
		Self = new WeaveManager();
	return (*Self);
}
/*** Galnor 12/29/2009 - This method will initialize all effect based callbacks.
     These are only defaults. They can be overridden by the affected_type's callTime and callBack
	 properties. If they are set when they get passed to affect_to_char(), then they will be used
	 rather than these defaults.
***/
void WeaveManager::SetupCallbacks()
{
	AddCallback( AFF_BURN, PULSE_VIOLENCE, effPerformBurn );
	AddCallback( AFF_DARK_PLAGUE, PULSE_VIOLENCE * 3, effPerformDarkPlague );
	//...
}
/*** 12/29/2009 - Add a single default callback to the map. ***/
void WeaveManager::AddCallback( const int eNum, const int cTime, EVENT(*cBack))
{
	WeaveCallback wc;
	wc.callBack = cBack;
	wc.callTime = cTime;
	wc.effect = eNum;
	this->DefaultCallbacks[ eNum ] = wc;
}
/*** 12/29/2009 - Get the callback data for a given effect number ***/
WeaveCallback WeaveManager::GetCallback( const int eNum )
{
	if( DefaultCallbacks.count( eNum ) != 0 )
	{
		return DefaultCallbacks[ eNum ];
	}
	WeaveCallback wc;
	memset( &wc, 0, sizeof( WeaveCallback) );
	return wc;
}

bool WeaveManager::SavingRoll(Character* caster, Character* victim)
{
	int roll = MiscUtil::random(1, 100) + (victim->player.level - caster->player.level);

	return ( roll >= 100 );
}

int WeaveManager::PercentForSkillLevel( int skill_level )
{
	if( skill_level == 0 ) return 0;
	if( skill_level < 0 || skill_level > 8 )
		return (-1);
	else
		return (12*skill_level);
}

Weave* WeaveManager::GetWeave( int vnum )
{
	unsigned int weaveVectorSize = WeaveVector.size();
	if( weaveVectorSize == 0 )
		return NULL;

	unsigned int bot = 0, top = weaveVectorSize-1, mid;
	int weaveVnum;

	while( true )
	{
		mid = ((bot + top) / 2);
		weaveVnum = WeaveVector[mid]->getVnum();
		if( vnum == weaveVnum )
			return WeaveVector[ mid ];
		if( bot >= top )
			return NULL;
		if( weaveVnum > vnum )
			top = mid - 1;
		else
			bot = mid + 1;
	}
	return NULL;
}
Weave* WeaveManager::GetWeave( const std::string &Name )
{
	for( std::vector<Weave*>::iterator weaveIter = WeaveVector.begin();weaveIter != WeaveVector.end();++weaveIter )
	{
		if( !str_cmp((*weaveIter)->getName(), Name) )//We simply compare to the name stored in the weave.
			return (*weaveIter);
	}
	return 0;//No weave with this name has been found.
}

std::pair< std::string, std::string > Weave::GetNthAttribute(int starting)
{
	WeaveAttrIter Iter;
	if( --starting < 0 )
		Iter = Attributes.end();
	else
	{
		for(
			Iter = Attributes.begin();
			(Iter != Attributes.end() && starting > 0) || (Iter != Attributes.end() && !(*Iter).first.compare("Vnum"));
			--starting, ++Iter
		)
		{
			if( !(*Iter).first.compare("Vnum") )
				++starting;//We skip over this one.
		}
	}
	if( Iter == Attributes.end() )
		return std::pair<std::string, std::string>("Invalid", "Invalid");
	return (*Iter);
}

void WeaveManager::SortWeaves()
{
	std::sort(WeaveVector.begin(), WeaveVector.end(), [](Weave *a, Weave *b) { return (a->getVnum() < b->getVnum()); });
}

void WeaveManager::AddWeave( Weave* w )
{
	if( w == NULL ) return;

	if(w->getVnum())
	for(std::vector<Weave*>::iterator weaveIter = WeaveVector.begin();weaveIter != WeaveVector.end();++weaveIter)
	{
		if( w->getVnum() < (*weaveIter)->getVnum() ) {
			WeaveVector.insert(weaveIter,w);
			return;
		}
	}
	WeaveVector.push_back( w );
}
std::vector< Weave* >::iterator WeaveManager::RemoveWeave( const int vnum )
{
	for(std::vector<Weave*>::iterator weaveIter = WeaveVector.begin();weaveIter != WeaveVector.end();++weaveIter) {
		if( (*weaveIter)->getVnum() == (vnum) ) {
			delete (*weaveIter);//Free
			return WeaveVector.erase( weaveIter );//Remove & return next in list.
		}
	}
	return (WeaveVector.end());//Not found.
}

bool WeaveManager::IsEmpty( std::string &WeaveValue )
{
	return !WeaveValue.compare("<None>");
}

bool WeaveManager::LoadWeaves()
{

	std::stringstream sql;

	sql << "SELECT * FROM weaveAttribute ORDER BY name";

	sql::Query query = gameDatabase->sendQuery(sql.str());

	Weave *weave = NULL;;

	while(query->hasNextRow()) {

		sql::Row row = query->getRow();

		if(weave == NULL || weave->getName() != row.getString("name")) {

			weave = new Weave();

			weave->setName(row.getString("name"));

			WeaveManager::AddWeave(weave);
		}

		weave->setAttribute(row.getString("attribute_name"), row.getString("attribute_value"));
	}

	WeaveManager::SortWeaves();
	return true;
}

bool WeaveManager::saveWeaves()
{

	gameDatabase->sendRawQuery("DROP TABLE IF EXISTS tempWeaveAttribute");

	gameDatabase->sendRawQuery("CREATE TABLE tempWeaveAttribute LIKE weaveAttribute");

	sql::BatchInsertStatement batchInsertStatement(gameDatabase, "tempWeaveAttribute", 10000);

	batchInsertStatement.addField("name");

	batchInsertStatement.addField("attribute_name");

	batchInsertStatement.addField("attribute_value");

	batchInsertStatement.start();

	for( std::vector<Weave*>::iterator iter = WeaveVector.begin();iter != WeaveVector.end();++iter ) {

		Weave *weave = (*iter);

		weave->addToBatchInsertStatement(batchInsertStatement);
	}

	batchInsertStatement.finish();

	gameDatabase->sendRawQuery("RENAME TABLE weaveAttribute TO weaveAttributeOld, tempWeaveAttribute TO weaveAttribute, weaveAttributeOld TO tempWeaveAttribute");

	gameDatabase->sendRawQuery("DROP TABLE tempWeaveAttribute");

	return true;
}

/* Return a list of weaves. Colors are set if ch both is defined and has the correct preferences set */
std::string WeaveManager::ListWeaves( int method, Character* ch )
{
	static std::stringstream Buffer;
	Buffer.str("");

	if( method == 0 )
	{
		std::vector< Weave* > WVector;

		WVector.resize( WeaveVector.size() );
		if( !WeaveVector.empty() )
			std::copy(WeaveVector.begin(), WeaveVector.end(), WVector.begin());

		const int segments = 3;
		if( segments == 0 ) return "Invalid";//Wtf?
		int wSize = WVector.size();
		while( (wSize % segments) )
			++wSize;
		for( unsigned int i = 0;i < (wSize/segments+1);++i )
		{
			for(unsigned int n = 0;n < segments;++n)
			{
				unsigned int pos = i + (WVector.size()/segments)*n;
				if( pos < WVector.size() )
				{
					if(ch) Buffer << COLOR_GREEN(ch, CL_COMPLETE);
					Buffer << std::setw(3) << std::right << WVector[pos]->getVnum();
					if(ch) Buffer << COLOR_NORMAL(ch, CL_COMPLETE);
					Buffer << ") " << (std::setw(20)) << std::left << WVector[pos]->getName();
				}
			}
			Buffer << std::endl;
		}
	}
	else if(method == 1)
	{
		int i = 0;
		for (std::vector<Weave*>::iterator weaveIter = WeaveVector.begin();weaveIter != WeaveVector.end();++weaveIter, ++i)
		{
			Buffer << std::setw(18) << std::left << (*weaveIter)->getName();

			if (i % 4 == 3)
				Buffer << std::endl;
		}
	}
	else if(method == 2)
	{
		Buffer << "                        A       W       S       F       E" << std::endl;
		Buffer << "---------------------------------------------------------" << std::endl;
		for (std::vector<Weave*>::iterator weaveIter = WeaveVector.begin();weaveIter != WeaveVector.end();++weaveIter)
		{
			if( !(*weaveIter)->isWeave() || (*weaveIter)->isElement() || (*weaveIter)->disabled() )
				continue;
			Buffer << std::setw(24) << std::left << (*weaveIter)->getName() << (*weaveIter)->getAttribute("Air") << "       " <<
				(*weaveIter)->getAttribute("Water") << "       " << (*weaveIter)->getAttribute("Spirit") << "       " <<
				(*weaveIter)->getAttribute("Fire") << "       " << (*weaveIter)->getAttribute("Earth");

			Buffer << std::endl;
		}
	}
	return Buffer.str();
}

std::list< int > WeaveManager::GetListOfWeaveVnums()
{
	std::list<int> Vnums;
	for( std::vector<Weave*>::iterator weaveIter = this->WeaveVector.begin();weaveIter != WeaveVector.end();++weaveIter )
	{
		Vnums.push_back( (*weaveIter)->getVnum() );
	}
	return Vnums;
}
std::list< std::pair<int, std::string> > WeaveManager::GetListOfWeavePairs()
{
	std::list< std::pair<int, std::string> > lPairs;
	for( std::vector<Weave*>::iterator weaveIter = WeaveVector.begin();weaveIter != WeaveVector.end();++weaveIter )
	{
		lPairs.push_back( std::pair<int, std::string>( (*weaveIter)->getVnum(), (*weaveIter)->getName() ) );
	}
	return lPairs;
}

const int WeaveManager::GetWeaveVnum( const std::string Name )
{
	for(std::vector<Weave*>::iterator weaveIter = WeaveVector.begin();weaveIter != WeaveVector.end();++weaveIter)
	{
		if( !str_cmp(Name, (*weaveIter)->getName() ) )
			return (*weaveIter)->getVnum();
	}
	return -1;
}
const std::string WeaveManager::GetWeaveName( const int vnum )
{
	Weave* w;
	if( (w = GetWeave(vnum)) == NULL )
		return std::string("!UNUSED!");
	return w->getName();
}

/********************** Gates *************************/

GateManager::GateManager() {}
GateManager::~GateManager()
{
	for(std::list<Gate*>::iterator gIter = GateList.begin();gIter != GateList.end();++gIter)
	{
		delete (*gIter);
		gIter = GateList.erase(gIter);
	}
	Self = NULL;
}

GateManager& GateManager::GetManager()
{
	if( !Self ) Self = new GateManager();
	return (*Self);
}

void GateManager::AddGate( Gate* gate )
{
	//Invalid. Gate must exist in order to add.
	if( !gate ) return;
	GateList.push_back( gate );
}

std::list<Gate*>::iterator GateManager::RemoveGate( Gate* gate )
{
	for(std::list<Gate*>::iterator gIter = GateList.begin();gIter != GateList.end();++gIter)
	{
		if( (*gIter) == gate )
			return (GateList.erase( gIter ));
	}
	throw GateNotFoundException();
}

void GateManager::UpdateGates()
{
	std::list<Gate *>::iterator g;

	if ( !NumberOfGates() )
		return;

	for ( g = GateList.begin();g != GateList.end(); )
	{
		if( GET_LEVEL((*g)->creator) >= LVL_IMMORT )
		{
			++g;//Must iterate outside of loop header
			continue;
		}
		GET_MANA((*g)->creator) = MAX(0, GET_MANA((*g)->creator) - 2);
		GET_MOVE((*g)->creator) = MAX(0, GET_MOVE((*g)->creator) - 2);
		if( GET_MOVE( (*g)->creator ) == 0 || GET_MANA( (*g)->creator ) <= 0 )
		{
			if ( (*g)->creator )
				(*g)->creator->send( "You can't hold open your gate any longer!\r\n" );

			try
			{
				Gate* gate = (*g);
				g = (*g)->Close();
				delete gate;
			}
			catch(GateNotFoundException e)
			{
				e.report();
			}
		}
		else
			++g;
	}
}

std::list< Gate* > GateManager::GetGatesgetRoom( const int room_vnum )
{
	int room_rnum = real_room(room_vnum);
	if( room_rnum == -1 )
	{
		std::stringstream Buffer;
		Buffer << "Attempting to grab gates in room vnum " << room_vnum << ".";
		throw RoomNotFoundException( Buffer.str() );
	}
	return GetGatesgetRoom( World[room_rnum] );

}

std::list< Gate* > GateManager::GetGatesgetRoom( class Room* room )
{
	std::list<Gate*> TempGateList;

	for(std::list<Gate*>::iterator gIter = GateList.begin();gIter != GateList.end();++gIter)
	{
		if( (*gIter)->roomA == room || (*gIter)->roomB == room )
			TempGateList.push_back( (*gIter) );
	}
	return TempGateList;
}

std::list< Gate* > GateManager::GetGatesByCreator( class Character* creator )
{
	std::list<Gate*> TempGateList;

	for(std::list<Gate*>::iterator gIter = GateList.begin();gIter != GateList.end();++gIter)
	{
		if( (*gIter)->creator == creator )
			TempGateList.push_back( (*gIter) );
	}
	return TempGateList;
}

void GateManager::KillAllGates( Character* creator )
{
	std::list<Gate *>::iterator g;

	for(g = GateList.begin();g != GateList.end();)
	{
		if((*g)->creator == creator)
		{
			try
			{
				Gate* gate = (*g);
				g = (*g)->Close();
				delete gate;
			}
			catch( GateNotFoundException e )
			{
				e.report();
				break;
			}
		}
		else
			g++;//Iterate. No gate was removed.
	}
}

void GateManager::Free()
{
	if( Self )
		delete Self;
}





Gate::~Gate()
{
}

std::list<Gate*>::iterator Gate::Close()
{
	sendToRoom("A gate slowly fades out of existance.\r\n", roomA);
	sendToRoom("A gate slowly fades out of existance.\r\n", roomB);
	return GateManager::GetManager().RemoveGate( this );
}

Gate::Gate(Room *room, Room *to_room, Character *creator)
{
	this->roomA = room;
	this->roomB = to_room;
	this->creator = creator;
}

Room* Gate::OtherEnd( Room* ThisEnd )
{
	if( ThisEnd == this->roomA )
		return roomB;
	if( ThisEnd == this->roomB )
		return roomA;
	std::stringstream Buffer;
	Buffer << "Gate::OtherEnd passed room with vnum " << ThisEnd->getVnum() << ".";
	throw RoomNotFoundException(Buffer.str());
}

ShieldManager::ShieldManager()
{
	Self = NULL;
}
ShieldManager::~ShieldManager()
{
	for( std::list< Shield* >::iterator sIter = ShieldList.begin();sIter != ShieldList.end();++sIter )
	{
		delete (*sIter);
		sIter = ShieldList.erase( sIter );
	}
	Self = NULL;
}


ShieldManager& ShieldManager::GetManager()
{
	if( !Self ) Self = new ShieldManager();
	return (*Self);
}

Character* ShieldManager::ShieldedBy( Character* Target )
{
	for( std::list< Shield* >::iterator sIter = ShieldList.begin();sIter != ShieldList.end();++sIter )
	{
		if( (*sIter)->GetTarget() == Target )
			return (*sIter)->GetCaster();
	}
	return NULL;
}
Character* ShieldManager::IsShielding( Character* Caster )
{
	for( std::list< Shield* >::iterator sIter = ShieldList.begin();sIter != ShieldList.end();++sIter )
	{
		if( (*sIter)->GetCaster() == Caster )
			return (*sIter)->GetTarget();
	}
	return NULL;
}

void ShieldManager::KillShieldsByTarget( Character* Target )
{
	for( std::list< Shield* >::iterator sIter = ShieldList.begin();sIter != ShieldList.end(); )
	{
		if( (*sIter)->GetTarget() == Target )
			sIter = RemoveShield( sIter );
		else
			++sIter;
	}
}
void ShieldManager::KillShieldsByCaster( Character* Caster )
{
	for( std::list< Shield* >::iterator sIter = ShieldList.begin();sIter != ShieldList.end(); )
	{
		if( (*sIter)->GetCaster() == Caster )
			sIter = RemoveShield( sIter );
		else
			++sIter;
	}
}

void ShieldManager::AddShield( Shield* s )
{
	ShieldList.push_back( s );
}


std::list< Shield* >::iterator ShieldManager::RemoveShield( Character* Caster, Character* Target )
{
	for( std::list< Shield* >::iterator sIter = ShieldList.begin();sIter != ShieldList.end(); )
	{
		if( (*sIter)->GetCaster() == Caster && (*sIter)->GetTarget() == Target )
			return RemoveShield( sIter );
	}
	return ShieldList.end();
}

std::list< Shield* >::iterator ShieldManager::RemoveShield( Shield* s )
{
	for( std::list< Shield* >::iterator sIter = ShieldList.begin();sIter != ShieldList.end();++sIter )
	{
		if( (*sIter) == s )
			return RemoveShield( sIter );
	}
	return ShieldList.end();
}

std::list< Shield* >::iterator ShieldManager::RemoveShield( std::list< Shield* >::iterator sIter )
{
	(*sIter)->GetCaster()->send("Your shield against %s has dissipated.\r\n", (*sIter)->GetTarget()->player.name.c_str());
	(*sIter)->GetTarget()->send("The shield blocking you from the True Source has dissipated.\r\n");

	delete (*sIter);
	return ShieldList.erase( sIter );
}
void ShieldManager::Free()
{
	if( Self )
		delete Self;
	Self = NULL;
}


/************** MISC ***************/

bool Character::ShieldOutOfRange( Character* Target )
{
	Weave* weave = WeaveManager::GetManager().GetWeave("Shield");
	if( !weave ) return false;

	double distance_required = atof(weave->getAttribute("SkillFactor").c_str()) * (GET_SKILL(this, weave->getVnum()));

	if (this->in_room->getDistanceToRoom(Target->in_room) >= (int)distance_required)
		return true;
	return false;
}

bool Character::CanTeach( Weave *w )
{
	if( (w) == (NULL) ) return (false);

	return (
		(MOB_FLAGGED(this, MOB_TRAINER)       && w->classSet(GET_CLASS(this))) ||
		(MOB_FLAGGED(this, MOB_TRADE_TRAINER) && w->classSet("Trade"))
	);
}

void Character::setSkill(short int skillId, int percent)
{
	if(percent <= 0)
	{
		auto skillIter = skills.find(skillId);
		if(skillIter != skills.end())
			skills.erase(skillIter);
	}

	PlayerSkill playerSkill;
	playerSkill.setPercent(percent);
	playerSkill.setSKillId(skillId);
	skills[skillId] = playerSkill;
}

int Character::getSkill(short int skillId)
{
	auto skillIter = skills.find(skillId);
	if(skillIter != skills.end())
		return (*skillIter).second.getPercent();
	return 0;
}

bool Character::hasSkill(short int skillId)
{
	auto skillIter = skills.find(skillId);
	return (skillIter != skills.end());
}
