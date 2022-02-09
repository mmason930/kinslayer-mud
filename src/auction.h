/********************************************************************
 *	auction.h - Defines the AuctionManager classes					*
 *																	*
 *	Coded by Galnor in September of 2009 (mikemason930@gmail.com)	*
 *																	*
 ********************************************************************/

class AuctionManager;
class Auction;
class auctionItem;

/* A singleton class, responsible for managing all Auctions in the game */
class AuctionManager
{
private:
	AuctionManager();
	~AuctionManager();

	static AuctionManager *Self;

	std::map<int, Auction*> MyAuctions;
public:
	static int ItemsPerPage;
	static AuctionManager &GetManager();
	void Free();
	void Boot();
	void save();
	
	Auction *BootOneAuction( sql::Row MyRow );
	Auction *GetAuction( const int _vnum );
	std::list< Auction * > &GetListOfAuctions();
	void AddAuction( Auction *a );
	std::map<int,Auction*>::iterator RemoveAuction( Auction *a );
	std::map<int,Auction*>::iterator RemoveAuction( const int _vnum );

	unsigned int NumAuctions();

	void SetupAuctionInterface( Character *ch, Auction *a );
	void UpdateAuctions();
	void ReimburseLosers( const int ai_id, const int winnerID );
	void RewardOwner( const int ai_id );
};

class Auction
{
private:
	int vnum;							//Auction's virtual MiscUtil::random(id)
	std::string Name;					//Name of Auction
	std::bitset< 16 > aRaces;			//Allowed races
	std::bitset< 64 > dClans;			//Disallowed clans

public:
	static std::vector<int> DurationList;
	Auction();
	Auction( const int _vnum, const std::string &_Name,
		const unsigned long long _aRaces=0, const unsigned long long _dClans=0 );
	~Auction();

	std::string getName();
	int getVnum();

	void setVnum( const int _vnum );
	void setName( const std::string _Name );

	bool clanIsAllowed( const int cl_vnum );
	bool raceIsAllowed( const int r_num );

	void allowRace( const int r_num );
	void allowClan( const int cl_vnum );
	void disallowRace( const int r_num );
	void disallowClan( const int cl_num );
	bool placeAuction( Character *ch, class AuctionData *ad );
	bool placeBid( Character *bidder, class AuctionData *ad );

	void retrieveItem(Descriptor *d, class AuctionData *ad );

	std::list< Object* > loadItemList( bool recursive );

	std::string aRacesStr( Character *ch=NULL );
	std::string dClansStr( Character *ch=NULL );

	std::bitset<16> bs_aRaces();
	std::bitset<64> bs_dClans();

	std::vector< AuctionItem* > loadItems( class AuctionData* Filter );
	std::vector< AuctionItem* > loadMyBids(Descriptor *d, class AuctionData *ad);
	std::vector< AuctionItem* > loadMyAuctions(Descriptor *d, class AuctionData *ad);

	void save();
};

