
#ifdef KINSLAYER_JAVASCRIPT

#include "JSCharacter.h"
#include "JSRoom.h"
#include "JSObject.h"
#include "utils.h"
#include "spells.h"
#include "handler.h"
#include "mobs.h"
#include "db.h"
#include "screen.h"
#include "clans.h"

#include "js.h"
#include "js_functions.h"

#include "StringUtil.h"
#include "ClanUtil.h"
#include "UserLogoutType.h"
#include "Descriptor.h"

int level_exp( int level );

class JSEnvironment;

int JSCharacter::numberAllocated = 0;
int JSCharacter::numberDeallocated = 0;

void JSEnvironment::LoadJSCharacter()
{
	load_class<JSCharacter>();
}

void JSCharacter::setExperience(int x)
{
	if( !real || real->IsPurged() || IS_NPC(real) )
		return;
	real->points.exp = MAX(0, x);

	while( true && GET_LEVEL(real) < LVL_IMMORT )
	{
		if( GET_EXP(real) < level_exp(GET_LEVEL(real)) ) {//drop level

			real->DropLevel();
		}
		else if( real->ExperienceToLevel() <= 0 ) {//raise level
			
			++real->player.level;
			real->GainLevel(true);
		}
		else
			break;
	}
}
void JSCharacter::tell(JSCharacter *vict, flusspferd::string msg)
{
	if( !real || real->IsPurged() || !vict->toReal() || vict->toReal()->IsPurged() )
		return;
	real->SendTell(vict->toReal(), (char*)msg.c_str());
}
void JSCharacter::send( std::string message )
{
	if( !real || real->IsPurged() )
		return;
	real->Send(StringUtil::vaEscape(message + "\r\n"));
}

ACMD(do_say);

struct GameTime *real_time_passed(time_t t2, time_t t1);

extern std::vector<Object*> obj_proto;

int JSCharacter::getPlayerDeaths()
{
	if( !real || real->IsPurged() ) return 0;
	return real->pk_deaths;
}
int JSCharacter::getMobDeaths()
{
	if( !real || real->IsPurged() ) return 0;
	return real->mob_deaths;
}
int JSCharacter::getPlayingDays()
{
	if( !real || real->IsPurged() ) return 0;
	struct GameTime playing_time;
	playing_time = *real_time_passed((time(0) - real->player.time.logon.getTime()) + real->player.time.played, 0);
	return playing_time.day;
}
int JSCharacter::getPlayingHours()
{
	if( !real || real->IsPurged() ) return 0;
	struct GameTime playing_time;
	playing_time = *real_time_passed((time(0) - real->player.time.logon.getTime()) + real->player.time.played, 0);
	return playing_time.hours;
}
int JSCharacter::getExperienceToLevel()
{
	if( !real || real->IsPurged() ) return 0;
	return real->ExperienceToLevel();
}
int JSCharacter::getLegend()
{
	if( !real || real->IsPurged() ) return 0;
	return real->GetLegend();
}
bool JSCharacter::getLinkless()
{
	if( !real || real->IsPurged() )
		return true;
	return real->desc ? false : true;
}
int JSCharacter::getIdle()
{
	if( !real || real->IsPurged() )
		return 0;
	return real->player.timer;
}
bool JSCharacter::getHasSource()
{
	if( !real || real->IsPurged() ) return false;
	return PRF_FLAGGED(real, PRF_SOURCE);
}

void JSCharacter::say(flusspferd::string msg)
{
	if( !real || real->IsPurged() ) return;
	do_say(real, const_cast<char*>(msg.c_str()), 0, 0);
}

void JSCharacter::warrantByClan( int iClan )
{
	if( !real || real->IsPurged() ) return;
	Clan *pClan;
	Warrant *pWarrant;
	if( !( pClan = ClanUtil::getClan( iClan ) ) ) return;
	if( !( pWarrant = pClan->GetWarrant() ) ) return;
	SET_BIT_AR(GET_WARRANTS(real), pWarrant->vnum);
}
void JSCharacter::pardonByClan( int iClan )
{
	if( !real || real->IsPurged() ) return;
	Clan *pClan;
	Warrant *pWarrant;
	if( !( pClan = ClanUtil::getClan( iClan ) ) ) return;
	if( !( pWarrant = pClan->GetWarrant() ) ) return;
	REMOVE_BIT_AR(GET_WARRANTS(real), pWarrant->vnum);

}
int JSCharacter::getRank( int iClan )
{
	if( !real || real->IsPurged() ) return 0;
	UserClan *userClan = real->getUserClan( iClan );

	if( !userClan ) return false;

	return (int)userClan->getRank();
}
bool JSCharacter::getIsCouncil( int iClan )
{
	if( !real || real->IsPurged() ) return false;
	UserClan *userClan = real->getUserClan( iClan );

	if( !userClan ) return false;

	return userClan->getIsCouncil();
}

void JSCharacter::affect( int effect, int duration, int skill, int modifier )
{
	if(real && !real->IsPurged() && !real->IsProto() && effect > 0 && effect < MAX_AFFECT && duration >= 0)
	{
		affected_type af;
		af.type = skill;
		af.duration = duration;
		af.modifier = modifier;
		af.location = APPLY_NONE;
		af.bitvector = effect;
		affect_to_char( real, &af );
	}
}

void JSCharacter::unaffect( int a )
{
	if( real && !real->IsPurged() && !real->IsProto()) {
		affect_from_char(real, 0, a);
	}
}

bool JSCharacter::affectedBy( int a )
{
	return (real && !real->IsPurged() && a > 0 && a < MAX_AFFECT && AFF_FLAGGED(real, a));
}

flusspferd::array JSCharacter::inventory()
{
	flusspferd::array a = flusspferd::create_array();
	if( real && !real->IsPurged() && !real->IsProto() )
	{
		Object *o;
		for (o = real->carrying; o; o = o->next_content)
			a.push(lookupValue(o));
	}
	return a;
}

flusspferd::value JSCharacter::room()
{
	return lookupValue(real && !real->IsProto() ? real->in_room : 0);
}
flusspferd::value JSCharacter::getLeader()
{
	return lookupValue(real ? real->master : 0);
}
flusspferd::array JSCharacter::getFollowers()
{
	flusspferd::array a = flusspferd::create_array();
	if( real && !real->IsPurged() && !real->IsProto() ) {
		struct Follower	*f;
		for (f = real->followers;f;f = f->next)
			a.call("push", lookupValue( f->follower ) );
	}
	return a;
}


flusspferd::value JSCharacter::eq(int pos)
{
	return lookupValue(real && !real->IsProto() ? GET_EQ(real, pos) : 0);
}

flusspferd::value JSCharacter::js_get_char_room_vis(const char* arg)
{
	return lookupValue( (real && !real->IsProto() && !real->IsPurged()) ? get_char_room_vis(real, (char*)arg) : 0);
}

flusspferd::value JSCharacter::js_get_char_vis(const char* arg)
{
	return lookupValue( (real && !real->IsProto() && !real->IsPurged()) ? get_char_vis(real, (char*)arg) : 0);
}

void JSCharacter::setFightingVar( JSCharacter *t )
{
	if( real && !real->IsPurged() && !real->IsProto() && t->toReal() && !t->toReal()->IsPurged() && !t->toReal()->IsProto() )
	{
		FIGHTING(real) = t->toReal();
	}
}

void JSCharacter::setFighting( JSCharacter *t )
{
	if( real && !real->IsPurged() && !real->IsProto() && t->toReal() && !t->toReal()->IsPurged() && !t->toReal()->IsProto() )
	{
		real->StopFighting();
		real->SetFighting( t->toReal() );
	}
}

void JSCharacter::stopFighting()
{
	if( real && !real->IsPurged() && !real->IsProto() )
	{
		real->StopFighting();
	}

}

void JSCharacter::removeSource()
{
	if( real && !real->IsPurged() && !real->IsProto() )
	{
		real->RemoveSource();
	}
}

flusspferd::array JSCharacter::getAliases()
{
	if( !real || IS_NPC(real) ) return flusspferd::create_array();
	flusspferd::array a = flusspferd::create_array();
	a.set_length( this->real->aliases.size() );

	int index = 0;
	for( std::map<std::string,std::string>::iterator iter = this->real->aliases.begin();iter != this->real->aliases.end();++iter)
	{
		flusspferd::string command = (*iter).first;
		flusspferd::string result = (*iter).second;

		flusspferd::object container = flusspferd::create_object();
		container.set_property("command", command);
		container.set_property("result", result);
		a.set_element(index, container);
		++index;
	}
	return a;
}
void JSCharacter::damage(int x, flusspferd::value attacker) 
{
	if( !real || real->IsPurged() || real->IsProto() )
		return;
	Character *cA = 0;
	if( !attacker.is_null() && flusspferd::is_native<JSCharacter>(attacker.to_object()) )
	{
		cA = flusspferd::get_native<JSCharacter>(attacker.to_object()).toReal();
		if( cA && !cA->IsPurged() )
			::damage(cA, real, x, SKILL_MISC, -1);
	}
	else
		::damage(real, real, x, SKILL_MISC, -1);
}

void JSCharacter::setHps(int x)
{
	if( !real || real->IsPurged() || real->IsProto() ) return;
	real->points.hit = MIN(real->points.max_hit, x);
	if(GET_HIT(real) <= 0)
	{
		if( GET_LEVEL(real) >= LVL_IMPL )
			GET_HIT(real) = 1;
		else {
			MudLog(BRF, 0, TRUE, "%s killed by a JS trap at %s", GET_NAME(real),
				(real->in_room ? real->in_room->name : "<Unknown>"));
			real->Die();
		}
	}
}

void JSCharacter::lag(int pulses)
{
	if (real && !real->IsProto() && pulses > 0 && !real->IsPurged())
		WAIT_STATE(real, pulses);
}

void JSCharacter::start_timer(float seconds)
{
	if (real && !real->IsProto() && seconds > 0 && !real->IsPurged())
		set_property("__timer", flusspferd::value(seconds));
}
void JSCharacter::comm(const char* comm)
{
	if( real && !real->IsProto() && !real->IsPurged() ) {
		real->InterpCommand(StringUtil::vaEscape(comm).c_str());
	}
}
void JSCharacter::move_to_room( JSRoom *r )
{
	if( !real || real->IsPurged() || real->IsProto() ) return;
	if( r->toReal() ) {
		real->RemoveFromRoom();
		real->MoveToRoom( r->toReal() );
	}
}
void JSCharacter::extract()
{
	if( real && !real->IsPurged() && !real->IsProto() ) {
		real->Extract(UserLogoutType::scriptExtraction);
	}
}

flusspferd::value JSCharacter::load_obj( const int vnum )
{
	if( !real || real->IsPurged() || real->IsProto() ) return lookupValue( 0 );
	int r_num;
	Object *obj;
	if ((r_num = real_object(vnum)) < 0)
	{
		return lookupValue(0);
	}

	if( GET_OBJ_TYPE(obj_proto[r_num]) == ITEM_SPECIAL )
	{
		lookupValue(0);
	}

	obj = read_object(r_num, REAL, true);
	Room *inRoom = this->real->InRoom();
	sprintf(obj->creator, "Loaded by Javascript. Type: JSCharacter, ID: %ld, Vnum: %d, Room: %d.", this->real->player.idnum, this->real->getVnum(), (inRoom ? inRoom->vnum : -1));

	obj_to_char(obj,real);

#ifdef KINSLAYER_JAVASCRIPT
	if( !obj->IsPurged() ) {
		js_load_triggers(obj);
	}
#endif

	return lookupValue(obj);
}
bool JSCharacter::in_clan( const int clan )
{
	return (real && real->isInClan( clan ) ? true : false);
}

int JSCharacter::getQuest(flusspferd::string qName)
{
	Quest *q = 0;
	if( real && (q = real->GetQuest(qName.to_string())) )
		return q->var;
	return 0;
}
void JSCharacter::qval(flusspferd::string qName, const int nVal)
{
	if( real && !real->IsProto() )
	{
		Quest *q = 0;
		bool new_q = false;
		if( !(q = real->GetQuest(qName.to_string())) )
		{
			new_q = true;
			q = new Quest(real, qName.to_string(), nVal);
		}
		else
			q->var = nVal;
		MySQLSaveQuest(real->player.name, q, !new_q);
	}
}

flusspferd::string JSCharacter::getYellow(const int level)
{
	return (real ? COLOR_YELLOW(real,level) : "");
}
flusspferd::string JSCharacter::getGreen(const int level)
{
	return (real ? COLOR_GREEN(real,level) : "");
}
flusspferd::string JSCharacter::getCyan(const int level)
{
	return (real ? COLOR_CYAN(real,level) : "");
}
flusspferd::string JSCharacter::getRed(const int level)
{
	return (real ? COLOR_RED(real,level) : "");
}
flusspferd::string JSCharacter::getMagenta(const int level)
{
	return (real ? COLOR_MAGENTA(real,level) : "");
}
flusspferd::string JSCharacter::getBlue(const int level)
{
	return (real ? COLOR_BLUE(real,level) : "");
}
flusspferd::string JSCharacter::getBold(const int level)
{
	return (real ? COLOR_BOLD(real,level) : "");
}
flusspferd::string JSCharacter::getNormal(const int level)
{
	return (real ? COLOR_NORMAL(real,level) : "");
}

bool JSCharacter::getMobFlagged(const int flag)
{
	return (real && !real->IsPurged() && IS_NPC(real) && flag > 0 && flag < 64 && MOB_FLAGGED(real,flag));
}

int JSCharacter::getQP(const int cvnum)
{
	if( !real )
		return 0;
	Clan *c = ClanUtil::getClan(cvnum);
	if(c && real->getUserClan(cvnum))
		return real->getUserClan(cvnum)->getQuestPoints();
	return 0;
}

int JSCharacter::getSkill( int s )
{
	if( s >= 0 && s <= MAX_SKILLS && real && !real->IsPurged() )
		return GET_SKILL(real, s);
	return 0;
}
void JSCharacter::setSkill( int s, int v )
{
	if( s >= 0 && s <= MAX_SKILLS && real && !real->IsPurged() )
		return SET_SKILL(real, s, v);
}

void JSCharacter::attach( int tVnum )
{
	if( !real || real->IsProto() ) return;
	real->attachJS( tVnum );

}

void JSCharacter::detach( int tVnum, int nr )
{
	if( !real || real->IsProto() ) return;
	real->detachJS( tVnum, nr );
}
int JSCharacter::countJS( flusspferd::value tVnum )
{
	if( !real ) return 0;
	return real->countJS( tVnum.is_undefined() ? 0 : (int)tVnum.to_number() );
}

void JSCharacter::setPosition( int x )
{
	if( real && !real->IsPurged() && !real->IsProto() && x >= 0 && x <= POS_FLYING )
		GET_POS(real) = x;
}
void JSCharacter::setSkill_Practices( int x )
{
	if( real && !real->IsPurged() && !real->IsProto() && real->PlayerData && x >= 0 && x <= 1000 )
		real->PlayerData->skillpracs = x;
}
void JSCharacter::setSpell_Practices( int x )
{
	if( real && !real->IsPurged() && !real->IsProto() && real->PlayerData && x >= 0 && x <= 1000 )
		real->PlayerData->spellpracs = x;
}
void JSCharacter::setTrade_Practices( int x )
{
	if( real && !real->IsPurged() && !real->IsProto() && real->PlayerData && x >= 0 && x <= 1000 )
		real->PlayerData->tradepracs = x;
}

bool JSCharacter::wantedBy( int cNum )
{
	if( !real || real->IsPurged() )
		return false;
	return real->wantedByClan( cNum );
}
bool JSCharacter::canSee( flusspferd::value t )
{
	if( !real || real->IsPurged() || real->IsProto() )
		return false;
	if( flusspferd::is_native< JSCharacter >(t.to_object()) )
	{
		Character *cT = flusspferd::get_native< JSCharacter >(t.to_object()).toReal();
		if( cT && !cT->IsPurged() )
			return CAN_SEE(real, cT);
		return false;
	}
	if( flusspferd::is_native< JSObject >(t.to_object()) )
	{
		Object *oT = flusspferd::get_native< JSObject >(t.to_object()).toReal();
		if( oT && !oT->IsPurged() )
			return CAN_SEE_OBJ(real, oT);
		return false;
	}
	return false;
}

// GETTERS 

flusspferd::string JSCharacter::getArriveMessage()
{
	if( real && !real->IsPurged() )
		return real->player.ArriveMessage;
	return flusspferd::string( "" );
}
flusspferd::string JSCharacter::getLeaveMessage()
{
	if( real && !real->IsPurged() )
		return real->player.LeaveMessage;
	return flusspferd::string( "" );

}
flusspferd::value JSCharacter::getMount()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return lookupValue(real->player.mount);
	return lookupValue( 0 );
}
flusspferd::value JSCharacter::getRiddenBy()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return lookupValue(real->player.ridden_by);
	return lookupValue( 0 );
}
flusspferd::value JSCharacter::getTarget()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return lookupValue(real->player.target);
	return lookupValue( 0 );
}
flusspferd::value JSCharacter::getMarked()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return lookupValue(real->player.marked);
	return lookupValue( 0 );
}
flusspferd::value JSCharacter::getHunting()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return lookupValue(real->player.hunting);
	return lookupValue( 0 );
}
flusspferd::value JSCharacter::getOTarget()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return lookupValue(real->player.otarget);
	return lookupValue( 0 );
}
flusspferd::value JSCharacter::getSittingOn()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return lookupValue(real->player.sitting_on);
	return lookupValue( 0 );
}
int JSCharacter::getDeathWait()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return real->points.death_wait;
	return 0;
}
int JSCharacter::getWarnings()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return real->points.warning;
	return 0;
}
int JSCharacter::getMasterWeapon()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return real->points.master_weapon;
	return 0;
}
int JSCharacter::getHunger()
{
	if( real && !real->IsPurged() && real->PlayerData ) return real->PlayerData->conditions[FULL];
	return 0;
}
int JSCharacter::getThirst()
{
	if( real && !real->IsPurged() && real->PlayerData ) return real->PlayerData->conditions[THIRST];
	return 0;
}
int JSCharacter::getDrunk()
{
	if( real && !real->IsPurged() && real->PlayerData ) return real->PlayerData->conditions[DRUNK];
	return 0;
}
int JSCharacter::getWimpy()
{
	if( real && !real->IsPurged() && real->PlayerData ) return real->PlayerData->wimp_level;
	return 0;
}
bool JSCharacter::getIsBashed()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return real->points.is_bashed;
	return false;
}
bool JSCharacter::getShieldBlock()
{
	if( real && !real->IsPurged() && !real->IsProto() ) return real->ShieldBlock;
	return false;
}

int JSCharacter::getMaxCarryItems()
{
	if( real && !real->IsPurged() ) return CAN_CARRY_N( real );
	return 0;
}
int JSCharacter::getMaxCarryWeight()
{
	if( real && !real->IsPurged() ) return CAN_CARRY_W( real );
	return 0;
}
void JSCharacter::setInvis(int invis)
{
	if( real && !real->IsPurged() )
		GET_INVIS_LEV(real) = MAX(LVL_IMPL,invis);
}
int JSCharacter::getInvis()
{
	if( real && !real->IsPurged() && !IS_NPC(real) )
		return GET_INVIS_LEV(real);
	return 0;
}
flusspferd::value JSCharacter::getDecayedBy()
{
	if( real && !real->IsPurged() && !real->IsProto() )
		return lookupValue( real->DecayedBy );
	return lookupValue( 0 );
}
flusspferd::value JSCharacter::getBurnedBy()
{
	if( real && !real->IsPurged() && !real->IsProto() )
		return lookupValue( real->BurnedBy );
	return lookupValue( 0 );
}
flusspferd::value JSCharacter::getPlaguedBy()
{
	if( real && !real->IsPurged() && !real->IsProto() )
		return lookupValue( real->PlaguedBy );
	return lookupValue( 0 );
}
// SETTERS 

void JSCharacter::setMount( JSCharacter *c )
{
	if( real && !real->IsPurged() && !real->IsProto() )
		real->player.mount = ( c->toReal() && c->toReal()->IsPurged() ) ? (0) : c->toReal();
}
void JSCharacter::setRiddenBy( JSCharacter *c )
{
	if( real && !real->IsPurged() && !real->IsProto() && c->toReal() && !c->toReal()->IsPurged() && !c->toReal()->IsProto() )
		real->player.ridden_by = ( c->toReal() && c->toReal()->IsPurged() ) ? (0) : c->toReal();
}
void JSCharacter::setTarget( JSCharacter *c )
{
	if( real && !real->IsPurged() && !real->IsProto() && c->toReal() && !c->toReal()->IsPurged() && !c->toReal()->IsProto() )
		real->player.target = ( c->toReal() && c->toReal()->IsPurged() ) ? (0) : c->toReal();
}
void JSCharacter::setMarked( JSCharacter *c )
{
	if( real && !real->IsPurged() && c->toReal() && !c->toReal()->IsPurged() && !real->IsProto() && !c->toReal()->IsProto() )
		real->player.marked = ( c->toReal() && c->toReal()->IsPurged() ) ? (0) : c->toReal();
}
void JSCharacter::setHunting( JSCharacter *c )
{
	if( real && !real->IsPurged() && c->toReal() && !c->toReal()->IsPurged() && !real->IsProto() && !c->toReal()->IsProto() )
		real->player.hunting = ( c->toReal() && c->toReal()->IsPurged() ) ? (0) : c->toReal();
}
void JSCharacter::setOTarget( JSObject *o )
{
	if( real && !real->IsPurged() && o->toReal() && !o->toReal()->IsPurged() && !real->IsProto() && !o->toReal()->IsProto() )
		real->player.otarget = ( o->toReal() && o->toReal()->IsPurged() ) ? (0) : o->toReal();
}
void JSCharacter::setSittingOn( JSObject *o )
{
	if( real && !real->IsPurged() && o->toReal() && !o->toReal()->IsPurged() && !real->IsProto() && !o->toReal()->IsProto() )
		real->player.sitting_on = ( o->toReal() && o->toReal()->IsPurged() ) ? (0) : o->toReal();
}
void JSCharacter::setIsBashed( bool x )
{
	if( real && !real->IsPurged() && !real->IsProto() )
		real->points.is_bashed = x;
}
void JSCharacter::setHunger( int x )
{
	if( real && !real->IsPurged() && real->PlayerData )
		real->PlayerData->conditions[FULL] = MAX( MIN(24, x), 0 );
}
void JSCharacter::setThirst( int x )
{
	if( real && !real->IsPurged() && real->PlayerData )
		real->PlayerData->conditions[THIRST] = MAX( MIN(24, x), 0 );
}
void JSCharacter::setDrunk( int x )
{
	if( real && !real->IsPurged() && real->PlayerData )
		real->PlayerData->conditions[DRUNK] = MAX( MIN(24, x), 0 );
}
void JSCharacter::setMasterWeapon( int x )
{
	if( real && !real->IsPurged() )
		real->points.master_weapon = x;
}
void JSCharacter::setWarnings( int x )
{
	if( real && !real->IsPurged() )
		real->points.warning = x;
}
void JSCharacter::setWimpy( int x )
{
	if( real && !real->IsPurged() && !real->PlayerData )
		real->PlayerData->wimp_level = x;
}
void JSCharacter::setDeathWait( int x )
{
	if( real && !real->IsPurged() )
		real->points.death_wait = x;
}
void JSCharacter::setDecayedBy( JSCharacter *t )
{
	if( real && !real->IsPurged() && t->toReal() && !t->toReal()->IsPurged() && !real->IsProto() && !t->toReal()->IsProto() )
		real->DecayedBy = t->toReal();
}
void JSCharacter::setBurnedBy( JSCharacter *t )
{
	if( real && !real->IsPurged() && t->toReal() && !t->toReal()->IsPurged() && !real->IsProto() && !t->toReal()->IsProto() )
		real->BurnedBy = t->toReal();
}
void JSCharacter::setPlaguedBy( JSCharacter *t )
{
	if( real && !real->IsPurged() && t->toReal() && !t->toReal()->IsPurged() && !real->IsProto() && !t->toReal()->IsProto() )
		real->PlaguedBy = t->toReal();
}
int JSCharacter::getAge()
{
	if( real && !real->IsPurged() )
		return GET_AGE(real);
	return 0;
}

flusspferd::object JSCharacter::getCreatedDatetime()
{
	if(!real || real->IsPurged())
		return flusspferd::object();

	return JS_createDatetime(real->player.time.birth);
}


#endif

