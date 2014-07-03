//
// C++ Interface: JSCharacter
//
// Description:
//
//
// Author: David Capel <wot.narg@gmail.com>, (C) 2009
//

#ifndef KINSLAYER_JSCHARACTER_H
#define KINSLAYER_JSCHARACTER_H

#include <flusspferd.hpp>
#include <string>
#include <iostream>

#include "structs.h"
#include "interpreter.h"
#include "js_utils.h"
#include "utils.h"
#include "clans.h"


class JSRoom;
class JSObject;

using namespace std;

#define SET_OR_REMOVE_JS(flagset, flags) { \
	if (true) SET_BIT_AR(flagset, flags); \
	else if (false) REMOVE_BIT_AR(flagset, flags); }

FLUSSPFERD_CLASS_DESCRIPTION(
    JSCharacter,
    (full_name, "JSCharacter")
    (constructor_name, "JSCharacter")
    (methods,
		("send", bind, send)
		("say", bind, say)
		("damage", bind, damage)
		("eq", bind, eq)
		("getCharRoomVis", bind, js_get_char_room_vis)
		("getCharVis", bind, js_get_char_vis)
		("lag", bind, lag)
		("startTimer", bind, start_timer)
		("comm", bind, comm)
		("loadObj", bind, load_obj)
		("extract", bind, extract)
		("moveToRoom", bind, move_to_room)
		("inClan", bind, in_clan)
		("qval", bind, qval)
		("quest", bind, getQuest)
		("qp", bind, getQP)
		("yellow", bind, getYellow)
		("green", bind, getGreen)
		("cyan", bind, getCyan)
		("red", bind, getRed)
		("magenta", bind, getMagenta)
		("blue", bind, getBlue)
		("bold", bind, getBold)
		("normal", bind, getNormal)
		("mobFlagged", bind, getMobFlagged)
		("getSkill", bind, getSkill)
		("setSkill", bind, setSkill)
		("attach", bind, attach)
		("detach", bind, detach)
		("countJS", bind, countJS)
		("wantedBy", bind, wantedBy)
		("canSee", bind, canSee)
		("affect", bind, affect)
		("unaffect", bind, unaffect)
		("affectedBy", bind, affectedBy)
		("warrantByClan", bind, warrantByClan)
		("pardonByClan", bind, pardonByClan)
		("getRank", bind, getRank)
		("isCouncil", bind, getIsCouncil)
		("hasSource", bind, getHasSource)
		("resetSkills", bind, resetSkills)
		("resetTrades", bind, resetTrades)
		("resetSpells", bind, resetSpells)
		("tell", bind, tell)
		("setFighting", bind, setFighting)
		("stopFighting", bind, stopFighting)
		("removeSource", bind, removeSource)
        )
    (properties,
        //read only properties
        ("name", getter, getName)
		("title", getter, getTitle)
        ("sex", getter, getSex)
        ("class", getter, getClass)
        ("race", getter, getRace)
        ("weight", getter, getWeight)
        ("height", getter, getHeight)
        ("carryWeight", getter, getCarry_Weight)
		("wornWeight", getter, getWorn_Weight)
        ("parry", getter, getParry)
        ("dodge", getter, getDodge)
        ("absorb", getter, getAbsorb)
        ("offense", getter, getOffense)
        ("weavePoints", getter, getWeave_Points)
		("vnum", getter, getVnum)
        ("level", getter, getLevel)
		("inventory", getter, inventory)
		("room", getter, room)
		("leader", getter, getLeader)
		("followers", getter, getFollowers)
		("id", getter, getID)
		("namelist", getter, getNameList)
		("description", getter, getDescription)
		("longDesc", getter, getLongDesc)
		("isValid", getter, getIsValid)
		("arriveMessage", getter, getArriveMessage)
		("leaveMessage", getter, getLeaveMessage)
		("shieldBlock", getter, getShieldBlock)
		("maxCarryItems", getter, getMaxCarryItems)
		("maxCarryWeight", getter, getMaxCarryWeight)
		("aliases", getter, getAliases)
		("expToLevel", getter, getExperienceToLevel)
		("legend", getter, getLegend)
		("playingDays", getter, getPlayingDays)
		("playingHours", getter, getPlayingHours)
		("playerDeaths", getter, getPlayerDeaths)
		("mobDeaths", getter, getMobDeaths)
		("idle", getter, getIdle)
		("linkless", getter, getLinkless)
		("age", getter, getAge)
		("createdDatetime", getter, getCreatedDatetime)

        // read-write properties
	("invis", getter_setter, (getInvis, setInvis))
        ("skillPractices", getter_setter, (getSkill_Practices, setSkill_Practices))
        ("spellPractices", getter_setter, (getSpell_Practices, setSpell_Practices))
        ("tradePractices", getter_setter, (getTrade_Practices, setTrade_Practices))
        ("position", getter_setter, (getPosition, setPosition))
        ("sps", getter_setter, (getSps, setSps))
        ("maxSpell", getter_setter, (getMax_Sps, setMax_Sps))
        ("hps", getter_setter, (getHps, setHps))
        ("maxHit", getter_setter, (getMax_Hps, setMax_Hps))
        ("mvs", getter_setter, (getMoves, setMoves))
        ("maxMove", getter_setter, (getMax_Moves, setMax_Moves))
        ("taint", getter_setter, (getTaint, setTaint))
        ("shadowPoints", getter_setter, (getShadow_Points, setShadow_Points))
        ("maxShadow", getter_setter, (getMax_Shadow_Points, setMax_Shadow_Points))
        ("strain", getter_setter, (getStrain, setStrain))
        ("gold", getter_setter, (getGold, setGold))
        ("bankGold", getter_setter, (getBank_Gold, setBank_Gold))
        ("experience", getter_setter, (getExperience, setExperience))
        ("damRollMod", getter_setter, (getDamroll_Mod, setDamroll_Mod))
        ("mood", getter_setter, (getMood, setMood))
        ("stance", getter_setter, (getStance, setStance))
        ("strength", getter_setter, (getStrength, setStrength))
        ("intelligence", getter_setter, (getIntelligence, setIntelligence))
        ("wisdom", getter_setter, (getWisdom, setWisdom))
        ("dexterity", getter_setter, (getDexterity, setDexterity))
        ("constitution", getter_setter, (getConstitution, setConstitution))
		("fighting", getter_setter, (getFighting, setFightingVar))
		("mount", getter_setter, (getMount, setMount))
		("riddenBy", getter_setter, (getRiddenBy, setRiddenBy))
		("hunger", getter_setter, (getHunger, setHunger))
		("thirst", getter_setter, (getThirst, setThirst))
		("drunk", getter_setter, (getDrunk, setDrunk))
		("wimpy", getter_setter, (getWimpy, setWimpy))
		("target", getter_setter, (getTarget, setTarget))
		("marked", getter_setter, (getMarked, setMarked))
		("hunting", getter_setter, (getHunting, setHunting))
		("otarget", getter_setter, (getOTarget, setOTarget))
		("sittingOn", getter_setter, (getSittingOn, setSittingOn))
		("deathWait", getter_setter, (getDeathWait, setDeathWait))
		("warnings", getter_setter, (getWarnings, setWarnings))
		("masterWeapon", getter_setter, (getMasterWeapon, setMasterWeapon))
		("isBashed", getter_setter, (getIsBashed, setIsBashed))
		("decayedBy", getter_setter, (getDecayedBy, setDecayedBy))
		("burnedBy", getter_setter, (getBurnedBy, setBurnedBy))
		("plaguedBy", getter_setter, (getPlaguedBy, setPlaguedBy))
        ))
{
public:
	static int numberAllocated;
	static int numberDeallocated;
	JSCharacter(flusspferd::object const &self, flusspferd::call_context& cc)
		: base_type(self)
	{
		++numberAllocated;
//		std::cout << "Creating JSCharacter: Real: -" << std::endl;
	}

	~JSCharacter() {
		++numberDeallocated;
//		std::cout << "Destroying JSCharacter" << std::endl;
	}

	JSCharacter(flusspferd::object const &self, Character* _real)
		: base_type(self)
	{
		++numberAllocated;
		real = _real;
//		std::cout << "Creating JSCharacter: Real: " << real << std::endl;
	}

	Character* toReal() { return real; }
	void setReal( Character *c ) { real = c; }

    void send(std::string message);
	void damage(int dmg, flusspferd::value attacker);
    void say(flusspferd::string msg);
    void tell(JSCharacter *vict, flusspferd::string msg);
	void move_to_room( JSRoom *r );
	void resetSkills() { if( !real || real->IsPurged() ) return; real->ResetSkills(); }
	void resetSpells() { if( !real || real->IsPurged() ) return; real->ResetSpells(); }
	void resetTrades() { if( !real || real->IsPurged() ) return; real->ResetTrades(); }
    flusspferd::value eq(int pos);

	
	flusspferd::object getCreatedDatetime();
	flusspferd::string getName() { return ( (real && !real->IsPurged()) ? GET_NAME(real) : "Invalid" ); }
	flusspferd::string getNameList()
	{
		if( !real || real->IsPurged() )
			return "Invalid";
		return real->player.name;
	}
	flusspferd::string getLongDesc()
	{
		if( !real || real->IsPurged() ) return "Invalid";
		return real->player.long_descr;
	}
	flusspferd::string getDescription()
	{
		if( !real || real->IsPurged() ) return "Invalid";
		if( real->player.description ) return "";
		return real->player.description;
	}
	flusspferd::string getTitle() { return ( (real && !real->IsPurged()) ? real->player.title : "Invalid" ); }
    int getSex()				{ return ( (real && !real->IsPurged()) ? real->player.sex : 0 ); }
    int getClass()				{ return ( (real && !real->IsPurged()) ? real->player.chclass : 0 ); }
    int getRace()				{ return ( (real && !real->IsPurged()) ? real->player.race : 0 ); }
    int getLevel()				{ return ( (real && !real->IsPurged()) ? real->player.level : 0 ); }
    
    int getPosition()			{ return ( (real && !real->IsPurged()) ? real->player.position : 0 ); }
    double getWeight()			{ return ( (real && !real->IsPurged()) ? real->player.weight : 0 ); }
    double getHeight()			{ return ( (real && !real->IsPurged()) ? real->player.height : 0 ); }
    double getCarry_Weight()	{ return ( (real && !real->IsPurged()) ? real->player.carry_weight : 0 ); }
    double getWorn_Weight()		{ return ( (real && !real->IsPurged()) ? real->WornWeight() : 0 ); }
    int getParry()				{ return ( (real && !real->IsPurged()) ? real->Parry() : 0 ); }
    int getDodge()				{ return ( (real && !real->IsPurged()) ? real->Dodge() : 0 ); }
    int getAbsorb()				{ return ( (real && !real->IsPurged()) ? real->Absorb() : 0 ); }
    int getOffense()			{ return ( (real && !real->IsPurged()) ? real->Offense() : 0 ); }
    int getWeave_Points()		{ return ( (real && !real->IsPurged()) ? real->points.weave : 0 ); }
	int getVnum()				{ return ( (real && !real->IsPurged() && IS_NPC(real)) ? (real->getVnum()) : (-1) ); }
    int getSkill_Practices()	{ return ( (real && !real->IsPurged()) ? real->PlayerData->skillpracs : 0 ); }
	int getSpell_Practices()	{ return ( (real && !real->IsPurged()) ? real->PlayerData->spellpracs : 0 ); }
	int getTrade_Practices()	{ return ( (real && !real->IsPurged()) ? real->PlayerData->tradepracs : 0 ); }
    
    int getSps()				{ if( !real || real->IsPurged() ) return 0; return real->points.mana; }
    int getMax_Sps()			{ if( !real || real->IsPurged() ) return 0; return real->points.max_mana; }
    int getHps()				{ if( !real || real->IsPurged() ) return 0; return real->points.hit; }
    int getMax_Hps()			{ if( !real || real->IsPurged() ) return 0; return real->points.max_hit; }
    int getMoves()				{ if( !real || real->IsPurged() ) return 0; return real->points.move; }
    int getMax_Moves()			{ if( !real || real->IsPurged() ) return 0; return real->points.max_move; }
    int getTaint()				{ if( !real || real->IsPurged() ) return 0; return real->points.taint; }
    int getShadow_Points()		{ if( !real || real->IsPurged() ) return 0; return real->points.shadow_points; }
    int getMax_Shadow_Points()	{ if( !real || real->IsPurged() ) return 0; return real->points.max_shadow_points; }
    int getStrain()				{ if( !real || real->IsPurged() ) return 0; return real->points.strain; }
    int getGold()				{ if( !real || real->IsPurged() ) return 0; return real->points.gold; }
    int getBank_Gold()			{ if( !real || real->IsPurged() ) return 0; return real->points.bank_gold; }
    int getExperience()			{ if( !real || real->IsPurged() ) return 0; return real->points.exp; }
    int getDamroll_Mod()		{ if( !real || real->IsPurged() ) return 0; return real->points.damroll; }
    int getMood()				{ if( !real || real->IsPurged() ) return 0; return real->PlayerData->mood; }
    int getStance()				{ if( !real || real->IsPurged() ) return 0; return real->stance; }
    int getStrength()			{ if( !real || real->IsPurged() ) return 0; return real->aff_abils.str; }
    int getIntelligence()		{ if( !real || real->IsPurged() ) return 0; return real->aff_abils.intel; }
    int getWisdom()				{ if( !real || real->IsPurged() ) return 0; return real->aff_abils.wis; }
    int getDexterity()			{ if( !real || real->IsPurged() ) return 0; return real->aff_abils.dex; }
    int getConstitution()		{ if( !real || real->IsPurged() ) return 0; return real->aff_abils.con; }
	int getID()					{ if( !real || real->IsPurged() ) return -1; return real->player.idnum; }
	bool getIsValid()			{ if( !real || real->IsPurged() ) return false; return true; }

    void setSps(int x)				{ if( !real || real->IsPurged() ) return; real->points.mana = INTERVAL(0, x, real->points.max_mana); }
    void setMax_Sps(int x)			{ if( !real || real->IsPurged() ) return; real->points.max_mana = MAX(0, x); }
    void setHps(int x);
    void setMax_Hps(int x)			{ if( !real || real->IsPurged() ) return; real->points.max_hit = MAX(0,x); }
    void setMoves(int x)			{ if( !real || real->IsPurged() ) return; real->points.move = INTERVAL(0, x, real->points.max_move); }
    void setMax_Moves(int x)		{ if( !real || real->IsPurged() ) return; real->points.max_move = MAX(0,x); }
    void setTaint(int x)			{ if( !real || real->IsPurged() ) return; real->points.taint = MAX(0,x); }
    void setShadow_Points(int x)	{ if( !real || real->IsPurged() ) return; real->points.shadow_points = INTERVAL(0, x, real->points.max_shadow_points); }
    void setMax_Shadow_Points(int x){ if( !real || real->IsPurged() ) return; real->points.max_shadow_points = MAX(0,x); }
    void setStrain(int x)			{ if( !real || real->IsPurged() ) return; real->points.strain = MAX(0,x); }
    void setGold(int x)				{ if( !real || real->IsPurged() ) return; real->points.gold = MAX(0, x); }
    void setBank_Gold(int x)		{ if( !real || real->IsPurged() ) return; real->points.bank_gold = MAX(0, x); }
    void setExperience(int x);
    void setDamroll_Mod(int x)		{ if( !real || real->IsPurged() ) return; real->points.damroll = x; }

    void setMood(int x)				{ if( !real || real->IsPurged() ) return; real->PlayerData->mood = INTERVAL(MOOD_WIMPY, x, MOOD_BERSERK); }
    void setStance(int x)			{ if( !real || real->IsPurged() ) return; real->stance = INTERVAL(STANCE_DEFENSIVE, x, STANCE_OFFENSIVE); }
    void setStrength(int x)			{ if( !real || real->IsPurged() ) return; real->real_abils.str = real->aff_abils.str = MAX(0,x); }
    void setIntelligence(int x)		{ if( !real || real->IsPurged() ) return; real->real_abils.intel = real->aff_abils.intel = MAX(0,x); }
    void setWisdom(int x)			{ if( !real || real->IsPurged() ) return; real->real_abils.wis = real->aff_abils.wis = MAX(0,x); }
    void setDexterity(int x)		{ if( !real || real->IsPurged() ) return; real->real_abils.dex = real->aff_abils.dex = MAX(0,x); }
    void setConstitution(int x)		{ if( !real || real->IsPurged() ) return; real->real_abils.con = real->aff_abils.con = MAX(0,x); }
	flusspferd::value getFighting()		{ if( !real || real->IsPurged() ) return lookupValue(0); return lookupValue(FIGHTING(real)); }
	void setFightingVar( JSCharacter *t );
	void setFighting( JSCharacter *t );
	void stopFighting();
	void removeSource();

	void setPosition( int x );
	void setSkill_Practices( int x );
	void setSpell_Practices( int x );
	void setTrade_Practices( int x );

	void qval(flusspferd::string qName, const int nVal);
	int getQuest(flusspferd::string qName);

	flusspferd::array inventory();

    flusspferd::value room();
    flusspferd::value getLeader();
	flusspferd::array getFollowers();

    flusspferd::value js_get_char_room_vis(const char*);

    flusspferd::value js_get_char_vis(const char*);

	flusspferd::string getYellow(const int level);
	flusspferd::string getGreen(const int level);
	flusspferd::string getCyan(const int level);
	flusspferd::string getRed(const int level);
	flusspferd::string getMagenta(const int level);
	flusspferd::string getBlue(const int level);
	flusspferd::string getBold(const int level);
	flusspferd::string getNormal(const int level);
	bool getMobFlagged(const int level);

	int getQP(const int cvnum);
    
    void lag(int pulses);

	void start_timer(float seconds);
	void extract();

	void comm(const char*);
	flusspferd::value load_obj( const int vnum );

	int getAge();
	int getSkill( int s );
	void setSkill( int s, int v );
	void attach( int tVnum );
	void detach( int tVnum, int nr );
	int countJS( flusspferd::value tVnum );
	bool in_clan( const int clan );
	bool wantedBy( int cNum );
	bool canSee( flusspferd::value t );
	void affect( int effect, int duration, int skill, int modifier );
	void unaffect( int a );
	bool affectedBy( int a );

	void warrantByClan( int iClan );
	void pardonByClan( int iClan );
	int getRank( int iClan );
	bool getIsCouncil( int iClan );

	flusspferd::string getArriveMessage();
	flusspferd::string getLeaveMessage();
	flusspferd::value getMount();
	flusspferd::value getRiddenBy();
	flusspferd::value getTarget();
	flusspferd::value getMarked();
	flusspferd::value getHunting();
	flusspferd::value getOTarget();
	flusspferd::value getSittingOn();
	int getDeathWait();
	int getWarnings();
	int getMasterWeapon();
	int getHunger();
	int getThirst();
	int getDrunk();
	int getWimpy();
	bool getIsBashed();
	bool getShieldBlock();
	int getMaxCarryItems();
	int getMaxCarryWeight();
	int getInvis();
	void setInvis(int invis);
	flusspferd::array getAliases();
	flusspferd::value getDecayedBy();
	flusspferd::value getBurnedBy();
	flusspferd::value getPlaguedBy();
	int getExperienceToLevel();
	int getLegend();
	int getPlayingDays();
	int getPlayingHours();
	int getPlayerDeaths();
	int getMobDeaths();

	bool getHasSource();
	bool getLinkless();
	int getIdle();

	void setMount( JSCharacter *c );
	void setRiddenBy( JSCharacter *c );
	void setTarget( JSCharacter *c );
	void setMarked( JSCharacter *c );
	void setHunting( JSCharacter *c );
	void setOTarget( JSObject *o );
	void setSittingOn( JSObject *o );
	void setIsBashed( bool x );
	void setHunger( int x );
	void setThirst( int x );
	void setDrunk( int x );
	void setMasterWeapon( int x );
	void setWarnings( int x );
	void setWimpy( int x );
	void setDeathWait( int x );
	void setDecayedBy( JSCharacter *t );
	void setBurnedBy( JSCharacter *t );
	void setPlaguedBy( JSCharacter *t );


private:
    Character * real;
};

#endif
