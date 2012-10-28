/********************************************************************
 *	mobs.cpp - Implements the MobManager classes					*
 *																	*
 *	Coded by Galnor in April of 2009 (Sortak@gmail.com)				*
 *																	*
 ********************************************************************/
#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "comm.h"
#include "interpreter.h"
#include "utils.h"
#include "db.h"
#include "constants.h"
#include "olc.h"
#include "clans.h"
#include "mobs.h"
#include "shop.h"

#include "MiscUtil.h"
#include "ClanUtil.h"
#include "Descriptor.h"
#include "ClanUtil.h"

#ifdef KINSLAYER_JAVASCRIPT
#include "js.h"
#include "js_trigger.h"
#endif

MobManager *MobManager::Self = NULL;
extern Character *character_list;
extern Descriptor *descriptor_list;
extern Shop *shop_index;
extern int top_shop;

int real_trigger(int vnum);
void SetupMySQL( bool crash_on_failure );
void assign_the_shopkeepers(void);
void assign_mobiles(void);

MobManager::MobManager()
{
}
MobManager::~MobManager()
{
	for( unsigned int i = 0;i < MyMobIndex.size();i++ )
	{
		delete (MyMobProto[i]);
		delete (MyMobIndex[i]);
	}
	Self = NULL;
}

MobManager &MobManager::GetManager()
{
	if( Self == NULL ) Self = new MobManager();
	return (*Self);
}

void MobManager::Free()
{
	if( Self != NULL )
		delete (Self);
}
void MobManager::Reload()
{
	//Before doing anything, change mob load real indices to virtual numbers.
	for( unsigned int i = 0;i < ZoneManager::GetManager().NumZones();++i )
	{
		Zone *z = ZoneManager::GetManager().GetZoneByRnum( i );
		for(unsigned int s = 0;s < z->cmd.size();++s)
		{
			if( z->cmd[ s ]->command == 'M' )
				z->cmd[ s ]->arg1 = z->cmd[ s ]->GetRealArg1();
		}
	}
	for (unsigned int i = 0; i < top_shop;++i)
	{
		shop_index[ i ].keeper = MobManager::GetManager().VirtualMobile( shop_index[i].keeper );
	}
	MobManager::GetManager().Free();
	MobManager::GetManager().BootPrototypes();

	Character *prototype;
	int prevNr = -1;
	// Go around and update the live prototypes.
	for( Character *mob = character_list;mob;mob = mob->next )
	{
		if( mob->nr == -1 ) continue;
		if( mob->nr != prevNr ) {//If the prev prototype is the same, we do not have to do a lookup.
			prototype = MobManager::GetManager().GetPrototypeByVnum( mob->getVnum() );
			prevNr = prototype->nr;
		}
		UpdateLiveMobile( mob, prototype );
	}
	//Go around and update the zone commands
	for( unsigned int i = 0;i < ZoneManager::GetManager().NumZones();++i )
	{
		Zone *z = ZoneManager::GetManager().GetZoneByRnum( i );
		for(unsigned int s = 0;s < z->cmd.size();)
		{
			if( z->cmd[ s ]->command != 'M' ) {
				++s;
				continue;
			}
			Character *mob;
			if( (mob = MobManager::GetManager().GetPrototypeByVnum( z->cmd[ s ]->arg1 )) != NULL ) {
				//Command is still valid.
				z->cmd[ s ]->arg1 = mob->nr;
				++s;
			}
			else {
				//Invalid. We must remove it.
				z->cmd.erase( z->cmd.begin()+s );
			}
		}
	}
	for (unsigned int i = 0; i < top_shop;++i)
	{
		shop_index[ i ].keeper = MobManager::GetManager().RealMobile( shop_index[i].keeper );
	}
	assign_the_shopkeepers();
	assign_mobiles();
}

void MobManager::BootPrototypes()
{
	std::stringstream Query1;
	sql::Query MyQuery1;
	sql::Row MyRow;

	Query1 << "SELECT * FROM mob_protos ORDER BY vnum ASC;";
	try {
		MyQuery1 = gameDatabase->sendQuery(Query1.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "MobManager::BootPrototypes : %s", e.getMessage().c_str());
		return;
	}
	
	Clock MyClock;
	MyClock.reset(true);
	while( MyQuery1->hasNextRow() )
	{
		MyRow = MyQuery1->getRow();
		BootPrototype( MyRow );
	}
	MyClock.turnOff();
	MyClock.print();
}
/* The mother of the booting methods for mobs... */
Character *MobManager::BootPrototype( sql::Row &MyRow )
{
	Character *NewMob = new Character();
	NewMob->MobData = new MobOnlyData();

	NewMob->proto					= true;
	NewMob->player.name				= MyRow["alias"];
	NewMob->player.short_descr		= (MyRow["sdesc"].size()?str_dup(MyRow["sdesc"].c_str()):(NULL));
	NewMob->player.long_descr		= (MyRow["ldesc"].size()?str_dup(MyRow["ldesc"].c_str()):(NULL));
	NewMob->player.description		= (MyRow["ddesc"].size()?str_dup(MyRow["ddesc"].c_str()):(NULL));
	NewMob->player.ArriveMessage	= (MyRow["arrive_message"].size()?str_dup(MyRow["arrive_message"].c_str()):(NULL));
	NewMob->player.LeaveMessage		= (MyRow["leave_message"].size()?str_dup(MyRow["leave_message"].c_str()):(NULL));
	NewMob->player.act				= atoi(MyRow["mob_flags"].c_str());
	NewMob->player.affected_by[0]	= atoi(MyRow["aff_flags0"].c_str());
	NewMob->player.affected_by[1]	= atoi(MyRow["aff_flags1"].c_str());
	NewMob->player.affected_by[2]	= atoi(MyRow["aff_flags2"].c_str());
	NewMob->player.affected_by[3]	= atoi(MyRow["aff_flags3"].c_str());
	NewMob->player.level			= atoi(MyRow["mob_level"].c_str());
	NewMob->points.max_hit			= atoi(MyRow["maxhit"].c_str());
	NewMob->points.max_move			= atoi(MyRow["maxmove"].c_str());
	NewMob->points.max_mana			= atoi(MyRow["maxmana"].c_str());
	NewMob->points.gold				= atoi(MyRow["gold"].c_str());

	NewMob->MobData->aggro			= atoi(MyRow["aggro"].c_str());

	NewMob->MobData->damnodice		= atoi(MyRow["numdamdice"].c_str());
	NewMob->MobData->damsizedice	= atoi(MyRow["damsizedice"].c_str());
	NewMob->points.hit				= atoi(MyRow["numhpdice"].c_str());
	NewMob->points.move				= atoi(MyRow["hpbonus"].c_str());
	NewMob->points.mana				= atoi(MyRow["sizehpdice"].c_str());
	NewMob->points.damroll			= atoi(MyRow["damroll"].c_str());
	NewMob->points.exp				= atoi(MyRow["exp"].c_str());
	NewMob->player.position			= atoi(MyRow["position"].c_str());
	NewMob->MobData->default_pos	= atoi(MyRow["default_position"].c_str());
	NewMob->player.sex				= atoi(MyRow["sex"].c_str());
	NewMob->player.race				= atoi(MyRow["race"].c_str());
	NewMob->points.offensive		= atoi(MyRow["ob"].c_str());
	NewMob->points.dodge			= atoi(MyRow["db"].c_str());
	NewMob->points.parry			= atoi(MyRow["pb"].c_str());

	NewMob->player.weight			= (200);
	NewMob->player.height			= (198);

	//check to see if this mob is in a clan
	if( ClanUtil::getClan(atoi(MyRow["clan"].c_str())) != NULL )
	{
		UserClan *userClan = UserClan::setupNewInstance(NewMob->getUserId(), MyRow.getInt("clan"));
		userClan->setRank(MyRow.getInt("rank"));
		NewMob->addToClan(userClan);
	}
	NewMob->player.chclass			= atoi(MyRow["class"].c_str());

	//Returns NULL if kit does not exist.
	NewMob->MobData->primary_kit	= KitManager::GetManager().GetKitByVnum(atoi(MyRow["primary_kit"].c_str()));

	if( MyRow["food_vnum"] != "-1" )
	{
		NewMob->MobData->Food = new FoodUnit();
		NewMob->MobData->Food->vnum		= atoi(MyRow["food_vnum"].c_str());
		NewMob->MobData->Food->quantity = atoi(MyRow["food_quantity"].c_str());
	}

	if( MyRow["skin_vnum"] != "-1" )
	{
		NewMob->MobData->Skin = new SkinningUnit();
		NewMob->MobData->Skin->vnum		= atoi(MyRow["skin_vnum"].c_str());
		NewMob->MobData->Skin->required = atoi(MyRow["skin_required"].c_str());
		NewMob->MobData->Skin->skinned  = atoi(MyRow["skin_skinned"].c_str());
	}

	NewMob->points.warrants[0]		= atoi(MyRow["warrants0"].c_str());
	NewMob->points.warrants[1]		= atoi(MyRow["warrants1"].c_str());
	NewMob->points.warrants[2]		= atoi(MyRow["warrants2"].c_str());
	NewMob->points.warrants[3]		= atoi(MyRow["warrants3"].c_str());
	NewMob->MobData->nsects			= atoi(MyRow["nsects"].c_str());

	NewMob->MobData->attack_type	= atoi(MyRow["attack"].c_str());
	NewMob->SetStr( 20 );
	NewMob->SetDex( 20 );
	NewMob->SetInt( 20 );
	NewMob->SetWis( 20 );
	NewMob->SetCon( 20 );
	NewMob->real_abils = NewMob->aff_abils;//Galnor 05/09/09 - fixed a bug w/ mob stats

	std::string Assists = MyRow["assists"];
	NewMob->MobData->assists = MiscUtil::splitToIntList(Assists, ' ');

#ifdef KINSLAYER_JAVASCRIPT

// Find any js scripts that are listed as attached to this mob in the db, and attach them to it
    NewMob->js_scripts = std::shared_ptr<std::vector<JSTrigger*> >(new std::vector<JSTrigger*>());
    try {
        sql::Query q = gameDatabase->sendQuery("SELECT * FROM js_attachments WHERE type='M' AND target_vnum='" + MyRow["vnum"] + "'");
        while( q->hasNextRow() )
        {
            sql::Row row = q->getRow();
			int vnum = atoi(row["script_vnum"].c_str());
			JSTrigger* t = JSManager::get()->getTrigger(vnum);
            NewMob->js_scripts->push_back(t);
        }
    } catch( sql::QueryException e ) {
		cout << "error in js mob load:" << endl;
        e.report();
    }

#endif

	if( !this->MyMobIndex.size() || this->MyMobIndex.back()->vnum < atoi(MyRow["vnum"].c_str()) )
	{
		Index *index = new Index();
		index->func = NULL;
		index->farg = NULL;
		index->in_db = true;
		index->number = this->MyMobIndex.size();
		index->vnum = atoi(MyRow["vnum"].c_str());

		this->MyMobIndex.push_back(index);
		this->MyMobProto.push_back(NewMob);

		NewMob->nr = (this->MyMobProto.size()-1);
	}

	return NewMob;
}
/*
Input:  The zone virtual number within which the mobs to be updated are contained.
Output: None
Functionality: Running this will either insert, update, or delete mobs in the database
               depending upon how they are flagged.

*/
void MobManager::SavePrototypes( const unsigned int zone_vnum )
{
	Zone *zone = ZoneManager::GetManager().GetZoneByVnum( zone_vnum );
	bool fail;

	if( zone == NULL )
		return;

	std::list<std::string> MyQueries;
	unsigned int bvnum = zone->GetBottom(),tvnum = zone->GetTop();
	int bot = -1;
	for(unsigned int i = 0;i < MyMobIndex.size() && (u_int)MyMobIndex[i]->vnum <= tvnum;++i)
	{
		if( (unsigned int)MyMobIndex[i]->vnum >= bvnum )
		{
			/* We need to delete the mob if it is flagged for deletion */
			if( MyMobProto[i]->MobData->deleted == true )
			{
				DeletePrototypeFromDatabase( i );
				continue;
			}

			MyQueries.clear();
			MyQueries = GrabSaveQuery(i);
			try {
				for(std::list<std::string>::iterator sqlIter = MyQueries.begin();sqlIter!=MyQueries.end();)
				{
					try {
						fail=false;
						gameDatabase->sendRawQuery( (*sqlIter) );
					}
					catch( sql::QueryException e ) {
						if( e.err == 2013 ) {
							fail=true;
							SetupMySQL(false);
						} else {
							MudLog(BRF, LVL_GOD, TRUE, "Unable to save mobs in zone %d : %s", zone_vnum, e.getErrorMessage().c_str());
							return;
						}
					}
					if( fail == false ) {
						++sqlIter;
					}
				}
			} catch(sql::QueryException e) {
				MudLog(BRF, LVL_APPR, TRUE, "MobManager::SavePrototypes : %s", e.getMessage().c_str());
				return;
			}
			GetIndex(i)->in_db = true;
		}
		if( bot == -1 )
			bot = i;
	}
	olc_remove_from_save_list(zone_vnum,OLC_SAVE_MOB);
//	if( bot == -1 )
//		return;//Nothing to save...

	//Mark all of these as existing in the DB.
//	for(;MyMobIndex[bot]->vnum <= tvnum;++bot)
//	{
//		GetIndex(bot)->in_db = true;
//	}
}
void MobManager::SavePrototypes( const unsigned int bottom_vnum, const unsigned int top_vnum )
{
	int brnum = RealMobile(bottom_vnum), trnum = RealMobile(top_vnum);
	bool fail;

	if( brnum == -1 || trnum == -1 || brnum >= trnum )
		return;//Invalid input.

	std::list<std::string> MyQueries;
	for(unsigned int i = brnum;i < (unsigned int)trnum;++i)
	{
		MyQueries = GrabSaveQuery( brnum );
		for(std::list<std::string>::iterator sqlIter=MyQueries.begin();sqlIter != MyQueries.end();)
		{
			try {
				fail=false;
				gameDatabase->sendRawQuery( (*sqlIter) );
			} catch( sql::QueryException e ) {
				if( e.err == 2013 )
				{//We got disconnected from the database...
					SetupMySQL( false );
				}
				else
				{
					e.report();
					return;
				}
			}
			if( false == false ) {
				++sqlIter;
			}
		}
	}
	for( unsigned int i = brnum;i < (unsigned int)trnum;++i )
	{
		GetIndex( i )->in_db = true;
	}
}
std::list<std::string> MobManager::GrabSaveQuery( const unsigned int mob_rnum )
{
	std::list<std::string> MyQueries;
	std::stringstream QueryBuffer, RemoveBuffer1, RemoveBuffer2;
	std::stringstream AssistsBuffer;

	Character *m = MyMobProto[mob_rnum];
	UserClan *userClan = m->userClans.empty() ? NULL : m->userClans.front();

	for(std::list<int>::iterator aIter = m->MobData->assists.begin();aIter != m->MobData->assists.end();++aIter)
	{
		if( aIter != m->MobData->assists.begin() )
			AssistsBuffer << " ";
		AssistsBuffer << (*aIter);
	}

	if( MyMobIndex[mob_rnum]->in_db == false )
	{
		QueryBuffer << "INSERT INTO mob_protos(vnum,alias,sdesc,ldesc,ddesc,arrive_message,leave_message,mob_flags,"
			"aff_flags0,aff_flags1,aff_flags2,aff_flags3,mob_level,maxhit,maxmove,"
			"maxmana,numdamdice,damsizedice,numhpdice,sizehpdice,hpbonus,damroll,exp,position,default_position,sex,"
			"race,ob,db,pb,clan,rank,class,primary_kit,food_vnum,food_quantity,skin_vnum,skin_required,skin_skinned,warrants0,warrants1,warrants2,warrants3,"
			"gold,aggro,nsects,attack,str,dex,intel,wis,con,assists)"
			"VALUES(";
			QueryBuffer << SQLVal(MyMobIndex[m->nr]->vnum);
			QueryBuffer << SQLVal(sql::escapeString(m->player.name));
			QueryBuffer <<(m->player.short_descr?SQLVal(sql::escapeString(m->player.short_descr)):"");
			QueryBuffer <<(m->player.long_descr?SQLVal(sql::escapeString(m->player.long_descr)):"");
			QueryBuffer <<(m->player.description?SQLVal(sql::escapeString(m->player.description)):"");
			QueryBuffer << SQLVal(sql::escapeString((m->player.ArriveMessage?m->player.ArriveMessage:"")));
			QueryBuffer << SQLVal(sql::escapeString((m->player.LeaveMessage?m->player.LeaveMessage:"")));
			QueryBuffer << SQLVal(m->player.act);
			QueryBuffer << SQLVal(m->player.affected_by[0]);
			QueryBuffer << SQLVal(m->player.affected_by[1]);
			QueryBuffer << SQLVal(m->player.affected_by[2]);
			QueryBuffer << SQLVal(m->player.affected_by[3]);
			QueryBuffer << SQLVal((int)m->player.level);
			QueryBuffer << SQLVal(m->points.max_hit);
			QueryBuffer << SQLVal(m->points.max_move);
			QueryBuffer << SQLVal(m->points.max_mana);
			QueryBuffer << SQLVal((int)GET_NDD(m));
			QueryBuffer << SQLVal((int)GET_SDD(m));
			QueryBuffer << SQLVal((int)GET_HIT(m));
			QueryBuffer << SQLVal((int)GET_MANA(m));
			QueryBuffer << SQLVal((int)GET_MOVE(m));
			QueryBuffer << SQLVal((int)GET_DAMROLL(m));
			QueryBuffer << SQLVal((int)GET_EXP(m));
			QueryBuffer << SQLVal((int)GET_POS(m));
			QueryBuffer << SQLVal((int)m->MobData->default_pos);
			QueryBuffer << SQLVal((int)GET_SEX(m));
			QueryBuffer << SQLVal((int)GET_RACE(m));
			QueryBuffer << SQLVal(GET_OB(m));
			QueryBuffer << SQLVal(GET_DB(m));
			QueryBuffer << SQLVal(GET_PB(m));
			QueryBuffer << SQLVal(userClan ? userClan->getClanId() : -1);
			QueryBuffer << SQLVal(userClan ? ((int)userClan->getRank()) : -1);
			QueryBuffer << SQLVal((int)GET_CLASS(m));
			QueryBuffer << SQLVal(m->MobData->primary_kit?m->MobData->primary_kit->vnum:-1);
			QueryBuffer << SQLVal(m->MobData->Food?m->MobData->Food->vnum:-1);
			QueryBuffer << SQLVal(m->MobData->Food?m->MobData->Food->quantity:-1);
			QueryBuffer << SQLVal(m->MobData->Skin?m->MobData->Skin->vnum:-1);
			QueryBuffer << SQLVal(m->MobData->Skin?m->MobData->Skin->required:-1);
			QueryBuffer << SQLVal(m->MobData->Skin?m->MobData->Skin->skinned:-1);
			QueryBuffer << SQLVal(GET_WARRANTS(m)[0]);
			QueryBuffer << SQLVal(GET_WARRANTS(m)[1]);
			QueryBuffer << SQLVal(GET_WARRANTS(m)[2]);
			QueryBuffer << SQLVal(GET_WARRANTS(m)[3]);
			QueryBuffer << SQLVal(m->points.gold);
			QueryBuffer << SQLVal(m->MobData->aggro);
			QueryBuffer << SQLVal(m->MobData->nsects);
			QueryBuffer << SQLVal(GET_ATTACK(m));
			QueryBuffer << SQLVal(m->GetStr());
			QueryBuffer << SQLVal(m->GetDex());
			QueryBuffer << SQLVal(m->GetInt());
			QueryBuffer << SQLVal(m->GetWis());
			QueryBuffer << SQLVal(m->GetCon());
			QueryBuffer << SQLVal(AssistsBuffer.str(),true) << ");" << std::endl;
	}
	else
	{//Ugh... Produce the same monstrosity as above, but in the UPDATE form...
		QueryBuffer << "UPDATE mob_protos SET ";
		QueryBuffer <<	"alias='"			<< (sql::escapeString(m->player.name))								<< "',";
		QueryBuffer <<	"sdesc='"			<< (m->player.short_descr?sql::escapeString(m->player.short_descr):"")		<< "',";
		QueryBuffer <<	"ldesc='"			<< (m->player.long_descr?sql::escapeString(m->player.long_descr):"")			<< "',";
		QueryBuffer <<	"ddesc='"			<< (m->player.description?sql::escapeString(m->player.description):"")		<< "',";
		QueryBuffer <<	"arrive_message='"	<< (m->player.ArriveMessage?sql::escapeString(m->player.ArriveMessage):"")	<< "',";
		QueryBuffer <<	"leave_message='"	<< (m->player.LeaveMessage?sql::escapeString(m->player.LeaveMessage):"")		<< "',";
		QueryBuffer <<	"mob_flags='"		<< (int)m->player.act												<< "',";
		QueryBuffer <<	"aff_flags0='"		<< m->player.affected_by[0]										<< "',";
		QueryBuffer <<	"aff_flags1='"		<< m->player.affected_by[1]										<< "',";
		QueryBuffer <<	"aff_flags2='"		<< m->player.affected_by[2]										<< "',";
		QueryBuffer <<	"aff_flags3='"		<< m->player.affected_by[3]										<< "',";
		QueryBuffer <<	"mob_level='"		<< (int)m->player.level											<< "',";
		QueryBuffer <<	"maxhit='"			<< m->points.max_hit											<< "',";
		QueryBuffer <<	"maxmove='"			<< m->points.max_move											<< "',";
		QueryBuffer <<	"maxmana='"			<< m->points.max_mana											<< "',";
		QueryBuffer <<	"numdamdice='"		<< (int)GET_NDD(m)												<< "',";
		QueryBuffer <<	"damsizedice='"		<< (int)GET_SDD(m)												<< "',";
		QueryBuffer <<	"numhpdice='"		<< (int)GET_HIT(m)												<< "',";
		QueryBuffer <<	"sizehpdice='"		<< (int)GET_MANA(m)												<< "',";
		QueryBuffer <<	"hpbonus='"			<< (int)GET_MOVE(m)												<< "',";
		QueryBuffer <<	"damroll='"			<< (int)GET_DAMROLL(m)											<< "',";
		QueryBuffer <<	"exp='"				<< (int)GET_EXP(m)												<< "',";
		QueryBuffer <<	"position='"		<< (int)m->player.position										<< "',";
		QueryBuffer <<	"default_position='"<< (int)m->MobData->default_pos									<< "',";
		QueryBuffer <<	"sex='"				<< (int)m->player.sex											<< "',";
		QueryBuffer <<	"race='"			<< (int)m->player.race											<< "',";
		QueryBuffer <<	"ob='"				<< GET_OB(m)													<< "',";
		QueryBuffer <<	"db='"				<< GET_DB(m)													<< "',";
		QueryBuffer <<	"pb='"				<< GET_PB(m)													<< "',";
		QueryBuffer <<	"clan='"			<< (userClan ? userClan->getClanId() : -1)						<< "',";
		QueryBuffer <<	"rank='"			<< (userClan ? ((int)userClan->getRank()) : -1)					<< "',";
		QueryBuffer <<	"class='"			<< (int)m->player.chclass										<< "',";
		QueryBuffer <<	"primary_kit='"		<< (m->MobData->primary_kit?m->MobData->primary_kit->vnum:-1)	<< "',";
		QueryBuffer <<	"food_vnum='"		<< (m->MobData->Food?m->MobData->Food->vnum:-1)					<< "',";
		QueryBuffer <<	"food_quantity='"	<< (m->MobData->Food?m->MobData->Food->quantity:-1)				<< "',";
		QueryBuffer <<  "skin_vnum='"       << (m->MobData->Skin?m->MobData->Skin->vnum:-1)                 << "',";
		QueryBuffer <<  "skin_required='"   << (m->MobData->Skin?m->MobData->Skin->required:-1)             << "',";
		QueryBuffer <<  "skin_skinned='"    << (m->MobData->Skin?m->MobData->Skin->skinned:-1)              << "',";
		QueryBuffer <<	"warrants0='"		<< m->points.warrants[0]										<< "',";
		QueryBuffer <<	"warrants1='"		<< m->points.warrants[1]										<< "',";
		QueryBuffer <<	"warrants2='"		<< m->points.warrants[2]										<< "',";
		QueryBuffer <<	"warrants3='"		<< m->points.warrants[3]										<< "',";
		QueryBuffer <<	"gold='"			<< m->points.gold												<< "',";
		QueryBuffer <<	"aggro='"			<< m->MobData->aggro											<< "',";
		QueryBuffer <<	"nsects='"			<< m->MobData->nsects											<< "',";
		QueryBuffer <<	"attack='"			<< GET_ATTACK(m)												<< "',";
		QueryBuffer <<	"str='"				<< m->GetStr()													<< "',";
		QueryBuffer <<	"dex='"				<< m->GetDex()													<< "',";
		QueryBuffer <<	"intel='"			<< m->GetInt()													<< "',";
		QueryBuffer <<	"wis='"				<< m->GetWis()													<< "',";
		QueryBuffer <<	"con='"				<< m->GetCon()													<< "',";
		QueryBuffer <<	"assists='"			<< AssistsBuffer.str()											<< "'";
		QueryBuffer <<	"WHERE vnum='"		<< VirtualMobile((u_int)m->nr)									<< "';";
		QueryBuffer <<	std::endl;

		//Now produce a query to clear the script from the database.
#ifdef KINSLAYER_JAVASCRIPT
		RemoveBuffer2 << "DELETE FROM js_attachments WHERE type='M' AND target_vnum='" <<  VirtualMobile((u_int)m->nr) << "';" << std::endl;
#endif
	}
	MyQueries.push_back(QueryBuffer.str());
	if( RemoveBuffer1.str().size() > 0 )
		MyQueries.push_back(RemoveBuffer1.str());

#ifdef KINSLAYER_JAVASCRIPT
	if( RemoveBuffer2.str().size() > 0 )
		MyQueries.push_back(RemoveBuffer2.str());
#endif

#ifdef KINSLAYER_JAVASCRIPT
	for(unsigned int i = 0;i < m->js_scripts->size();++i) {
		QueryBuffer.str("");
		QueryBuffer << "INSERT INTO js_attachments(type,target_vnum,script_vnum) VALUES('M','"
			<< VirtualMobile((unsigned int)m->nr) << "','" << m->js_scripts->at(i)->vnum << "');";
		MyQueries.push_back(QueryBuffer.str());
	}
#endif
	return MyQueries;
}

/*
Input:  Mob's index number
Output: A list of queries that will can be used to delete a mob from the database fully.
*/
std::list<std::string> MobManager::GrabDeleteQuery( const unsigned int mob_rnum )
{
	Index *MyIndex;
	Character *MyProto;
	std::list<std::string> MyQueries;
	std::stringstream QueryBuffer;

	if( (MyIndex = GetIndex(mob_rnum)) == NULL || (MyProto = GetPrototype(mob_rnum)) == NULL )
		return MyQueries;
	if( MyIndex->in_db == false )//Mob doesn't even exist in the DB yet. Might as well return nothing.
		return MyQueries;

	QueryBuffer << "DELETE FROM mob_protos WHERE vnum='" << MyIndex->vnum << "';";
	MyQueries.push_back(QueryBuffer.str());
	QueryBuffer.str("");
	QueryBuffer << "DELETE FROM js_attachments WHERE type='M' AND target_vnum='" << MyIndex->vnum << "';";
	MyQueries.push_back(QueryBuffer.str());

	return MyQueries;
}

void MobManager::SavePrototype( const unsigned int index )
{
	if( index > MyMobIndex.size() )
		return;
	std::list<std::string> SaveQueries = GrabSaveQuery(index);
	try {
		for( std::list<std::string>::iterator sqlIter = SaveQueries.begin();sqlIter != SaveQueries.end();++sqlIter )
		{
			gameDatabase->sendRawQuery( (*sqlIter) );
		}
	} catch(sql::QueryException e) {
		MudLog(BRF, LVL_APPR, TRUE, "MobManager::SavePrototype : %s", e.getMessage().c_str());
		e.report();
		return;
	}
	MyMobIndex[index]->in_db = true;
}
void MobManager::AddPrototype( Character *Proto, const int vnum )
{
	Index *index = new Index();
	Character *mob;
	Descriptor *dsc;
	unsigned int rmob_num, cmd_no;
	int shop;

	std::vector<Index *>::iterator IndexIterator;
	std::vector<Character *>::iterator MobIterator;
	//Find where to put the new MOB.
	for (rmob_num = 0, IndexIterator = MyMobIndex.begin(),MobIterator = MyMobProto.begin();
		rmob_num < MyMobIndex.size();
		++rmob_num, ++MobIterator, ++IndexIterator
		)
	{
		if((*IndexIterator)->vnum > vnum)
			break;
	}
	//We need the iterators from the above loop in order to insert into the vector
	//at a very specific location. Without them, we're limited to front and back only.

	index->vnum = vnum;
	index->number = 0;
	index->func = NULL;
	Proto->nr = rmob_num;
	MyMobIndex.insert(IndexIterator, index);
	MyMobProto.insert(MobIterator  , Proto);

	/*
	* Update live mobile rnums.
	*/

	for (mob = character_list; mob; mob = mob->next)
		if (mob->nr >= Proto->nr)
			++mob->nr;
	for( unsigned int i = rmob_num+1;i < MobManager::GetManager().NumberOfPrototypes();++i )
	{
		MobManager::GetManager().GetIndex(i)->number++;
		MobManager::GetManager().GetPrototype(i)->nr++;
	}

	/*
	 * Update zone table.
	 */

	Zone *zone;
	for (unsigned int i = 0; (zone = ZoneManager::GetManager().GetZoneByRnum(i)) != NULL;i++)
	{
		for (cmd_no = 0; cmd_no < zone->cmd.size(); ++cmd_no)
		{
			if (zone->cmd[cmd_no]->command == 'M')
			{
				if (zone->cmd[cmd_no]->arg1 >= Proto->nr)
				{
					++zone->cmd[cmd_no]->arg1;
				}
			}
		}
	}
	/*
	 * Update shop keepers.
	 */

	if (shop_index)
		for (shop = 0; shop < top_shop; shop++)
			if (SHOP_KEEPER(shop) >= Proto->nr)
				SHOP_KEEPER(shop)++;

	/*
	 * Update keepers in shops being edited and other mobs being edited.
	 */
	for (dsc = descriptor_list; dsc; dsc = dsc->next)
	{
		if (dsc->connected == CON_SEDIT)
		{
			if (OLC_SHOP(dsc)->keeper >= Proto->nr)
				OLC_SHOP(dsc)->keeper++;
		}

		else if (dsc->connected == CON_MEDIT)
		{
			if (OLC_MOB(dsc)->nr >= Proto->nr && OLC_MOB(dsc) != Proto)
				OLC_MOB(dsc)->nr++;
		}
	}
}

unsigned int MobManager::NumberOfPrototypes()
{
	return (MyMobProto.size());
}

/*
Input:  The index of the mob to be deleted
Output: None
Functionality: Running this should completely erase a mob from the database.
*/
void MobManager::DeletePrototypeFromDatabase( const unsigned int mob_rnum )
{
	std::list<std::string> Queries = MobManager::GetManager().GrabDeleteQuery(mob_rnum);

	try {
		for(std::list<std::string>::iterator sqlIter = Queries.begin();sqlIter != Queries.end();++sqlIter)
		{
			gameDatabase->sendRawQuery( (*sqlIter) );
		}
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "MobManager::DeletePrototypeFromDatabase : %s", e.getMessage().c_str());
		e.report();
		return;
	}
	//If we made it this far, we know the mob was removed successfully.
	Index *MobIndex = GetIndex(mob_rnum);
	if( MobIndex != NULL )
		MobIndex->in_db = false;
}

/* Return a MOB's index number based on the provided vnum */
int MobManager::RealMobile( const unsigned int vnum )
{
	if( MyMobIndex.size() == 0 )
		return -1;
	//At this point we are guaranteed that MyMobIndex.size()-1 will yield a valid result.
	unsigned int bot = 0, top = MyMobIndex.size()-1, mid;

	while( true )
	{
		mid = ((bot + top) / 2);

		if( vnum == (unsigned int)MyMobIndex[mid]->vnum )
			return (int)(mid);
		if( bot >= top )
			return (-1);
		if( (unsigned int)MyMobIndex[mid]->vnum > vnum )
			top = mid - 1;
		else
			bot = mid + 1;
	}
	return -1;//Mob does not exist.
}


int MobManager::VirtualMobile( const unsigned int rnum )
{
	if( rnum > MyMobIndex.size() )
		return -1;//Invalid. rnum also cannot be < 0 because it is unsigned.
	return MyMobIndex[rnum]->vnum;
}

Character *MobManager::GetPrototype( const unsigned int rnum )
{
	if( rnum >= MyMobProto.size() )
		return NULL;
	return MyMobProto[rnum];
}

Index *MobManager::GetIndex( const unsigned int rnum )
{
	if( rnum > MyMobIndex.size() )
		return NULL;
	return MyMobIndex[rnum];
}

Character *MobManager::GetPrototype( Character *LiveMob )
{
	if( LiveMob->nr < 0 || (unsigned int)LiveMob->nr >= MyMobProto.size() )
		return (NULL);
	return (MyMobProto[LiveMob->nr]);
}

Character *MobManager::GetPrototypeByVnum( const unsigned int vnum )
{
	unsigned int rnum = RealMobile( vnum );
	return (GetPrototype(rnum));
}

void MobManager::UpdateLiveMobilesFromPrototype( const unsigned int rnum )
{
	UpdateLiveMobilesFromPrototype( GetPrototype( rnum ) );
}
void MobManager::UpdateLiveMobilesFromPrototype( Character *Prototype )
{
	if( Prototype == NULL ) return;
	for( Character *mob = character_list;mob;mob = mob->next )
	{
		if( IS_MOB(mob) && mob->nr == Prototype->nr ) {
			UpdateLiveMobile( mob, Prototype );
		}
	}
}
void MobManager::UpdateLiveMobile( Character *Live, Character *Prototype )
{
	//Update...
	GET_ALIAS(Live) 				= GET_ALIAS(Prototype);
	GET_SDESC(Live) 				= GET_SDESC(Prototype);
	GET_LDESC(Live) 				= GET_LDESC(Prototype);
	GET_DDESC(Live) 				= GET_DDESC(Prototype);
	Live->player.ArriveMessage		= Prototype->player.ArriveMessage;
	Live->player.LeaveMessage		= Prototype->player.LeaveMessage;

	Live->DeleteClans();
	Live->userClans = ClanUtil::cloneUserClansFromMobPrototype(Prototype);
}

Character *MobManager::AllocateRawMobile() {
	Character *NewMob = new Character();
	NewMob->MobData = new MobOnlyData();
	SET_BITK(MOB_FLAGS(NewMob), Q_BIT(MOB_ISNPC));
	return NewMob;
}

//Input:  The source prototype, and the prototype which should be copied from the source.
//Output: None
//Functionality: The Destination should be a replicate of the Source.
void MobManager::CopyPrototype( Character *Destination, Character *Source )
{
	// Free up any used strings.
	if (Destination->player.short_descr)
		delete[] (Destination->player.short_descr);
	if (Destination->player.description)
		delete[] (Destination->player.description);
	if (Destination->player.long_descr)
		delete[] (Destination->player.long_descr);
	if (Destination->player.ArriveMessage)
		delete[] (Destination->player.ArriveMessage);
	if (Destination->player.LeaveMessage)
		delete[] (Destination->player.LeaveMessage);

	if( !Destination->MobData )
		Destination->MobData = new MobOnlyData();
	if(Destination->MobData->Food)
	{
		delete Destination->MobData->Food;
		Destination->MobData->Food = NULL;
	}
	if(Destination->MobData->Skin)
	{
		delete Destination->MobData->Skin;
		Destination->MobData->Skin = NULL;
	}

	if(Destination->MobData)
		delete Destination->MobData;
	// Copy mob over.
	*Destination = *Source;

	Destination->MobData = new MobOnlyData(Source->MobData);

#ifdef KINSLAYER_JAVASCRIPT
	Destination->js_scripts = std::shared_ptr<std::vector<JSTrigger*> >(new std::vector<JSTrigger*>());
	for( unsigned int i = 0;i < Source->js_scripts->size();++i )
	{
		Destination->js_scripts->push_back( Source->js_scripts->at( i ) );
	}
#endif
}

/* The following do not belong to the MobManager class, but are methods specific to dealing with MOBs */

/*
Input:  None
Output: Returns the prototype for this specific MOB. NULL if (this) is a PC or has no prototype
*/
Character *Character::GetPrototype()
{
	return (MobManager::GetManager().GetPrototype(this));
}

/*
Input:  None
Output: The MOB's vnum. (-1) if this is a PC or invalid mobile(index not found)
*/
int Character::getVnum() const
{
	Index *index;

	if( this->nr < 0 || (index = MobManager::GetManager().GetIndex((unsigned int)this->nr)) == NULL )
		return (0);
	return index->vnum;
}
