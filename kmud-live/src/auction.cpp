/********************************************************************
 *	auction.cpp - Implements the AuctionManager classes				*
 *																	*
 *	Coded by Galnor in September of 2009 (Sortak@gmail.com)			*
 *																	*
 ********************************************************************/

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "utils.h"
#include "constants.h"
#include "db.h"
#include "clans.h"
#include "olc.h"
#include "auction.h"
#include "handler.h"
#include "screen.h"

#include "MiscUtil.h"
#include "StringUtil.h"
#include "Descriptor.h"

std::vector<int> Auction::DurationList;
int AuctionManager::ItemsPerPage = 20;

void AuctionParse( Descriptor *d, const std::string &arg );
void AuctionDispMenu( Descriptor *d );


/******************************************************************************************
 *                                                                                        *
 *                          AUCTION MANAGER IMPLEMENTATION                                *
 *                                                                                        *
 ******************************************************************************************/

AuctionManager *AuctionManager::Self = (NULL);

/* Constructor for the Auction Manager. This is private and only called via AuctionManager::GetManager() */
AuctionManager::AuctionManager()
{
	Auction::DurationList.push_back( 60 * 60 * 6 );   //6 hours
	Auction::DurationList.push_back( 60 * 60 * 12 );  //12 hours
	Auction::DurationList.push_back( 60 * 60 * 18 );  //18 hours
	Auction::DurationList.push_back( 60 * 60 * 24 );  //One day
	Auction::DurationList.push_back( 60 * 60 * 48 );  //Two days
	Auction::DurationList.push_back( 60 * 60 * 168 ); //One week
}
/* Cleanup for the Auction Manager. This is a private member and is called via AuctionManager::Free() */
AuctionManager::~AuctionManager()
{
	for( std::map<int,Auction*>::iterator aIter = MyAuctions.begin();aIter != MyAuctions.end();++aIter )
	{
		delete (*aIter).second;
	}
	MyAuctions.clear();
}
/* Return a reference to the AuctionManager. If one does not exist, it will create it and then return. */
AuctionManager &AuctionManager::GetManager()
{
	if( Self == NULL )
		Self = new AuctionManager();
	return (*Self);
}
/* Delete the Auction Manager. Cleanup goes in the destructor. This is the public accessor for destruction */
void AuctionManager::Free()
{
	if( Self != NULL )
		delete (Self);
	Self = (NULL);
}
/* Given the sql::Row MyRow, load data into Auction object and return it. */
Auction *AuctionManager::BootOneAuction( sql::Row MyRow )
{
	Auction *a = new Auction( atoi(MyRow["vnum"].c_str()), MyRow["name"], MiscUtil::Convert<unsigned long long>(MyRow["aRaces"]),
		MiscUtil::Convert<unsigned long long>(MyRow["dClans"]) );
	return (a);
}

/* Boot the auctions directly from the database */
void AuctionManager::Boot()
{
	sql::Query MyQuery;

	try {
		MyQuery = gameDatabase->sendQuery("SELECT ai.* FROM auctions ai WHERE 1 ORDER BY vnum ASC;");
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Failed to boot auctions from database: %s", e.getMessage().c_str());
		return;
	}
	while( MyQuery->hasNextRow() )
	{
		sql::Row MyRow = MyQuery->getRow();
		AddAuction( BootOneAuction( MyRow ) );
	}
}
/* Save all of the auctions in the manager to the database */
void AuctionManager::Save()
{
	for( std::map<int,Auction*>::iterator aIter = MyAuctions.begin();aIter != MyAuctions.end();++aIter )
		(*aIter).second->Save();
}
/* Return the auction whose vnum is equal to _vnum. Otherwise, return NULL if no match is found. */
Auction *AuctionManager::GetAuction( const int _vnum )
{
	return (MyAuctions.find( _vnum ) == MyAuctions.end() ? (NULL) : (MyAuctions[_vnum]));
}

/* Return an ordered copy of the auctions stored in the manager */
std::list< Auction * > &AuctionManager::GetListOfAuctions()
{
	static std::list<Auction *> AList;
	AList.clear();
	for( std::map<int,Auction*>::iterator aIter = MyAuctions.begin();aIter != MyAuctions.end();++aIter )
		AList.push_back( (*aIter).second );
	return AList;
}

/* Add auction, with key value equal to the auction's virtual number */
void AuctionManager::AddAuction( Auction *a )
{
	MyAuctions[ a->getVnum() ] = a;
}
/* Remove an auction from the manager. Returns an iterator one beyond where the removed auction was in the map. */
std::map<int,Auction*>::iterator AuctionManager::RemoveAuction( Auction *a )
{
	std::map<int,Auction*>::iterator aIter;
	for(aIter = MyAuctions.begin();aIter != MyAuctions.end();++aIter)
	{
		if( (*aIter).second == (a) )
		{//Should only be one match.
			MyAuctions.erase(aIter);
			return MyAuctions.end();
//			return MyAuctions.erase(aIter);
		}
	}
	return (aIter);//std::map<int,Auction*>::end()
}
/* Remove any auctions from the manger whose vnum matches the _vnum.  Returns an iterator one beyond where the removed auction was in the map. */
std::map<int,Auction*>::iterator AuctionManager::RemoveAuction( const int _vnum )
{
	std::map<int,Auction*>::iterator aIter;
	if( (aIter = MyAuctions.find(_vnum)) != MyAuctions.end() )
		MyAuctions.erase(aIter);//Keep deleting while we find auctions of the correct vnum.
	return (aIter);//std::map<int,Auction*>::end()
}
/* Returns the number of auctions held by the manager */
unsigned int AuctionManager::NumAuctions()
{
	return MyAuctions.size();
}

/* Setup the user's auction inteface. Uses the OLC system. */
void AuctionManager::SetupAuctionInterface( Character *ch, Auction *a )
{
	ch->desc->olc = new OLC();

	ch->desc->olc->auction_data = new AuctionData();
	OLC_ZONE(ch->desc) = 0;
	ch->desc->olc->auction = a;
	STATE(ch->desc) = CON_AUCTION;

	AuctionDispMenu( ch->desc );
}

/* Cycle through the auction houses and perform any maintenance that is needed */
void AuctionManager::UpdateAuctions()
{
	std::stringstream QueryBuffer;
	sql::Query MyQuery;
	sql::Row MyRow;
	time_t curTime = time(0);
	int winnerID;
	int auctionID;

	try {
		//Select all auctions that have expired.
		QueryBuffer << "SELECT ai.*,"
			<< " (SELECT ab.bidder_id FROM auctionBid ab WHERE ab.auction_id=ai.auction_id"
			<< " ORDER BY ab.bid_amount DESC LIMIT 1) AS winner_id"
			<< " FROM auctionItem ai"
			<< " WHERE ai.end_time <= '" << curTime << "' AND ai.active='1';";

		MyQuery = gameDatabase->sendQuery(QueryBuffer.str());
		//Notify all of the winners of their victory!
		while( MyQuery->hasNextRow() )
		{
			MyRow = MyQuery->getRow();
			winnerID = (MyRow["winner_id"].empty() ? -1 : atoi(MyRow["winner_id"].c_str()));
			auctionID = atoi(MyRow["id"].c_str());

			if( winnerID != -1 )
			{//The auction has a winner.
				Character *winner;
				if( (winner = get_char_by_id(winnerID)) != 0 )
				{//Notify the winner(they are logged in)
					winner->Send("You have won an auction that has recently ended.\r\n");
				}
				AuctionManager::GetManager().ReimburseLosers( auctionID, winnerID );
				AuctionManager::GetManager().RewardOwner( auctionID );
			}
			else
			{//The auction does not have a winner.
				int ownerID = atoi(MyRow["owner_id"].c_str());
				Character *owner;
				if( (owner = get_char_by_id(ownerID)) != 0 )
				{//Notify the owner if they are loged in.
					owner->Send("%s%sOne of your auctions has ended without any bidders\r\n"
						"You may retrieve your item at the auction house.%s\r\n",
						COLOR_BOLD(owner,CL_COMPLETE),COLOR_MAGENTA(owner,CL_COMPLETE),
						COLOR_NORMAL(owner,CL_COMPLETE));
				}
			}
		}
		QueryBuffer.str("");

		//Set auctions to inactive if their time has expired.
		QueryBuffer << "UPDATE auctionItem SET active='0' WHERE end_time <= '" << curTime << "';";
		gameDatabase->sendRawQuery(QueryBuffer.str());
//		QueryBuffer.str("");
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "ActionManager::UpdateAuctions : Failed to run maintenance queries : %s",
			e.getMessage().c_str());
		return;
	}
}
void AuctionManager::RewardOwner( const int ai_id )
{
	//First we need to get the ID of the bid owner.
	std::stringstream QueryBuffer;
	sql::Query MyQuery;
	sql::Row MyRow;
	int owner_id;
	long long bid_amount;
	Character *owner;

	QueryBuffer << "SELECT ai.owner_id,ab.bid_amount FROM auctionItem ai JOIN auctionBid ab ON ab.ai_id=ai.id"
		" WHERE ai.id='" << ai_id << "' ORDER BY ab.bid_amount DESC LIMIT 1;";
	try {
		MyQuery = gameDatabase->sendQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "AuctionManager::RewardWinner : Failed to send query : %s",
			e.getMessage().c_str());
		return;
	}
	if( MyQuery->numRows() <= 0 )
	{
		return;//wtf? Someone is Narging the database.
	}
	MyRow = MyQuery->getRow();
	owner_id = atoi(MyRow["owner_id"].c_str());
	bid_amount = MiscUtil::Convert<long long>(MyRow["bid_amount"].c_str());

	QueryBuffer.str("");

	owner = get_char_by_id(owner_id);

	if( owner == 0 )
	{//Owner is not online.
		QueryBuffer << "UPDATE users SET bank_gold = bank_gold + '" << bid_amount << "' WHERE user_id='"
			<< owner_id << "';";
		gameDatabase->sendRawQuery(QueryBuffer.str());
	}
	else
	{//Owner IS online. Let's notify them of their sale!
		owner->Send("%s%sAn auction of yours has completed. You have earned %lld coppers!%s\r\n",
			COLOR_BOLD(owner,CL_COMPLETE), COLOR_MAGENTA(owner,CL_COMPLETE),
			bid_amount, COLOR_NORMAL(owner,CL_COMPLETE));
		owner->points.bank_gold += bid_amount;
	}
	
}
void AuctionManager::ReimburseLosers( const int ai_id, const int winnerID )
{
	std::stringstream QueryBuffer;
	sql::Query MyQuery;
	sql::Row MyRow;
	try {
		//We need to reimburse everyone who lost the auction.
		QueryBuffer << "SELECT ab.* FROM auctionBid ab WHERE ab.ai_id='" << ai_id << "'"
			<< " ORDER BY ab.bid_amount DESC;";
		MyQuery = gameDatabase->sendQuery(QueryBuffer.str());

		std::list<int> AlreadyReimbursed;
		while( MyQuery->hasNextRow() )
		{
			MyRow = MyQuery->getRow();
			int bidderID = atoi(MyRow["bidder_id"].c_str());

			if( bidderID == winnerID )
				continue;//The winner does not get reimbursed.
			if( std::find(AlreadyReimbursed.begin(),AlreadyReimbursed.end(),bidderID) != AlreadyReimbursed.end() )
				continue;
			AlreadyReimbursed.push_back(bidderID);

			Character *bidder = get_char_by_id(bidderID);

			if( bidder == 0 )
			{//They are offline. Update their bank account in the DB.
				QueryBuffer.str("");
				QueryBuffer << "UPDATE users SET bank_gold = bank_gold+'" << MyRow["bid_amount"]
					<< "' WHERE user_id='" << bidderID << "';";
				gameDatabase->sendRawQuery(QueryBuffer.str());
			}
			else
			{
				bidder->points.bank_gold += MiscUtil::Convert<long long>(MyRow["bid_amount"].c_str());
				bidder->Send("%s%sYou have been reimbursed %lld coppers from a recently completed auction.%s\r\n",
					bld, mag, MiscUtil::Convert<long long>(MyRow["bid_amount"].c_str()), nrm);
			}
		}
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "AuctionManager::ReimburseUsers : Failed to send query : %s",
			e.getMessage().c_str());
	}
}

/******************************************************************************************
 *                                                                                        *
 *                              AUCTION IMPLEMENTATION                                    *
 *                                                                                        *
 ******************************************************************************************/
Auction::Auction()
{
	vnum = (-1);
}
Auction::Auction( const int _vnum, const std::string &_Name,
				 const unsigned long long _aRaces, const unsigned long long _dClans)
{
	vnum = _vnum;
	Name = _Name;
	for(unsigned int i = 0;i < aRaces.size();++i)
		if( IS_SET(_aRaces, (1<<i)) )
			aRaces.set(i);
	for(unsigned int i = 0;i < dClans.size();++i)
		if( IS_SET(_dClans, (1<<i)) )
			dClans.set(i);
}
Auction::~Auction()
{//Do nothing
}

/* Returns auction's name */
std::string Auction::getName()
{
	return (Name);
}
/* Return auction's vnum */
const int Auction::getVnum()
{
	return (vnum);
}

/* Set auction's vnum to _vnum */
void Auction::setVnum( const int _vnum )
{
	vnum = (_vnum);
}
/* Set auction's name to _Name */
void Auction::setName( const std::string _Name )
{
	Name = (_Name);
}

/* Returns true if clan of vnum cl_vnum is not disallowed */
bool Auction::clanIsAllowed( const int cl_vnum )
{
	return ( !(dClans.test( cl_vnum )) );
}
/* Returns true if race #r_num is set to allowed. */
bool Auction::raceIsAllowed( const int r_num )
{
	return (aRaces.test( r_num ));
}

/* Set race #r_num to allowed */
void Auction::allowRace( const int r_num )
{
	aRaces.set( r_num );
}
/* Set clan vnum cl_vnum's disallowed state to false */
void Auction::allowClan( const int cl_vnum )
{
	dClans.reset( cl_vnum );
}

/* Set race #r_num to a disallowed state */
void Auction::disallowRace( const int r_num )
{
	aRaces.reset( r_num );
}

/* Set clan vnum cl_vnum's disallowed state to true */
void Auction::disallowClan( const int cl_num )
{
	dClans.set( cl_num );
}

/* Save individual auction to the database */
void Auction::Save()
{
	std::stringstream QueryBuffer;
	unsigned long long ar=0, dc=0;

	for(unsigned int i = 0;i < this->aRaces.size();++i)
		if( this->aRaces.test(i) == true )
			SET_BITK(ar,(1<<i));
	for(unsigned int i = 0;i < this->dClans.size();++i)
		if( this->dClans.test(i) == true )
			SET_BITK(dc,(1<<i));

	QueryBuffer << "REPLACE DELAYED INTO auctions SET vnum='" << getVnum() << "',Name='" << sql::escapeString(getName())
		<< "',aRaces='" << ar << "',dClans='" << dc << "';";
	try {
		gameDatabase->sendRawQuery( QueryBuffer.str() );
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Unable to save auction #%d : %s", getVnum(), e.getMessage().c_str());
		return;
	}
}
/* Return a copy of the allowed races */
std::bitset<16> Auction::bs_aRaces()
{
	return (aRaces);
}
/* Return a copy of the disallowed clans */
std::bitset<64> Auction::bs_dClans()
{
	return (dClans);
}
/* Return a comma separated list of all races that are set to an 'allowed' state.
 * Color preferences are determined only if 'ch' is passed */
std::string Auction::aRacesStr( Character *ch )
{
	char rBuf[1024*16];
	if( ch != NULL )
		get_char_cols( ch );
	sprintbit(this->bs_aRaces().to_ulong(), pc_race_types, rBuf, ", ", cyn, nrm);
	return rBuf;
}
/* Return a comma separated list of all clans that are set to an 'allowed' state.
 * Color preferences are determined only if 'ch' is passed */
std::string Auction::dClansStr( Character *ch )
{
	if( ch != NULL )
		get_char_cols( ch );
	std::string cBuf;
	for(Clan *c = ClanList;c;c=c->Next)
	{
		if( this->clanIsAllowed( c->vnum ) )
		{
			if( cBuf.size() )
				cBuf += ", ";
			cBuf += cyn;
			cBuf += c->Name;
			cBuf += nrm;
		}
	}
	if( cBuf.size() == 0 )
		cBuf = "NOBITS";
	return cBuf;
}


/* Submit an item to the auction */
bool Auction::placeAuction( Character *ch, AuctionData *ad )
{
	std::stringstream QueryBuffer, QueryBuffer2;
	Object *obj = ad->GetItemToSell();

	if( obj == NULL || obj->carried_by != ch )
		return false;//Item somehow vanished in the duration of the editor. Stolen & rented? Purged?
	obj_from_char(obj);

	QueryBuffer << "INSERT INTO auctionItem (auction_id,object_id,owner_id,end_time,starting_price,buyout_price,active,timestamp) VALUES(";
	QueryBuffer << SQLVal(this->getVnum());
	QueryBuffer << SQLVal(ToString(obj->objID));
	QueryBuffer << SQLVal(ch->player.idnum);
	QueryBuffer << SQLVal(time(0) + ad->GetSellDuration());
	QueryBuffer << SQLVal(ad->GetStartingPrice());
	QueryBuffer << SQLVal(ad->GetBuyoutPrice());
	QueryBuffer << SQLVal("1");
	QueryBuffer << SQLVal(time(0),true);
	QueryBuffer << ");";

	Object::saveItemToTopLevelHolder('A', ToString(this->getVnum()), obj);

	try {
		gameDatabase->sendRawQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, MAX(GET_INVIS_LEV(ch), LVL_APPR), TRUE, "%s failed to place item into auction #%d (item: %s): %s",
			GET_NAME(ch), this->getVnum(), obj->GetSDesc(), e.getMessage().c_str());
		obj_to_char(obj, ch);
		ch->Send("There was an error with your request. Please report this!");
		return true;//Red herring, but the error has been dealt with.
	}
	obj->Extract(false);
	return true;
}

bool Auction::placeBid( Character *bidder, class AuctionData *ad )
{
	int itemID = ad->GetSelectedItem()->GetID();
	long long bid = ad->GetBidRequest();
	long bidder_id = bidder->player.idnum;
	int auction_id = this->getVnum();
	time_t timestamp = time(0);
	std::stringstream QueryBuffer;

	QueryBuffer << "INSERT INTO auctionBid (auction_id,object_id,bidder_id,bid_amount,timestamp,ai_id) VALUES (";
	QueryBuffer << SQLVal(auction_id);
	QueryBuffer << SQLVal(ad->GetSelectedItem()->GetObjID());
	QueryBuffer << SQLVal(bidder_id);
	QueryBuffer << SQLVal(bid);
	QueryBuffer << SQLVal(timestamp);
	QueryBuffer << SQLVal(itemID,true);
	QueryBuffer << ");";

	try {
		gameDatabase->sendRawQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, MAX(LVL_APPR,GET_LEVEL(bidder)), TRUE, "SYSERR : %s failed to place bid : %s",
			GET_NAME(bidder), e.getMessage().c_str());
		return false;
	}

	//Take the gold from the user.
	bidder->points.gold -= (bid - ad->GetSelectedItem()->GetLastBid(bidder->player.idnum));

	//If this bid matched the buyout, then we'll end the auction
	if( ad->GetSelectedItem()->GetBuyout() > 0 && bid >= ad->GetSelectedItem()->GetBuyout() )
	{
		QueryBuffer.str("");
		QueryBuffer << "UPDATE auctionItem SET active='0' WHERE id='"
			<< itemID << "';";
		try {
			gameDatabase->sendRawQuery(QueryBuffer.str());
		} catch( sql::QueryException e ) {
			MudLog(BRF, MAX(LVL_APPR,GET_LEVEL(bidder)), TRUE, "SYSERR : %s failed to place bid : %s",
				GET_NAME(bidder), e.getMessage().c_str());
			return false;
		}
		AuctionManager::GetManager().ReimburseLosers(itemID, bidder->player.idnum);
			AuctionManager::GetManager().RewardOwner( itemID );
	}
	return true;
}
void Auction::retrieveItem(Descriptor *d, class AuctionData *ad )
{
	Object *obj = Object::loadSingleItem( ad->GetSelectedItem()->GetObjID(), true );
	if( obj != 0 )
	{//There should only be one item...
		obj_to_char(obj, d->character);
	}
	d->character->itemSave();

	//Mark the item as retrieved.
	std::stringstream QueryBuffer;
	QueryBuffer << "UPDATE auctionItem SET retrieved='1' WHERE id='" << ad->GetSelectedItem()->GetID() << "';";
	try {
		gameDatabase->sendRawQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Error attempting to mark auction item as retrieved(Auction Item #%d) : %s",
			ad->GetSelectedItem()->GetID(), e.getMessage().c_str());
	}
	ad->GetSelectedItem()->SetRetrieved();
	delete (ad->GetSelectedItem());
	ad->aiCache.erase(std::remove(ad->aiCache.begin(), ad->aiCache.end(), ad->GetSelectedItem()),ad->aiCache.end());
}

/* Given an OLC AuctionData object, load items from this auction house */
std::vector< AuctionItem* > Auction::loadItems( class AuctionData* Filter )
{
//	std::list< Object * > ItemList = this->loadItemList( false );
	std::vector< AuctionItem* > lAuctionItems;

	std::stringstream QueryBuffer;
	sql::Query MyQuery;

	QueryBuffer << "SELECT os.sdesc AS objShortDesc_Special,op.sdesc AS objShortDesc,o.id AS objID,ai.*"
		<< " FROM objects o JOIN auctionItem ai ON ai.object_id=o.id LEFT JOIN obj_protos op ON op.vnum=o.vnum"
		<< " LEFT JOIN object_specials os ON os.id=o.id"
		<< " WHERE ai.auction_id='" << this->getVnum() << "' AND ai.active='1' AND ai.end_time > '" << time(0) << "'"
		<< " ORDER BY ai.end_time ASC;";
	try {
		MyQuery =  gameDatabase->sendQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Failed to load items for auction #%d : %s",
			this->getVnum(), e.getMessage().c_str());
		return lAuctionItems;
	}

	AuctionItem *ai;
	while( MyQuery->hasNextRow() )
	{
		ai = new AuctionItem( MyQuery->getRow() );

		//Make sure that the auction has not ended.
		if( !ai->IsActive() || time(0) > ai->GetEndTime() )
		{
			delete (ai);
			continue;
		}
		lAuctionItems.push_back( ai );
	}
	return lAuctionItems;
}
std::vector< AuctionItem* > Auction::loadMyBids(Descriptor *d, class AuctionData *ad)
{
//	std::list< Object * > ItemList = this->loadItemList( false );
	std::vector< AuctionItem* > lAuctionItems;

	std::stringstream QueryBuffer;
	sql::Query MyQuery;

	QueryBuffer << "SELECT os.sdesc AS objShortDesc_Special,op.sdesc AS objShortDesc,o.id AS objID,ai.*"
//		<< " (SELECT ab.bidder_id FROM auctionBid ab WHERE ab.ai_id=ai.id ORDER BY bid_amount DESC LIMIT 1) AS top_bidder"
		<< " FROM objects o JOIN auctionItem ai ON ai.object_id=o.id LEFT JOIN obj_protos op ON op.vnum=o.vnum"
		<< " LEFT JOIN object_specials os ON os.id=o.id"
		<< " WHERE ai.auction_id='" << this->getVnum() << "'"
		<< " AND ai.retrieved='0'"
		<< " ORDER BY ai.end_time ASC;";
	try {
		MyQuery =  gameDatabase->sendQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Failed to load items for auction #%d : %s",
			this->getVnum(), e.getMessage().c_str());
		return lAuctionItems;
	}

	AuctionItem *ai;
	sql::Row MyRow;
	while( MyQuery->hasNextRow() )
	{
		MyRow = MyQuery->getRow();

//		if( atoi(MyRow["top_bidder"].c_str()) == d->character->player.idnum )
//		{
		ai = new AuctionItem( MyRow );
		if( ai->GetLastBid( d->character->player.idnum ) > 0 )
			lAuctionItems.push_back( ai );
		else
			delete ai;
//		}
	}
	return lAuctionItems;
}
std::vector< AuctionItem* > Auction::loadMyAuctions(Descriptor *d, AuctionData *ad)
{
	std::vector< AuctionItem* > lAuctionItems;

	std::stringstream QueryBuffer;
	sql::Query MyQuery;

	QueryBuffer << "SELECT os.sdesc AS objShortDesc_Special,op.sdesc AS objShortDesc,o.id AS objID,ai.*"
//		<< " (SELECT ab.bidder_id FROM auctionBid ab WHERE ab.ai_id=ai.id ORDER BY bid_amount DESC LIMIT 1) AS top_bidder"
		<< " FROM objects o JOIN auctionItem ai ON ai.object_id=o.id LEFT JOIN obj_protos op ON op.vnum=o.vnum"
		<< " LEFT JOIN object_specials os ON os.id=o.id"
		<< " WHERE ai.auction_id='" << this->getVnum() << "'"
		<< " AND ai.retrieved='0'"
		<< " ORDER BY ai.end_time ASC;";
	try {
		MyQuery =  gameDatabase->sendQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Failed to load items for auction #%d : %s",
			this->getVnum(), e.getMessage().c_str());
		return lAuctionItems;
	}

	AuctionItem *ai;
	sql::Row MyRow;
	while( MyQuery->hasNextRow() )
	{
		MyRow = MyQuery->getRow();

		ai = new AuctionItem( MyRow );
		if( ai->GetOwnerID() == d->character->player.idnum )
			lAuctionItems.push_back( ai );
		else
			delete ai;
	}
	return lAuctionItems;
}
/* Load all items held by this auction from the database */
std::list< Object* > Auction::loadItemList( bool recursive )
{
	return Object::loadItemList( recursive, 'A', ToString(this->getVnum()) );
}

void AuctionItem::Zero()
{
	starting	= (0);
	endTime		= (0);
	buyout		= (0);
	ownerID		= (0);
	is_active	= (false);
	is_retrieved= (false);
}
AuctionItem::AuctionItem()
{
	Zero();
}
AuctionItem::AuctionItem( const sql::Row &MyRow )
{
	Zero();

	std::stringstream QueryBuffer;
	sql::Query MyQuery;
	QueryBuffer << "SELECT ab.*,u.username,u.user_id AS uid FROM auctionBid ab JOIN users u ON u.user_id=ab.bidder_id WHERE ai_id='"
		<< MyRow["id"] << "' ORDER BY bid_amount DESC;";

	try {
		this->BidQuery = gameDatabase->sendQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Could not load list of auction bids : %s", e.getMessage().c_str());
		return;
	}
	this->endTime = atol(MyRow["end_time"].c_str());
	this->buyout = MiscUtil::Convert<long long>(MyRow["buyout_price"]);
	this->starting = MiscUtil::Convert<long long>(MyRow["starting_price"]);
	this->is_active = StringUtil::parseBoolean(MyRow["active"]);
	this->objID = boost::uuids::string_generator()((MyRow["objID"]));
	this->id = MiscUtil::Convert<int>(MyRow["id"]);
	this->is_retrieved = StringUtil::parseBoolean(MyRow["retrieved"]);
	this->ownerID = MiscUtil::Convert<int>(MyRow["owner_id"]);
	if( MyRow["objShortDesc"].empty() )
		this->objShortDesc = (MyRow["objShortDesc_SPECIAL"]);
	else
		this->objShortDesc = (MyRow["objShortDesc"]);
}
AuctionItem::~AuctionItem()
{
}
const long long AuctionItem::GetNextMinBid()
{
	return GetTopBid() + 100;
}
const long long AuctionItem::GetTopBid()
{
	if( BidQuery && BidQuery->numRows() )
		return MiscUtil::Convert<long long>(BidQuery->peekRow()["bid_amount"]);
	else
		return starting;
}
const long long AuctionItem::GetLastBid( const int uid )
{
	long long bid = 0;
	while( BidQuery->hasNextRow() )
	{
		sql::Row MyRow = BidQuery->getRow();
		if( atoi(MyRow["uid"].c_str()) == uid )
		{
			bid = atoi(MyRow["bid_amount"].c_str());
			break;
		}
	}
	BidQuery->resetRowQueue();
	return bid;
}
/** Galnor 12/25/2009 - Reload the auction's data from the database **/
void AuctionItem::Refresh()
{
	std::stringstream QueryBuffer;
	sql::Query MyQuery;

	QueryBuffer << "SELECT os.sdesc AS objShortDesc_Special,op.sdesc AS objShortDesc,o.id AS objID,ai.*"
		<< " FROM objects o JOIN auctionItem ai ON ai.object_id=o.id LEFT JOIN obj_protos op ON op.vnum=o.vnum"
		<< " LEFT JOIN object_specials os ON os.id=o.id"
		<< " WHERE ai.id='" << this->GetID() << "' AND ai.active='1' AND ai.end_time > '" << time(0) << "'"
		<< " ORDER BY ai.end_time ASC;";
	try {
		MyQuery = gameDatabase->sendQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		MudLog(BRF, LVL_APPR, TRUE, "Failed to refresh auction item : %s", e.getMessage().c_str());
		return;
	}

	if( MyQuery->numRows() )
	{
		AuctionItem aiTemp = AuctionItem(MyQuery->getRow());

		this->buyout = aiTemp.GetBuyout();
		this->endTime = aiTemp.GetEndTime();
		this->starting = aiTemp.GetStartingBid();
		this->is_active = aiTemp.IsActive();
		this->is_retrieved = aiTemp.IsRetrieved();
		this->ownerID = aiTemp.GetOwnerID();

		this->BidQuery = aiTemp.BidQuery;
	}
	else
	{//This is bad. The item no longer exists in the database?
		this->Zero();//This will render the auction invalid.
	}
}
bool AuctionItem::CanBeRetrievedBy( Character *user )
{
	if( IsActive() || IsRetrieved() )
		return false;
	if( GetTopBidderName().empty() && GetOwnerID() == user->player.idnum )
		return true;
	if(	GetTopBidderID() == user->player.idnum )
		return true;
	return false;
}
const std::string AuctionItem::GetTopBidderName()
{
	return (this->BidQuery->numRows() ? this->BidQuery->peekRow()["username"] : "");
}
const int AuctionItem::GetTopBidderID()
{
	return (this->BidQuery->numRows() ? atoi(this->BidQuery->peekRow()["uid"].c_str()) : -1);
}

/******************************************************************************************
 *                                                                                        *
 *                                    AUCTION OLC                                         *
 *                                                                                        *
 ******************************************************************************************/

void AuctionDispMenu( Descriptor *d )
{
	get_char_cols(d->character);
	d->Send("Welcome to %s!\r\n\r\n", StringUtil::vaEscape(d->olc->auction->getName()).c_str());

//	d->Send("%s1%s) Search Items\r\n" , grn, nrm);
	d->Send("%s1%s) View Your Bids\r\n", grn, nrm);
	d->Send("%s2%s) View Your Auctions\r\n", grn, nrm);
	d->Send("%s3%s) Sell an Item\r\n", grn, nrm);
	d->Send("%s4%s) Browse\r\n", grn, nrm);
	d->Send("\r\n%sQ%s)uit\r\n", grn, nrm);

	OLC_MODE(d) = AUCTION_MENU;

	d->olc->auction_data->ClearSellData();
}
void AuctionDispBuyMenu( Descriptor *d )
{
	char atStr[1024];
	sprintbit(d->olc->auction_data->item_types.to_ulong(), atTypeStr, atStr, ", ", cyn, nrm);
	d->Send("%s1%s) Item Type     : %s\r\n", grn, nrm, atStr);
	d->Send("%s2%s) Search Term   : %s%s%s\r\n", grn, nrm, cyn, d->olc->auction_data->GetSearchTerm().c_str(), nrm);
	d->Send("%s3%s) Search\r\n", grn, nrm);
	d->Send("%s4%s) Quit\r\n", grn, nrm);

	d->Send("\r\nMoney: %s%s%s\r\n", cyn, d->character->GoldString(d->character->points.gold,false).c_str(),nrm);
	OLC_MODE(d) = AUCTION_BUY_MENU;
}
void AuctionDispBuyTypeMenu( Descriptor *d )
{
	get_char_cols(d->character);
	char typesSet[1024];

	std::bitset<8> bs;

	for(int i = 0;i < d->olc->auction_data->item_types.size();++i)
	{
		d->Send("%s%d%s) %s%s%s\r\n", grn, (i+1), nrm, cyn, atTypeStr[i], nrm);
	}

	sprintbit(d->olc->auction_data->item_types.to_ulong(), atTypeStr, typesSet, ", ", cyn);
	d->Send("\r\n%sQ%s) %sExit%s\r\n", grn, nrm, cyn, nrm);
	d->Send("Types: %s\r\n", typesSet);

	OLC_MODE(d) = AUCTION_BUY_TYPE;
}
void AuctionDispSellMenu( Descriptor *d )
{
	Object *itemToSell = d->olc->auction_data->GetItemToSell();
	Time Duration = Time(d->olc->auction_data->GetSellDuration());
	std::stringstream DurationStr;

	DurationStr << Duration.dDays() << " day(s), " << Duration.dHours() << " hour(s)";

	d->Send("~~Sell an Item~~\r\n\r\n");
	d->Send("%sI%s)tem to Sell   : %s%s%s\r\n", grn, nrm, cyn,
		(itemToSell == NULL ? "<NONE>" : itemToSell->GetSDesc()), nrm);
	d->Send("%sD%s)uration       : %s%s%s\r\n", grn, nrm, cyn, DurationStr.str().c_str(), nrm);
	d->Send("%sB%s)uyout Price   : %s%s%s\r\n", grn, nrm, cyn,
		d->character->GoldString(d->olc->auction_data->GetBuyoutPrice(), false).c_str(), nrm);
	d->Send("%sS%s)tarting Price : %s%s%s\r\n", grn, nrm, cyn,
		d->character->GoldString(d->olc->auction_data->GetStartingPrice(), false).c_str(), nrm);
	d->Send("%sP%s)lace Auction\r\n", grn, nrm);
	d->Send("%sQ%s)uit\r\n", grn, nrm);
	d->Send("\r\nYour option: ");

	OLC_MODE(d) = AUCTION_SELL_MENU;
}

void AuctionDispDurationMenu( Descriptor *d )
{
	d->Send("Please select a duration for the auction:\r\n");
	for(unsigned int i = 0;i < Auction::DurationList.size();++i)
	{
		Time Duration = Time(Auction::DurationList[i]);

		d->Send("%s%d%s) %s%d%s day(s), %s%2d%s hour(s)\r\n",
			grn, i+1, nrm, cyn, (int)Duration.dDays(), nrm, cyn, (int)Duration.dHours(), nrm);
	}
	OLC_MODE(d) = AUCTION_DURATION_MENU;
}
void AuctionDispBuyoutMenu( Descriptor *d )
{
	d->Send("Buyout Price:\r\n");
	d->Send("%sG%s)old     : %s%d%s\r\n", grn, nrm, cyn, CalcGold(d->olc->auction_data->GetBuyoutPrice()), nrm);
	d->Send("%sS%s)ilver   : %s%d%s\r\n", grn, nrm, cyn, CalcSilver(d->olc->auction_data->GetBuyoutPrice()), nrm);
	d->Send("%sC%s)opper   : %s%d%s\r\n", grn, nrm, cyn, CalcCopper(d->olc->auction_data->GetBuyoutPrice()), nrm);
	d->Send("%sQ%s)uit\r\n", grn, nrm);
	d->Send("Your option : ");

	OLC_MODE(d) = AUCTION_SELL_BUYOUT;
}
void AuctionDispStartingPriceMenu( Descriptor *d )
{
	d->Send("Buyout Price:\r\n");
	d->Send("%sG%s)old     : %s%d%s\r\n", grn, nrm, cyn, CalcGold(d->olc->auction_data->GetStartingPrice()), nrm);
	d->Send("%sS%s)ilver   : %s%d%s\r\n", grn, nrm, cyn, CalcSilver(d->olc->auction_data->GetStartingPrice()), nrm);
	d->Send("%sC%s)opper   : %s%d%s\r\n", grn, nrm, cyn, CalcCopper(d->olc->auction_data->GetStartingPrice()), nrm);
	d->Send("%sQ%s)uit\r\n", grn, nrm);
	d->Send("Your option : ");

	OLC_MODE(d) = AUCTION_SELL_STARTING_PRICE;
}
void AuctionItemToSellMenu( Descriptor *d )
{
	get_char_cols(d->character);
	d->Send("You have the following items in your inventory:\r\n");
	unsigned int i = 0;
	for(Object *o = d->character->carrying;o;o = o->next_content)
	{
		++i;
		d->Send("%s%2d%s) %s%s%s\r\n", grn, i, nrm, cyn, o->short_description, nrm);
	}

	OLC_MODE(d) = AUCTION_ITEM_TO_SELL;
}
void AuctionDispBrowseMenu( Descriptor *d )
{
	char objNameBuf[256];
	const static int maxNameSize=45;
	int firstItem = d->olc->auction_data->GetPage() * AuctionManager::ItemsPerPage
		- AuctionManager::ItemsPerPage;
	int lastItem = (firstItem + AuctionManager::ItemsPerPage);
	lastItem = MIN(lastItem, d->olc->auction_data->aiCache.size()) - 1;

	get_char_cols(d->character);

	d->Send("\r\n");
	d->Send("PAGE %s%d%s OF %s%d%s\r\n\r\n", grn, d->olc->auction_data->GetPage(), nrm, grn,
		d->olc->auction_data->NumPages(), nrm);

	d->Send("              Item Name                            |  Min Bid    |  Buyout     | Rem Time    | Top Bidder |\r\n");
	d->Send("-----------------------------------------------------------------------------------------------------------\r\n");
	d->Send("                                                   |             |             |             |            |\r\n");
//	d->Send("\r\n");

	std::stringstream OutBuf;
	std::string tStr;
	std::string ObjShortDesc;
	std::string UserName;
	/*** Display the items on this page ***/
	for(int i = firstItem;i <= lastItem;++i)
	{
		AuctionData *ad = d->olc->auction_data;
		tStr = "";

		if( time(0) >= ad->aiCache[i]->GetEndTime() || !ad->aiCache[i]->IsActive() )
			tStr = "Ended";
		else
		{
			Time t(ad->aiCache[i]->GetEndTime() - time(0));
			tStr = MiscUtil::Convert<std::string>(t.dDays()) + "D " + MiscUtil::Convert<std::string>(t.dHours()) + "H "
				+ MiscUtil::Convert<std::string>(t.dMinutes()) + "M";
		}
		strncpy(objNameBuf, ad->aiCache[i]->GetObjShortDesc().c_str(), maxNameSize);
		if( (ad->aiCache[i]->GetObjShortDesc().size()+3) > maxNameSize )
			sprintf(objNameBuf+(maxNameSize-3), "...");
		if( ad->aiCache[i]->GetTopBidderID() == -1 )
			UserName = "<None>";
		else
			UserName = ad->aiCache[i]->GetTopBidderName();
		if( UserName.size() > 10 )
			UserName.resize(10);
		OutBuf << cyn << std::left << std::setw(3) << (i+1) << nrm << ") "
			<< grn << std::setw(maxNameSize) << std::left << objNameBuf
			<< nrm << " | "
			<< std::setw(11) << std::left
			<< d->character->GoldString(ad->aiCache[i]->GetNextMinBid(),false,true) << " | "
			<< std::setw(11) << std::left
			<< d->character->GoldString(ad->aiCache[i]->GetBuyout(),false,true) << " | "
			<< std::setw(11) << std::left << tStr << " | "
			<< std::setw(10) << std::left << UserName << " |"
			<< std::endl;
		d->Send(StringUtil::vaEscape(OutBuf.str()).c_str());
		OutBuf.str("");
	}
	/*** Display pagination ***/
	d->Send("\r\n");
	d->Send("To bid on an item, enter its number from the list above.");
	d->Send("\r\n\r\n");

	d->Send("[%sN%s] Next Page\r\n", grn, nrm);
	d->Send("[%sP%s] Previous page\r\n", grn, nrm);
//	d->Send("[%sB%s] Bid on Item\r\n", grn, nrm);
//	d->Send("[%sU%s] Buyout\r\n", grn, nrm);
	d->Send("[%sQ%s] Quit\r\n", grn, nrm);
	OLC_MODE(d) = AUCTION_BROWSE;
}
void AuctionDispItemBidMenu( Descriptor *d )
{
	get_char_cols(d->character);
	AuctionItem *ai = d->olc->auction_data->GetSelectedItem();
	std::string RemTimeStr;

	time_t tmpT = (time(0) > ai->GetEndTime()) ? (0) : (ai->GetEndTime() - time(0));
	Time t( tmpT );
	RemTimeStr = MiscUtil::Convert<std::string>(t.dDays()) + "D " + MiscUtil::Convert<std::string>(t.dHours()) + "H " +
		MiscUtil::Convert<std::string>(t.dMinutes()) + "M";
	d->Send("Selected Item : %s%s%s\r\n", cyn, ai->GetObjShortDesc().c_str(), nrm);
	d->Send("Time remaining: %s%s%s\r\n", cyn, RemTimeStr.c_str(), nrm);
	d->Send("Minimum Bid   : %s%s%s\r\n", cyn, d->character->GoldString(ai->GetNextMinBid(),false).c_str(), nrm);
	d->Send("Buyout        : %s%s%s\r\n", cyn, d->character->GoldString(ai->GetBuyout(),false).c_str(), nrm);

	d->Send("\r\n");

	d->Send("%s1%s) Place Bid\r\n", cyn, nrm);
	d->Send("%s2%s) Buyout\r\n", cyn, nrm);
	d->Send("%s3%s) Quit\r\n", cyn, nrm);

	if( !ai->IsActive() && ai->CanBeRetrievedBy(d->character) )
	{//Auction has ended and the browser is the winner. Let them retrieve the item.
		d->Send("\r\n");
		d->Send("%s4%s) Retrieve Item\r\n",cyn,nrm);
	}

	d->olc->auction_data->SetBidRequest(0);
	OLC_MODE(d) = AUCTION_PLACE_BID_MENU;
}
void AuctionDispBidConfirmationMenu( Descriptor *d )
{
	d->Send("You are about to bid %lld coppers(%lld coppers more than your last bid).\r\n"
		"Are you sure you want to do this? (Y/N) : ",
		d->olc->auction_data->GetBidRequest(),
		d->olc->auction_data->GetBidRequest() - d->olc->auction_data->GetSelectedItem()->GetLastBid(d->character->player.idnum));
	OLC_MODE(d) = AUCTION_CONFIRM_BID;
}
void AuctionParse( Descriptor *d, const std::string &arg )
{
	AuctionData *ad = d->olc->auction_data;
	Auction *a = d->olc->auction;
	get_char_cols(d->character);
	switch( OLC_MODE(d) )
	{
	case AUCTION_MENU:
		if( toupper(arg[0]) == 'Q' )
			cleanup_olc(d, CLEANUP_ALL);
		else if( atoi(arg.c_str()) == 1 )
		{//View Your Bids
			d->olc->auction_data->ClearCache();
			d->olc->auction_data->aiCache = a->loadMyBids(d,ad);
			AuctionDispBrowseMenu(d);
		}
		else if( atoi(arg.c_str()) == 2 )
		{//View Your Auctions
			d->olc->auction_data->ClearCache();
			d->olc->auction_data->aiCache = a->loadMyAuctions(d,ad);
			AuctionDispBrowseMenu(d);
		}
		else if( atoi(arg.c_str()) == 3 )
		{//Sell An Item
			d->olc->auction_data->ClearSellData();
			AuctionDispSellMenu(d);
		}
		else if( atoi(arg.c_str()) == 4 )
		{//Browse
			d->olc->auction_data->ClearCache();
			d->olc->auction_data->aiCache = d->olc->auction->loadItems( (0) );
			AuctionDispBrowseMenu(d);
		}
		else
			d->Send("Invalid option.\r\nTry again: ");
		break;
	case AUCTION_AUCTION_OVER_DIALOG:
		//...Any input will return us to the browser.
		AuctionDispBrowseMenu(d);
		break;
	case AUCTION_PLACE_NEW_BID_DIALOG:
		if( toupper(arg[0]) == 'Y' )
		{
			AuctionDispItemBidMenu(d);
		}
		else if( toupper(arg[0]) == 'N' )
		{
			AuctionDispBrowseMenu(d);
		}
		else
		{
			d->Send("Please enter 'Y' or 'N' : ");
		}
		break;
	case AUCTION_CONFIRM_BID:
		if( toupper(arg[0]) == 'N' )
		{
			AuctionDispBrowseMenu(d);
			return;
		}
		else if( toupper(arg[0]) == 'Y' )
		{
			/*** IMPORTANT: FIRST, refresh the item to make sure nobody else has bid.
				Remember, this item is cached into memory. If we reload and determine that the bid is now unacceptable,
				we must alert the user and ask them what they wish to do about it.
			***/
			int nrBidsBefore = d->olc->auction_data->GetSelectedItem()->BidQuery->numRows();
			d->olc->auction_data->GetSelectedItem()->Refresh();
			int nrBidsNow = d->olc->auction_data->GetSelectedItem()->BidQuery->numRows();

			//Make sure the auction is still alive.
			if( !d->olc->auction_data->GetSelectedItem()->IsActive() ||
				d->olc->auction_data->GetSelectedItem()->GetEndTime() <= time(0))
			{
				d->Send("The auction has already ended. Press return to go to the auction browser: ");
				OLC_MODE(d) = AUCTION_AUCTION_OVER_DIALOG;
				return;
			}

			//Make sure the bid is still acceptable(meets minimum bid requirement)
			if( nrBidsNow > nrBidsBefore )
			{//Someone has placed a bid on this item while we were browsing.
				d->Send("Someone has placed a bid since you selected this bid. The minimum bid is now %d.\r\n"
					"Would you like to place a new bid? (Y/N) : ", d->olc->auction_data->GetSelectedItem()->GetNextMinBid());
				OLC_MODE(d) = AUCTION_PLACE_NEW_BID_DIALOG;
				return;
			}
			//If the above tests pass, we will place the bid.
			bool res = d->olc->auction->placeBid(d->character, d->olc->auction_data);

			if( res == true )
				d->Send("%s%sYour bid has been placed.%s\r\n", bld, mag, nrm);
			else
				d->Send("An error occured when we tried to place your bid. Please report this!\r\n");
			AuctionDispMenu(d);
		}
		else
			d->Send("Invalid option.\r\nTry again: ");
		break;
	case AUCTION_PLACE_BID:
		if( toupper(arg[0]) == 'Q' )
		{//Quit
			AuctionDispItemBidMenu(d);
			return;
		}
		else if( MiscUtil::isNumber(arg) )
		{
			long long bid = atoi(arg.c_str());
			if( d->olc->auction_data->GetSelectedItem()->GetBuyout() > 0 ) {
				bid = MIN(bid, d->olc->auction_data->GetSelectedItem()->GetBuyout());
			}
			if( bid < d->olc->auction_data->GetSelectedItem()->GetNextMinBid() )
			{//Bid is too small
				d->Send("You entered a value that is less than the minimum bid.\r\n");
				return;
			}
			if( d->character->points.gold < bid )
			{//Bidder does not have sufficient gold
				d->Send("You do not have enough gold to match that bid.\r\n");
				return;
			}
			d->olc->auction_data->SetBidRequest( bid );
			AuctionDispBidConfirmationMenu(d);
			return;
		}
		d->Send("Your input must be numeric.\r\nTry again: ");
		break;
	case AUCTION_PLACE_BID_MENU:
		if( atoi(arg.c_str()) == 1 )
		{//Manual Bid
			if( d->olc->auction_data->GetSelectedItem()->GetOwnerID() == d->character->player.idnum )
			{
				d->Send("You may not bid on your own item.\r\n");
				return;
			}
			d->Send("Enter your bid in coppers(100 C = 1 S, 100S = 1G)\r\nYou may enter 'Q' to exit: ");
			OLC_MODE(d) = AUCTION_PLACE_BID;
			return;
		}
		else if( atoi(arg.c_str()) == 2 )
		{//Buyout
			if( d->olc->auction_data->GetSelectedItem()->GetOwnerID() == d->character->player.idnum )
			{
				d->Send("You may not bid on your own item.\r\n");
				return;
			}
			if( d->olc->auction_data->GetSelectedItem()->GetBuyout() <= 0 )
			{
				d->Send("This auction does not have a buyout price.\r\n");
				return;
			}
			if( d->character->points.gold < d->olc->auction_data->GetSelectedItem()->GetBuyout() )
			{
				d->Send("You don't have enough gold to match the buyout price.\r\n");
				return;
			}
			d->olc->auction_data->SetBidRequest( d->olc->auction_data->GetSelectedItem()->GetBuyout() );
			AuctionDispBidConfirmationMenu(d);
			return;
		}
		else if( atoi(arg.c_str()) == 3 )
		{//Quit
			AuctionDispBrowseMenu(d);
			return;
		}
		else if( atoi(arg.c_str()) == 4 && ad->GetSelectedItem()->CanBeRetrievedBy(d->character) )
		{//User has won. Let them pick up the item.
			d->Send("%s%sYou have retrieved %s.%s\r\n", bld,mag,ad->GetSelectedItem()->GetObjShortDesc().c_str(),nrm);
			a->retrieveItem(d,ad);
			AuctionDispBrowseMenu(d);
			return;
		}
		d->Send("Invalid option.\r\nTry again: ");
		break;
	case AUCTION_BROWSE:
		if( toupper(arg[0]) == 'N' )
		{
			if( (d->olc->auction_data->GetPage() + 1) > d->olc->auction_data->NumPages() )
				d->Send("There is no next page.\r\n");
			else
			{
				d->olc->auction_data->SetPage( d->olc->auction_data->GetPage()+1 );
				AuctionDispBrowseMenu(d);
			}
			return;
		}
		else if( toupper(arg[0]) == 'P' )
		{
			if( (d->olc->auction_data->GetPage() ) == 1 )
				d->Send("There is no previous page.\r\n");
			else
			{
				d->olc->auction_data->SetPage( d->olc->auction_data->GetPage()-1 );
				AuctionDispBrowseMenu(d);
			}
			return;
		}
		else if( toupper(arg[0]) == 'Q' )
		{
			AuctionDispMenu(d);
			return;
		}
		else if( MiscUtil::isNumber( arg ) )
		{
			int nr = atoi(arg.c_str())-1;
			if( nr >= 0 && nr < d->olc->auction_data->aiCache.size() )
			{
				d->olc->auction_data->SetSelectedItem( nr );
				AuctionDispItemBidMenu(d);
				return;
			}
		}
		d->Send("Invalid option.\r\nTry again: ");
		break;
	case AUCTION_SELL_MENU:
		if( toupper(arg[0]) == 'B' )
			AuctionDispBuyoutMenu(d);
		else if( toupper(arg[0]) == 'I' )
			AuctionItemToSellMenu(d);
		else if( toupper(arg[0]) == 'D' )
			AuctionDispDurationMenu(d);
		else if( toupper(arg[0]) == 'S' )
			AuctionDispStartingPriceMenu(d);
		else if( toupper(arg[0]) == 'Q' )
			AuctionDispMenu(d);
		else if( toupper(arg[0]) == 'P' )
		{
			if( ad->GetBuyoutPrice() > 0 && ad->GetBuyoutPrice() < ad->GetStartingPrice() )
			{//The buyout price is invalid...
				d->Send("You have set a buyout that is lower than the minimum bid.\r\n"
					"Your buyout must either be set to zero, or it must be greater than the starting price.\r\n");
				d->Send("Your option : ");
				return;
			}
			if( ad->GetItemToSell() == 0 )
			{//No item has been selected...
				d->Send("You have not selected an item to sell. You must do this in order to submit an auction\r\n");
				d->Send("Your option : ");
				return;
			}
			if( ad->GetSellDuration() == 0 )
			{//Invalid sell duration...
				d->Send("You haven't selected a duration for your auction's sale.\r\n");
				d->Send("Your option : ");
				return;
			}
			if( ad->GetItemToSell()->carried_by != d->character )
			{//Seller is no longer holding the item they were trying to sell. Stolen?
				d->Send("You are no longer carrying the item you were trying to sell!\r\n");
				d->Send("Your option : ");
				return;
			}
			if( !d->olc->auction->placeAuction( d->character, d->olc->auction_data ) )
				d->Send("Oops! It appears that you no longer have that item!\r\n");
			else
				d->Send("Auction submitted.\r\n");
			d->olc->auction_data->ClearSellData();
			AuctionDispMenu(d);
		}
		else
			d->Send("Invalid opton.\r\nTry again: ");
		break;
	case AUCTION_ITEM_TO_SELL:
	{
		if( toupper( arg[0] ) == 'Q' )
		{
			AuctionDispSellMenu(d);
			return;
		}
		if( !MiscUtil::isNumber( arg.c_str() ) || atoi(arg.c_str()) <= 0 )
		{
			d->Send("Your input must be numeric and greater than zero, or 'Q' to quit.\r\nTry again: ");
			return;
		}
		int nr = atoi( arg.c_str() );
		Object *o;
		for( o = d->character->carrying;o && --nr > 0;o = o->next_content );
		if( o == NULL )
			d->Send("That item does not exist.\r\n");
		else if( o->contains != NULL )
			d->Send("You must select an item which is not carrying other items.\r\n");
		else
			d->olc->auction_data->SetItemToSell(o);
		AuctionDispSellMenu(d);
		break;
	}
	case AUCTION_DURATION_MENU:
		if( atoi(arg.c_str())-1 < 0 || atoi(arg.c_str())-1 >= Auction::DurationList.size() )
		{
			d->Send("You must choose a duration from the list.\r\nTry again: ");
			return;
		}
		d->olc->auction_data->SetSellDuration( Auction::DurationList[atoi(arg.c_str())-1] );
		AuctionDispSellMenu(d);
		break;
	case AUCTION_SELL_BUYOUT:
		if( toupper(arg[0]) == 'G' )
		{
			d->Send("Enter a value for gold: ");
			OLC_MODE(d) = AUCTION_BUYOUT_GOLD;
		}
		else if( toupper(arg[0]) == 'S' )
		{
			d->Send("Enter a value for silver: ");
			OLC_MODE(d) = AUCTION_BUYOUT_SILVER;
		}
		else if( toupper(arg[0]) == 'C' )
		{
			d->Send("Enter a value for coppers: ");
			OLC_MODE(d) = AUCTION_BUYOUT_COPPER;
		}
		else if( toupper(arg[0]) == 'Q' )
			AuctionDispSellMenu(d);
		else
			d->Send("Invalid option.\r\nTry again: ");
		break;
	case AUCTION_BUYOUT_GOLD:
		d->olc->auction_data->SetBuyoutPrice(atoi(arg.c_str()), 'G');
		AuctionDispBuyoutMenu(d);
		break;
	case AUCTION_BUYOUT_SILVER:
		d->olc->auction_data->SetBuyoutPrice(MIN(99,atoi(arg.c_str())), 'S');
		AuctionDispBuyoutMenu(d);
		break;
	case AUCTION_BUYOUT_COPPER:
		d->olc->auction_data->SetBuyoutPrice(MIN(99,atoi(arg.c_str())), 'C');
		AuctionDispBuyoutMenu(d);
		break;
	case AUCTION_SELL_STARTING_PRICE:
		if( toupper(arg[0]) == 'G' )
		{
			d->Send("Enter a value for gold: ");
			OLC_MODE(d) = AUCTION_STARTING_GOLD;
		}
		else if( toupper(arg[0]) == 'S' )
		{
			d->Send("Enter a value for silver: ");
			OLC_MODE(d) = AUCTION_STARTING_SILVER;
		}
		else if( toupper(arg[0]) == 'C' )
		{
			d->Send("Enter a value for coppers: ");
			OLC_MODE(d) = AUCTION_STARTING_COPPER;
		}
		else if( toupper(arg[0]) == 'Q' )
			AuctionDispSellMenu(d);
		else
			d->Send("Invalid option.\r\nTry again: ");
		break;
	case AUCTION_STARTING_GOLD:
		d->olc->auction_data->SetStartingPrice(atoi(arg.c_str()), 'G');
		AuctionDispStartingPriceMenu(d);
		break;
	case AUCTION_STARTING_SILVER:
		d->olc->auction_data->SetStartingPrice(MIN(99,atoi(arg.c_str())), 'S');
		AuctionDispStartingPriceMenu(d);
		break;
	case AUCTION_STARTING_COPPER:
		d->olc->auction_data->SetStartingPrice(MIN(99,atoi(arg.c_str())), 'C');
		AuctionDispStartingPriceMenu(d);
		break;
	case AUCTION_BUY_MENU:
		if     ( arg == "1" )
			AuctionDispBuyTypeMenu(d);
		else if( arg == "2" )
		{
			d->Send("Enter a word to act as a filter for your search results: ");
			OLC_MODE(d) = AUCTION_BUY_SEARCH_TERM;
		}
		else if( arg == "3" ){}
		else if( arg == "4" )
			AuctionDispMenu(d);
		else
		{
			d->Send("Invalid option.\r\nTry again: ");
			return;
		}
		break;
	case AUCTION_BUY_SEARCH_TERM:
		if( arg.size() >= 32 )
		{
			d->Send("Search term too long.\r\nPlease submit a shorter term: ");
			return;
		}
		d->olc->auction_data->SetSearchTerm( arg );
		AuctionDispBuyMenu(d);
		break;
	case AUCTION_BUY_TYPE:
	{
		if( toupper(arg[0]) == 'Q' )
		{
			AuctionDispBuyMenu(d);
			return;
		}
		int narg = atoi(arg.c_str())-1;
		if( narg < 0 || narg > NUM_ATYPES )
		{
			d->Send("Invalid option.\r\nTry again: ");
			return;
		}
		d->olc->auction_data->item_types.flip(narg);
		AuctionDispBuyTypeMenu(d);
		break;
	}
	default:
		MudLog(BRF, MAX( GET_INVIS_LEV(d->character), LVL_APPR ), TRUE, "%s has entered an invalid state in the auction interface. Mode: %d.",
			GET_NAME( d->character ), OLC_MODE(d));
		break;
	}
}
