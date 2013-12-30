#ifndef POKER_H
#define POKER_H

class Card;
class PokerTable;
class PokerPlayer;

enum PokerState
{
	SEATING, CONFIRMING, DEAL, PREFLOP, FLOP, TURN, RIVER, END
};

class Poker
{
public:
	static const int NumCardValues = 13;	//Number of card values. Ace, 2, 3, etc.
	static const int NumCardSuits  = 4;		//Number of card suits. Clubs, Spades, etc.
	static std::vector< Card* > Deck;	//A standard, 52 card deck. Does not change.
	static void Setup();
	static void Cleanup();
	static std::string CardLetter( const int _val );
};

struct HandValue
{
	int Hand;
	int val1, val2;
	std::list<int> High;

	bool BetterThan( HandValue* OtherHand );
	bool WorseThan( HandValue* OtherHand );
	bool SameAs( HandValue* OtherHand );

	std::string HandName();
};

class PokerTable
{
private:
	std::list< PokerPlayer* >	Players;		//Those still in the overall game.
	std::list< Character* >		Watchers;
	std::queue< PokerPlayer* >	TurnCycle;		//Order of play.
	std::vector< Card* >		Deck;
	std::vector< Card* >		CommunityCards;
	std::list< std::string >	BanList;

	PokerPlayer* Dealer, *LastToShow, *LastToCall;
	int Pot, MinimumRaise, stakes, current_bet, num_init_players;

	PokerState State;
	class Room* InRoom;
public:
	bool DealerIsGone;
	PokerTable( class Room* _InRoom );
	~PokerTable();

	bool ProcessGameInput( PokerPlayer* Player, std::string Command, std::string Argument );

	int NumberOfActivePlayers();
	int NumberOfUnfoldedPlayers();
	int NumOfPlayers();
	bool HasStarted();
	bool AllConfirmed();
	bool CollectWagers();
	bool GameIsOver();
	PokerPlayer* NextUp();
	PokerPlayer* TableHost();
	void AddPlayer( class Character*   _Player );
	void AddPlayer( PokerPlayer* _Player );
	void MinRaise( const int _MinRaise );

	void Flop();
	void Turn();
	void River();

	void AddWatcher( Character *ch );
	void RemoveWatcher( Character *ch );
	bool IsWatching( Character *ch );

	const int MinRaise( void );
	void NextGameStep();
	void SetupDeck();
	void DealCards();
	void SetupTurns( PokerPlayer* From, bool exclude );
	void PrintBoard( Character *ch = NULL, const int wait_time = 8 );
	void EmptyTurnCycle() { while( !TurnCycle.empty() ) TurnCycle.pop(); }
	void RemoveCardFromDeck( Card* _Card );
	void PrintCommunityCards();
	void FindWinners( std::list< std::list< PokerPlayer* > > &Winners );
	void SendToPlayers( const char* message, ... );
	void SendToWatchers( const char* message, ... );
	void SendToTable( const char* message, ... );
	void RemovePlayer( class Character* ch );
	void RemovePlayer( PokerPlayer* Player );
	void SetState( PokerState _State ) { State = _State; }
	void ClearCards() { CommunityCards.clear(); }
	void AccumulateBets();
	void RewardWinners( std::list<PokerPlayer* > &Winners );
	void EliminateLostPlayers();
	void EndGame();
	void SetPotSizes();
	void PerformBlinds();
	void CloseTable();

	int Stakes() { return stakes; }
	void Stakes( const int _stakes ) { stakes = _stakes; }

	int CurrentBet() { return current_bet; }
	void CurrentBet( const int _NewBet ) { current_bet = _NewBet; }


	std::string NameFormat( std::string _Name )
	{
		_Name[0] = toupper( _Name[0] );
		for(int i = 1;i < (int)_Name.size();++i) _Name[i] = tolower( _Name[i] );
		return _Name;
	}
	void AddBan( class Character* _Who ) { BanList.push_back( _Who->player.name ); }
	void AddBan( std::string _Name ) { BanList.push_back( NameFormat( _Name ) ); }
	void RemoveBan( class Character* _Who ) {BanList.remove( _Who->player.name ); }
	void RemoveBan( const std::string &_Name ) { BanList.remove( NameFormat( _Name ) ); }

	bool IsBanned( class Character* _Who ) { return (std::find(BanList.begin(), BanList.end(), _Who->player.name) != BanList.end()); }
	bool IsBanned( const std::string &_Name ) { return (std::find(BanList.begin(), BanList.end(), NameFormat( _Name )) != BanList.end()); }

	void PrintBanList();
	void PrintPlayers(Character *ch);

	PokerState GetState() { return State; }

	PokerPlayer* GetDealer() { return Dealer; }
	PokerPlayer* GetPlayerAtSeat( const int seat );
	PokerPlayer* SetNextDealer();
	PokerPlayer* GetLastToShow() { return LastToShow; }
	PokerPlayer* GetLastToCall() { return LastToCall; }
	void SetLastToShow( PokerPlayer* P) { LastToShow = P; }
	void SetLastToCall( PokerPlayer* P) { LastToCall = P; }

	std::vector<Card*> &GetCommunityCards() { return CommunityCards; }
	std::list< PokerPlayer* > &GetPlayers() { return Players; }

	Card* GetCardFromDeck( const std::string &Name );
	Card* GetCardFromDeck();
};

class PokerPlayer
{
private:
	std::vector< Card* > Hand;
	class Character* Player;
	int chips;
	bool folded;
	int seat, bet, pot_size;
	bool confirmed;
	HandValue* HV;

public:
	PokerTable* Table;

	PokerPlayer( class Character* _Player, PokerTable* _Table );
	PokerPlayer();

	Character* GetPlayer() { return Player; }

	Card* GetCard( int nr );
	void ClearHand();
	void DealCard( Card* _Card );

	void Chips( const int _chips );
	const int Chips( void );
	std::string PlayerName( bool board = false );
	std::string ChipStr( bool board = false );
	std::string BetStr( bool board = false );

	HandValue* GetHandValue();

	void PrintHand( void );
	void send(const char* message, ...);
	void Fold();
	void UnFold() { folded = false; }
	void ResetHandValue() { HV = 0; }
	void ShowHandToTable();

	int Seat() { return seat; }
	void Seat( const int _seat ) { seat = _seat; }

	bool Confirmed() { return confirmed; }

	void RemoveConfirmation() { confirmed = false; }
	void Confirm() { confirmed = true; Table->SendToTable("%s confirms.\r\n", PlayerName().c_str()); }
	void Deny()
	{
		Table->SendToTable("%s denies.\r\n", PlayerName().c_str());
		Table->RemovePlayer( Player );
	}

	int Bet() { return bet; }
	void Bet( const int _bet ) { bet = _bet; }

	void RaiseBetTo( const int _amt );
	void Raise( const int amt );
	void Call();

	int PotSize() { return pot_size; }
	void PotSize( const int _amt ) { pot_size = _amt; }

	void DivyUp() { Player->points.gold -= Table->Stakes(); }
	void RewardWithGold( const int _gold ) { Player->points.gold += _gold; }
	int PlayerGold() { return Player->points.gold; }


	bool Folded();
};

class Card
{
private:
	int	value;
	int	suit;

public:
	Card();
	Card( int _value, int _suit );

	template< typename _Container >
	void MoveTo( _Container &Destination, PokerTable* Table );
	std::string ToString( void );

	int GetSuit()	{	return suit;	}
	int GetValue()	{	return value;	}
};

const int PAIR				= 1;
const int TWO_PAIR			= 2;
const int THREE_OF_A_KIND	= 3;
const int STRAIGHT			= 4;
const int FLUSH				= 5;
const int FULL_HOUSE		= 6;
const int FOUR_OF_A_KIND	= 7;
const int STRAIGHT_FLUSH	= 8;

enum HandComparison
{
	BETTER, WORSE, SAME
};

template <typename _Container>
void InsertionSort( _Container &C, int val, int size_limit )
{
	for( typename _Container::iterator i = C.begin();i != C.end();++i)
	{
		if( val >(*i) )
		{
			for( typename _Container::iterator SwapIter = i;SwapIter != C.end();++SwapIter)
			{
				int temp = (*SwapIter);
				(*SwapIter) = val;
				val = temp;
			}
			break;
		}
	}
	if( (int)C.size() < size_limit )
		C.push_back( val );
}

class SevenCardHand
{
public:
	static const int CardsInAHand = 5;
	//Hand is a vector of cards, type Card
	//CardsUsed is a list of card indexes.
	static HandComparison BetterHighCards( std::list< int > HighA, std::list< int > HighB )
	{
		for(std::list< int >::iterator IterA = HighA.begin(), IterB = HighB.begin();IterA != HighA.end() && IterB != HighB.end();++IterA, ++IterB)
		{
			if		( (*IterA) > (*IterB) )
				return BETTER;
			else if	( (*IterB) > (*IterA) )
				return WORSE;
		}
		//All cards thus far have matched. Whichever list is shorter loses.
		return ( HighA.size() > HighB.size() ? BETTER :
			(HighB.size() > HighA.size() ? WORSE : SAME) );
	}
	static std::list<int> FindHighCards(std::vector<Card*> &Hand,std::list<int> &CardsUsed)
	{
		static std::list< int > High;
		High.clear();

		for(int p = 0;p < (int)Hand.size();++p)
		{
			//card is used to make a hand; can't use as high card.
			if( std::find(CardsUsed.begin(), CardsUsed.end(), p) == CardsUsed.end())
			{
				InsertionSort( High, Hand[p]->GetValue(), CardsInAHand - CardsUsed.size() );
			}

		}
		return High;
	}
	static bool HasPair( std::vector< Card* > &Hand, int &val, std::list<int> &High )
	{
		std::list< int > CardsUsed;
		val = -1;

		High.clear();
		for(int i = 0;i < (int)Hand.size();++i)
		{
			int cur = Hand.at( i )->GetValue();

			CardsUsed.clear();
			CardsUsed.push_back( i );

			for(int n = i + 1;n < (int)Hand.size();++n)
			{
				if(Hand.at( n )->GetValue() == cur && cur > val)
				{
					CardsUsed.push_back( n );
					High = SevenCardHand::FindHighCards( Hand, CardsUsed );

					val = cur;
					break;
				}
			}
		}
		return (val != -1);
	}
	//val1 and val2 refer to the values of the high and low pair
	static bool HasTwoPair( std::vector< Card* > &Hand, int &val1, int &val2,std::list<int> &High )
	{
		val1 = val2 = -1;
		High.clear();

		std::list< int > CardsUsed;

		for(int i = 0;i < (int)Hand.size();++i)
		{
			int cur = Hand.at( i )->GetValue();

			for(int n = i + 1;n < (int)Hand.size();++n)
			{
				//if cur == val1, then we're not looking at a second pair,
				//rather a 3 or 4 of a kind. That's not what we want...
				if(Hand.at( n )->GetValue() == cur && cur != val1)
				{
					if(cur > val1)
					{
						int temp = val1;
						val1 = cur;
						val2 = temp;
					}
					else if(cur > val2)
					{
						val2 = cur;
					}
					break; //Skip the rest, start checking the next #
				}
			}
		}
		int highest = -1;
		for(int i = 0;i < (int)Hand.size();++i)
		{
			if(	Hand[ i ]->GetValue() != val1 && Hand[ i ]->GetValue() != val2 &&
				Hand[ i ]->GetValue() > highest)
			{
				highest = Hand[ i ]->GetValue();
			}
		}
		High.push_back( highest );
		return (val1 != -1 && val2 != -1);
	}
	static bool HasThreeOfAKind( std::vector< Card* > &Hand, int &val,std::list<int> &High )
	{
		val = -1;
		std::list< int > CardsUsed;

		High.clear();
		for(int i = 0;i < (int)Hand.size();++i)
		{
			int nr, cur = Hand.at( i )->GetValue();
			nr = 1;
			CardsUsed.clear();
			CardsUsed.push_back( i );
			for(int n = i + 1;n < (int)Hand.size();++n)
			{
				if(Hand.at( n )->GetValue() == cur && cur > val)
				{
					CardsUsed.push_back( n );
					if(++nr == 3)
					{
						val = cur;
						break; //Skip the rest, check next #...
					}
				}
			}
		}
		High = SevenCardHand::FindHighCards( Hand, CardsUsed );
		return (val != -1);
	}
	static bool HasStraight( std::vector< Card* > &Hand, int &val )
	{
		std::vector< Card* > OrderedHand = Hand;
		//Element we're going to swap for.
		for(int i = 0;i < (int)OrderedHand.size();++i)
		{
			int largest = i;
			//Find the largest element.
			for(int k = i+1;k < (int)OrderedHand.size();++k)
			{
				if(OrderedHand.at(k)->GetValue() > OrderedHand.at(largest)->GetValue())
					largest = k;
			}
			if( OrderedHand[ largest ]->GetValue() > OrderedHand[ i ]->GetValue() )
			{
				Card* temp = OrderedHand.at( largest );
				OrderedHand[ largest ] = OrderedHand[ i ];
				OrderedHand[ i ] = temp;
			}
		}

		//A 2 3 4 5 8 J

		//This would come to be...
		// 2 3 4 5, if you have this specific run, you gotta specially check for Ace.

		//So first check for a run...

		int run = 1;
		int bot_of_run=0, top_of_run=0;
		for( int i = 0; i < (int)OrderedHand.size();++i )
		{
			if( (i) && OrderedHand[ i ]->GetValue() == OrderedHand[ i - 1 ]->GetValue() - 1)
			{
				--bot_of_run;
				if(++run == 5)
					break;
			}
			else if( (!i) || OrderedHand[ i ]->GetValue() != OrderedHand[ i - 1 ]->GetValue())
			{
				bot_of_run = top_of_run = OrderedHand[i]->GetValue();
				run = 1;
			}
		}

		if(run == 4 && bot_of_run == 0 && OrderedHand[ 0 ]->GetValue() == 12)
		//If only a 4 card run and the bottom card is a two and there is an ace in
		//the hand, then this is still a straight!
		{
			++run;
		}
		if(run >= 5)
		{
			val = top_of_run;
			return true;
		}
		return false;
	}
	static bool HasFlush( std::vector< Card* > &Hand, int &val )
	{
		for(int suit = 0;suit < Poker::NumCardSuits;++suit)
		{
			int num, high, i;
			for(i = 0, high = 0, num = 0;i < (int)Hand.size();++i)
			{
				if( Hand[ i ]->GetSuit() == suit )
				{
					if( Hand[ i ]->GetValue() > high )
						high = Hand[ i ]->GetValue();
					++num;
				}
			}
			if(num >= 5)
			{
				val = high;
				return true;
			}
		}
		return false;
	}
	static bool HasFullHouse( std::vector< Card* > &Hand, int &val1, int &val2 )
	{
		val1 = val2 = -1;
		//First find the set.
		for(int i = 0;i < (int)Hand.size();++i)
		{
			int nr, cur = Hand.at( i )->GetValue();
			nr = 1;
			for(int n = i + 1;n < (int)Hand.size();++n)
			{
				if(Hand.at( n )->GetValue() == cur && cur > val1 && ++nr == 3)
				{
					val1 = cur;
					break; //Skip the rest, check next #...
				}
			}
		}
		if (val1 == -1)
			return false;//No three of a kind!

		//Now check for a pair NOT equal to the set.
		for(int i = 0;i < (int)Hand.size();++i)
		{
			int cur = Hand.at( i )->GetValue();

			//This is one of the cards from the set. Skip it.
			if(cur == val1)
				continue;
			for(int n = i + 1;n < (int)Hand.size();++n)
			{
				if(Hand.at( n )->GetValue() == cur && cur > val2)
				{
					val2 = cur;
					break; //Skip the rest, check next #...
				}
			}
		}
		return (val2 != -1);
	}
	static bool HasFourOfAKind( std::vector< Card* > &Hand, int &val,std::list<int> &High )
	{
		val = -1;
		int h = -1;
		std::list< int > CardsUsed;

		High.clear();

		for(int i = 0;i < (int)Hand.size();++i)
		{
			int nr, cur = Hand.at( i )->GetValue();
			nr = 1;

			CardsUsed.clear();
			for(int n = i + 1;n < (int)Hand.size();++n)
			{
				if(Hand.at( n )->GetValue() == cur && cur > val)
				{
					CardsUsed.push_back( n );
					if(++nr == 4)
					{
						val = cur;
						for(int p = 0;p < (int)Hand.size();++p)
						{
							if(Hand[ p ]->GetValue() != cur && Hand[ p ]->GetValue() > h)
								h = Hand[ p ]->GetValue();
						}
						CardsUsed.push_back( h );
						High = SevenCardHand::FindHighCards( Hand, CardsUsed );
						return true;//Can't be more than one 4 of a kind.
					}
				}
			}
		}
		return false;
	}
	static bool HasStraightFlush( std::vector< Card* > &Hand, int &val )
	{
		std::vector< Card* > OrderedHand;

		int suit, num;
		for(suit = 0;suit < Poker::NumCardSuits;++suit)
		{
			int i;
			for(i = 0, num = 0;i < (int)Hand.size();++i)
			{
				if( Hand[ i ]->GetSuit() == suit )
				{
					OrderedHand.push_back( Hand[ i ] );
					++num;
				}
			}
			if(num >= 5)
			{
				break;
			}
			OrderedHand.clear();
		}
		if(num < 5) return false;

		//Element we're going to swap for.
		for(int i = 0;i < (int)OrderedHand.size();++i)
		{
			int largest = i;
			//Find the largest element.
			for(int k = i+1;k < (int)OrderedHand.size();++k)
			{
				if(OrderedHand.at(k)->GetValue() > OrderedHand.at(largest)->GetValue())
					largest = k;
			}
			if( OrderedHand[ largest ]->GetValue() > OrderedHand[ i ]->GetValue() )
			{
				Card* temp = OrderedHand.at( largest );
				OrderedHand[ largest ] = OrderedHand[ i ];
				OrderedHand[ i ] = temp;
			}
		}

		//A 2 3 4 5 8 J

		//This would come to be...
		// 2 3 4 5, if you have this specific run, you gotta specially check for Ace.

		//So first check for a run...

		int run = 1;
		int bot_of_run=0, top_of_run=0;
		for( int i = 0; i < (int)OrderedHand.size();++i )
		{
			//10H, 10S, 9S, 8S, 7S, 6S
			if( (i) && OrderedHand[ i ]->GetValue() == OrderedHand[ i - 1 ]->GetValue() - 1)
			{
				--bot_of_run;
				if(++run == 5)
					break;
			}
			else if( (!i) || OrderedHand[ i ]->GetValue() != OrderedHand[ i - 1 ]->GetValue())
			{
				bot_of_run = top_of_run = OrderedHand[i]->GetValue();
				run = 1;
			}
		}

		if(run == 4 && bot_of_run == 0 && OrderedHand[ 0 ]->GetValue() == 12)
		//If only a 4 card run and the bottom card is a two and there is an ace in
		//the hand, then this is still a straight!
		{
			++run;
		}
		if(run >= 5)
		{
			val = top_of_run;
			return true;
		}
		return false;
	}
};

#endif


