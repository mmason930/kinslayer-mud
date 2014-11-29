#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "poker.h"
#include "db.h"
#include "utils.h"
#include "interpreter.h"
#include "handler.h"
#include "olc.h"
#include "screen.h"
#include "dg_event.h"

#include "MiscUtil.h"
#include "StringUtil.h"
#include "rooms/Room.h"

PokerTable *PokerTableList = NULL;

//#define POKER_DEBUG

ACMD(do_tell);

std::vector< Card* > Poker::Deck;

void PokerAutoComplete( void* Data );
void PokerDeal( void* Data );
void PokerShowCards( void* Data );
void PokerShuffle( void* Data );
void PokerPrint( void* Data );

	/**** Standard Poker Data ****/

void Poker::Setup()
{
	for(int suit = 0;suit < Poker::NumCardSuits;++suit)
	{
		for(int value = 0;value < Poker::NumCardValues;++value)
		{
			Poker::Deck.push_back( new Card( value, suit ) );
		}
	}
}
void Poker::Cleanup()
{
	while( !Poker::Deck.empty() )
	{
		delete (Poker::Deck.back());
		Poker::Deck.pop_back();
	}
}
std::string Poker::CardLetter( const int _val )
{
	std::string Name;
	switch( _val )
	{
	case 0: return "2";
	case 1: return "3";
	case 2: return "4";
	case 3: return "5";
	case 4: return "6";
	case 5: return "7";
	case 6: return "8";
	case 7: return "9";
	case 8: return "10";
	case 9: return "J";
	case 10:return "Q";
	case 11:return "K";
	case 12:return "A";
	default:return "Invalid";
	}
}
	/**** Poker Table Data ****/
PokerTable::PokerTable( Room* _InRoom )
{
	Dealer = 0;
	Pot = 0;
	stakes = 0;
	LastToShow = 0;
	current_bet = 0;
	num_init_players = 0;
	InRoom = _InRoom;
	DealerIsGone = false;
	InRoom->PTable = this;

	State = SEATING;
}

PokerTable::~PokerTable()
{
	while( Players.size() )
	{
		RemovePlayer( Players.front() );
	}
	if( InRoom ) InRoom->PTable = 0;
}

void PokerTable::SetupDeck()
{
	Deck.resize( Poker::Deck.size() );
	std::copy(Poker::Deck.begin(), Poker::Deck.end(), this->Deck.begin());
}
void PokerTable::SendToPlayers(const char* message, ...)
{
	char buffer[MAX_STRING_LENGTH];
	va_list args;
	va_start( args, message );
	vsnprintf( buffer, sizeof(buffer), message, args );
	va_end( args );

	for( std::list< PokerPlayer* >::iterator P = Players.begin();P != Players.end();++P)
	{
		(*P)->send(buffer);
	}
}
void PokerTable::SendToWatchers( const char* message, ... )
{
	char buffer[MAX_STRING_LENGTH];
	va_list args;
	va_start( args, message );
	vsnprintf( buffer, sizeof(buffer), message, args );
	va_end( args );

	for( std::list< Character* >::iterator W = Watchers.begin();W != Watchers.end();++W)
	{
		(*W)->send(buffer);
	}
}
void PokerTable::SendToTable( const char* message, ... )
{
	char buffer[MAX_STRING_LENGTH];
	va_list args;
	va_start( args, message );
	vsnprintf( buffer, sizeof(buffer), message, args );
	va_end( args );
	SendToPlayers(buffer);
	SendToWatchers(buffer);
}
PokerPlayer* PokerTable::SetNextDealer()
{
	if( !Dealer )
	{
		return (Dealer = TableHost());//If there's no dealer, then the dealer is the host.
	}
	else
	{
		if( Dealer == Players.back() )//If we're at the last person in the list.
			return (Dealer = Players.front());//Dealer becomes the first person in the list.
		else
		{
			for(std::list<PokerPlayer*>::iterator P = Players.begin();P != Players.end();++P)
			{
				if( Dealer == (*P) )
				{
					//Dealer is next person after P.
					P++;
					return (Dealer = (*P));
				}
			}
		}
	}
	return 0;
}
void PokerTable::DealCards()
{
	for(std::list< PokerPlayer* >::iterator Player = Players.begin();Player != Players.end();++Player)
	{
#ifdef POKER_DEBUG
		if( !(*Player)->PlayerName().compare("Galnor") )
		{
			(*Player)->DealCard( GetCardFromDeck("KD") );
			(*Player)->DealCard( GetCardFromDeck("JD") );
		}
		else if( !(*Player)->PlayerName().compare("Kison") )
		{
			(*Player)->DealCard( GetCardFromDeck("JS") );
			(*Player)->DealCard( GetCardFromDeck("6H") );
		}
		else if( !(*Player)->PlayerName().compare("Test") )
		{
			(*Player)->DealCard( GetCardFromDeck("8D") );
			(*Player)->DealCard( GetCardFromDeck("AD") );
		}
		else
		{
#endif

			(*Player)->DealCard( GetCardFromDeck() );
			(*Player)->DealCard( GetCardFromDeck() );
#ifdef POKER_DEBUG
		}
#endif
		(*Player)->UnFold();
		(*Player)->PotSize( 0 );
		(*Player)->ResetHandValue();
		LastToShow = 0;
		LastToCall = 0;
		SendToTable( "%s has been dealt.\r\n", (*Player)->PlayerName().c_str() );
	}
}
Card* PokerTable::GetCardFromDeck( const std::string &Name )
{
	for(std::vector< Card* >::iterator C = Deck.begin();C != Deck.end();++C)
	{
		if( !Name.compare( (*C)->ToString() ) )
			return (*C);
	}
	return GetCardFromDeck();
}
Card* PokerTable::GetCardFromDeck()
{
	return Deck.at( MiscUtil::random(0, Deck.size() - 1) );
}
//Setup the turn cycle in accordance to a player.
void PokerTable::SetupTurns( PokerPlayer* From, bool exclude )
{
	while(!TurnCycle.empty()) TurnCycle.pop();
	std::list< PokerPlayer* >::iterator Player;

	for(Player = this->Players.begin();(*Player) != From;++Player);

	if( Player == Players.end() )
	{
		MudLog(BRF, LVL_APPR, TRUE, "Invalid player selected as central player for turn setup in poker game.\r\n");
		Player = Players.begin();
	}

	++Player;
	for(;Player != Players.end();++Player)
	{
		if( !(*Player)->Folded() && (*Player)->Chips())
			this->TurnCycle.push( (*Player) );
	}
	for(Player = this->Players.begin();(*Player) != From;++Player)
	{
		if( !(*Player)->Folded() && (*Player)->Chips())
			this->TurnCycle.push( (*Player) );
	}
	if( !exclude && !(*Player)->Folded() ) TurnCycle.push( (*Player) );

	SetLastToCall( 0 );
}
void PokerTable::RemovePlayer( class Character* ch )
{
	RemovePlayer( ch->PokerData );

	ch->save();

	delete ch->PokerData;
	ch->PokerData = 0;

	ch->send("You leave the table.\r\n");
	SendToTable("%s has left the table.\r\n", ch->player.name.c_str());
	GET_POS( ch ) = POS_STANDING;

	if(State == CONFIRMING && NumOfPlayers() == 1)
	{
		SendToTable("The game has reverted back to setup mode due to the lack of players.\r\n");
		Players.front()->RemoveConfirmation();
		State = SEATING;
	}
}
void PokerTable::RemovePlayer( PokerPlayer* Player )
{
	if( Player )
	{
		if(Players.size() != 1 && Player == Dealer)
		{
			this->SetNextDealer();
			DealerIsGone = true;
		}
		Players.remove( Player );
	}
}
PokerPlayer* PokerTable::NextUp()
{
	return (TurnCycle.size() ? TurnCycle.front() : 0);
}
PokerPlayer* PokerTable::TableHost()
{
	return (Players.size() ? Players.front() : 0);
}

void PokerTable::AddWatcher( Character *ch )
{
	this->Watchers.push_back(ch);
	ch->PokerTable = this;
}
void PokerTable::RemoveWatcher( Character *ch )
{
	this->Watchers.remove(ch);
	ch->PokerTable = NULL;
}
bool PokerTable::IsWatching( Character *ch )
{
	return (std::find( this->Watchers.begin(), this->Watchers.end(), ch ) != Watchers.end());
}

bool PokerTable::ProcessGameInput( PokerPlayer* Player, std::string Command, std::string Argument )
{

	if(State != PREFLOP && State != FLOP && State != TURN && State != RIVER)
		return false;

	if( (Player && Player != NextUp()) || GameIsOver())
	{
		if(!Command.compare("check") || !Command.compare("call") ||
		!Command.compare("raise") || !Command.compare("fold") ||
		!Command.compare("bet") )
		{
			Player->send("It is not your turn.\r\n");
			return true;
		}
		return false;
	}

	if(!Command.compare("check"))
	{
		if(Player->Bet() < Player->Table->CurrentBet())
		{
			Player->send("You must either call, raise, or fold, because your wager does not match the current bet.\r\n");
			return true;
		}
		this->SendToTable("%s checks.\r\n", TurnCycle.front()->PlayerName().c_str());
		TurnCycle.pop();
	}
	else if(!Command.compare("raise") || !Command.compare("bet"))
	{
		if(CurrentBet() > Player->Chips())
		{
			Player->send("You do not have enough chips to raise. You must either call or fold.\r\n");
			return true;
		}
		if(atoi(Argument.c_str()) + CurrentBet() > Player->Bet() + Player->Chips())
		{
			Player->send("You do not have enough chips to raise that high.\r\n");
			return true;
		}
		if(atoi(Argument.c_str()) < MinimumRaise)
		{
			if(Player->Chips() < MinimumRaise && atoi(Argument.c_str()) != Player->Chips())
			{
				Player->send("You must raise by a minimum of %d.\r\n", Player->Chips());
				return true;
			}
			else if(Player->Chips() >= MinimumRaise)
			{
				Player->send("You must raise by a minimum of %d.\r\n", MinimumRaise);
				return true;
			}
		}
		int bet_to = atoi(Argument.c_str()) + CurrentBet(), highest_bet = 0;

		for(std::list<PokerPlayer*>::iterator P = Players.begin();P != Players.end();++P)
		{
			if( (*P) == Player || (*P)->Folded() )
				continue;
			highest_bet = MAX(highest_bet, (*P)->Bet() + (*P)->Chips());
		}
		bet_to = MIN(bet_to, highest_bet);

		SendToTable("%s raises to %d.\r\n", Player->PlayerName().c_str(), bet_to);
		Player->RaiseBetTo( bet_to );
		Player->Table->SetupTurns( Player, true );
	}
	else if(!Command.compare("fold"))
	{
		Player->Fold();
		TurnCycle.pop();
	}
	else if(!Command.compare("call"))
	{
		if( Player->Bet() >= CurrentBet() )
		{
			Player->send("Your bet already matches the pot. Simply check.\r\n");
			return true;
		}
		else
			Player->Call();
		TurnCycle.pop();
	}
	else
		return false;
	if(!Player->Chips())
		SendToTable("%s is all in!\r\n", Player->PlayerName().c_str());
	NextGameStep();

	return true;
}

void PokerTable::PrintCommunityCards()
{
	SendToTable("Community Cards: ");
	for(std::vector< Card* >::iterator card = this->CommunityCards.begin();card != this->CommunityCards.end();++card)
	{
		if(card != CommunityCards.begin())
			SendToTable(", ");
		SendToTable((*card)->ToString().c_str());
	}
	SendToTable("\r\n");
}

HandValue* PokerPlayer::GetHandValue()
{
	if(HV)
		return HV;

	HV = new HandValue();
	std::vector< Card* > PlayerHand = Table->GetCommunityCards();

	PlayerHand.push_back( this->GetCard( 1 ) );
	PlayerHand.push_back( this->GetCard( 2 ) );

	HV->Hand = HV->val1 = HV->val2 = -1;

	if( SevenCardHand::HasStraightFlush( PlayerHand, HV->val1 ) )
		HV->Hand = STRAIGHT_FLUSH;
	else if( SevenCardHand::HasFourOfAKind( PlayerHand, HV->val1, HV->High ) )
		HV->Hand = FOUR_OF_A_KIND;
	else if( SevenCardHand::HasFullHouse( PlayerHand, HV->val1, HV->val2 ))
		HV->Hand = FULL_HOUSE;
	else if( SevenCardHand::HasFlush( PlayerHand, HV->val1 ) )
		HV->Hand = FLUSH;
	else if( SevenCardHand::HasStraight( PlayerHand, HV->val1 ) )
		HV->Hand = STRAIGHT;
	else if( SevenCardHand::HasThreeOfAKind( PlayerHand, HV->val1, HV->High ) )
		HV->Hand = THREE_OF_A_KIND;
	else if( SevenCardHand::HasTwoPair( PlayerHand, HV->val1, HV->val2, HV->High ) )
		HV->Hand = TWO_PAIR;
	else if( SevenCardHand::HasPair( PlayerHand, HV->val1, HV->High ) )
		HV->Hand = PAIR;
	if(HV->Hand == -1)
	{
		std::list< int > Blank;
		HV->High = SevenCardHand::FindHighCards( PlayerHand, Blank );
		HV->val1 = HV->val2 = -1;
	}
	return HV;

}
std::string HandValue::HandName()
{
	std::string Name;
	switch( Hand )
	{
	case PAIR:
		Name = "a pair of ";
		Name += Poker::CardLetter( val1 );
		Name += "'s";
		return Name;
	case TWO_PAIR:
		Name = "two pairs: ";
		Name += Poker::CardLetter( val1 );
		Name += "'s and ";
		Name += Poker::CardLetter( val2 );
		Name += "'s";
		return Name;
	case THREE_OF_A_KIND:
		Name = "three of a kind: ";
		Name += Poker::CardLetter( val1 );
		Name += "'s";
		return Name;
	case STRAIGHT:
		Name = "a straight, ";
		Name += Poker::CardLetter( val1 );
		Name += " high";
		return Name;
	case FLUSH:
		Name = "a flush, ";
		Name += Poker::CardLetter( val1 );
		Name += " high";
		return Name;
	case FULL_HOUSE:
		Name = "a full house: ";
		Name += Poker::CardLetter( val1 );
		Name += "'s full of ";
		Name += Poker::CardLetter( val2 );
		Name += "'s";
		return Name;
	case FOUR_OF_A_KIND:
		Name = "four of a kind: ";
		Name += Poker::CardLetter( val1 );
		Name += "'s";
		return Name;
	case STRAIGHT_FLUSH:
		Name = "a straight-flush, ";
		Name += Poker::CardLetter( val1 );
		Name += " high";
		return Name;
	default:
		Name = "a high card of ";
		Name += Poker::CardLetter( High.front() );
		return Name;
	}
}
bool HandValue::BetterThan( HandValue* OtherHand )
{
	return( Hand > OtherHand->Hand ||
	(Hand == OtherHand->Hand && (val1 > OtherHand->val1 || (val1 == OtherHand->val1 &&
	( val2 > OtherHand->val2 || (val2 == OtherHand->val2 &&
	SevenCardHand::BetterHighCards( High, OtherHand->High) == BETTER ) )))));
}
bool HandValue::WorseThan( HandValue* OtherHand )
{
	return (!WorseThan( OtherHand ) && !SameAs( OtherHand ));
}
bool HandValue::SameAs( HandValue* OtherHand )
{
	return
	( Hand == OtherHand->Hand && val1 == OtherHand->val1 &&
	val2 == OtherHand->val2 &&
	SevenCardHand::BetterHighCards( High, OtherHand->High ) == SAME
	);

}
void PokerTable::FindWinners( std::list< std::list< PokerPlayer* > > &Winners )
{
	std::list< PokerPlayer* > CurrentWinners;

	for( std::list< PokerPlayer* >::iterator p1 = Players.begin();p1 != Players.end();++p1)
	{
		if( (*p1)->Folded() )
			continue;
		std::list< PokerPlayer* > CurrentList;

		CurrentList.push_back( (*p1) );
		for( std::list< PokerPlayer* >::iterator p2 = p1;p2 != Players.end();++p2)
		{
			if( (*p2)->Folded() || (*p1) == (*p2))
				continue;
			if( (*p1)->GetHandValue()->SameAs((*p2)->GetHandValue()) )
			{
				CurrentList.push_back( (*p2) );
			}
		}
		std::list< std::list<PokerPlayer*> >::iterator Group;
		for(Group = Winners.begin();Group != Winners.end();++Group)
		{
			if( CurrentList.front()->GetHandValue()->BetterThan((*Group).front()->GetHandValue()) )
				break;
		}
		Winners.insert( Group, CurrentList);
	}

//	for(std::list< PokerPlayer* >::iterator BestPlayer = Winners.begin();BestPlayer != Winners.end();++BestPlayer)
//		SendToTable("%s is the winner!\r\n", (*BestPlayer)->PlayerName().c_str());
}
void PokerTable::PrintBoard( Character *ch, const int wait_time )
{
	if( wait_time )
	{
		add_event(wait_time, PokerPrint, this);
		return;
	}
	char Buffer[ MAX_STRING_LENGTH ];
	char Line1[ 1024 ], Line2[ 1024 ], Line3[ 1024 ];
	sprintf(Buffer, "\r\n");

	*Line1 = *Line2 = *Line3 = *Buffer = 0;

	PokerPlayer* Player, *PlayerB;

//30 spaces in
	if( (Player = GetPlayerAtSeat( 1 )) )
	{
		sprintf(Line1, "               %-10s", Player->PlayerName(true).c_str());
		sprintf(Line2, "                 %s", Player->ChipStr(true).c_str());
		sprintf(Line3, "                 %s", Player->BetStr(true).c_str());
	}
	while( StringUtil::strlenIgnoreColors( Line1 ) < 38) strcat( Line1, " " );
	while( StringUtil::strlenIgnoreColors( Line2 ) < 40) strcat( Line2, " " );
	while( StringUtil::strlenIgnoreColors( Line3 ) < 40) strcat( Line3, " " );

	if( (Player = GetPlayerAtSeat( 2 )) )
	{
		strcat( Line1, Player->PlayerName(true).c_str() );
		sprintf( Line2 + strlen( Line2 ), "%s", Player->ChipStr(true).c_str());
		sprintf( Line3 + strlen( Line3 ), "%s", Player->BetStr(true).c_str());
	}
	sprintf( Buffer, "%s\r\n", Line1 );
	sprintf( Buffer + strlen( Buffer ), "%s\r\n", Line2 );
	sprintf( Buffer + strlen( Buffer ), "%s\r\n", Line3 );
	*Line1 = 0; *Line2 = *Line3 = 0;

	sprintf( Buffer + strlen( Buffer ), "                  ______________________________________\r\n");
	sprintf( Buffer + strlen( Buffer ), "                 |                                      |\r\n");


	if( (Player = GetPlayerAtSeat( 10 )) )
	{
		sprintf( Line1, "%s", Player->PlayerName(true).c_str() );
		sprintf( Line2, "  %s", Player->ChipStr(true).c_str() );
		sprintf( Line3, "  %s", Player->BetStr(true).c_str() );
	}

	while( StringUtil::strlenIgnoreColors( Line1 ) < 17) strcat( Line1, " " );
	while( StringUtil::strlenIgnoreColors( Line2 ) < 17) strcat( Line2, " " );
	while( StringUtil::strlenIgnoreColors( Line3 ) < 17) strcat( Line3, " " );

	PlayerB = GetPlayerAtSeat( 3 );
	sprintf( Line1 + strlen( Line1 ), "|                                      | %s\r\n", PlayerB?PlayerB->PlayerName(true).c_str():"");
	sprintf( Line2 + strlen( Line2 ), "|                                      |   %s\r\n", PlayerB?PlayerB->ChipStr(true).c_str():"");
	sprintf( Line3 + strlen( Line3 ), "|   ____   ____   ____   ____   ____   |   %s\r\n", PlayerB?PlayerB->BetStr(true).c_str():"");

	strcat(Buffer, Line1);
	strcat(Buffer, Line2);
	strcat(Buffer, Line3);
	*Line1 = 0; *Line2 = *Line3 = 0;

	std::string Card1, Card2, Card3, Card4, Card5;

	if(CommunityCards.size())
	{
		Card1 = CommunityCards[ 0 ]->ToString();
		Card2 = CommunityCards[ 1 ]->ToString();
		Card3 = CommunityCards[ 2 ]->ToString();
	}
	if(CommunityCards.size() >= 4)
		Card4 = CommunityCards[ 3 ]->ToString();
	if(CommunityCards.size() >= 5)
		Card5 = CommunityCards[ 4 ]->ToString();

	sprintf( Buffer + strlen( Buffer ), "                 |  |    | |    | |    | |    | |    |  |\r\n");
	sprintf( Buffer + strlen( Buffer ), "                 |  |    | |    | |    | |    | |    |  |\r\n");
	sprintf( Buffer + strlen( Buffer ), "                 |  | %-3s| | %-3s| | %-3s| | %-3s| | %-3s|  |\r\n",
		Card1.c_str(), Card2.c_str(), Card3.c_str(), Card4.c_str(), Card5.c_str());


	if( (Player = GetPlayerAtSeat( 9 )) )
	{
		sprintf( Line1, "%s", Player->PlayerName(true).c_str() );
		sprintf( Line2, "  %s", Player->ChipStr(true).c_str() );
		sprintf( Line3, "  %s", Player->BetStr(true).c_str() );
	}

	while( StringUtil::strlenIgnoreColors( Line1 ) < 17) strcat( Line1, " " );
	while( StringUtil::strlenIgnoreColors( Line2 ) < 17) strcat( Line2, " " );
	while( StringUtil::strlenIgnoreColors( Line3 ) < 17) strcat( Line3, " " );

	PlayerB = GetPlayerAtSeat( 4 );
	sprintf( Line1 + strlen( Line1 ), "|  |    | |    | |    | |    | |    |  | %s\r\n", PlayerB?PlayerB->PlayerName(true).c_str():"");
	sprintf( Line2 + strlen( Line2 ), "|  |____| |____| |____| |____| |____|  |   %s\r\n", PlayerB?PlayerB->ChipStr(true).c_str():"");
	sprintf( Line3 + strlen( Line3 ), "|                                      |   %s\r\n", PlayerB?PlayerB->BetStr(true).c_str():"");

	strcat(Buffer, Line1);
	strcat(Buffer, Line2);
	strcat(Buffer, Line3);
	*Line1 = 0; *Line2 = *Line3 = 0;


//	sprintf( Buffer + strlen( Buffer ), "                 |  |    | |    | |    | |    | |    |  |\r\n");
//	sprintf( Buffer + strlen( Buffer ), "                 |  |____| |____| |____| |____| |____|  |\r\n");
//	sprintf( Buffer + strlen( Buffer ), "                 |                                      |\r\n");


	//..Pot...
	sprintf( Line1, "                 |            POT: %d", Pot);
	while(StringUtil::strlenIgnoreColors(Line1) < 56) strcat(Line1, " ");
	sprintf(Buffer + strlen(Buffer), "%s|\r\n", Line1);
	*Line1 = 0;

	//End of Pot

	sprintf( Buffer + strlen( Buffer ), "                 |                                      |\r\n");

	if( (Player = GetPlayerAtSeat( 8 )) )
	{
		sprintf( Line1, "%s", Player->PlayerName(true).c_str() );
		sprintf( Line2, "  %s", Player->ChipStr(true).c_str() );
		sprintf( Line3, "  %s", Player->BetStr(true).c_str() );
	}

	while( StringUtil::strlenIgnoreColors( Line1 ) < 17) strcat( Line1, " " );
	while( StringUtil::strlenIgnoreColors( Line2 ) < 17) strcat( Line2, " " );
	while( StringUtil::strlenIgnoreColors( Line3 ) < 60) strcat( Line3, " " );


	PlayerB = GetPlayerAtSeat( 5 );
	sprintf( Line1 + strlen( Line1 ), "|                                      | %s\r\n", PlayerB?PlayerB->PlayerName(true).c_str():"");
	sprintf( Line2 + strlen( Line2 ), "|______________________________________|   %s\r\n", PlayerB?PlayerB->ChipStr(true).c_str():"");
	sprintf( Line3 + strlen( Line3 ), "%s\r\n", PlayerB?PlayerB->BetStr(true).c_str():"");

	strcat(Buffer, Line1);
	strcat(Buffer, Line2);
	strcat(Buffer, Line3);
	*Line1 = *Line2 = *Line3 = 0;

	strcat(Buffer, "\r\n");

	if( (Player = GetPlayerAtSeat( 6 )) )
	{
		sprintf(Line1, "               %s", Player->PlayerName(true).c_str());
		sprintf(Line2, "                 %s", Player->ChipStr(true).c_str());
		sprintf(Line3, "                 %s", Player->BetStr(true).c_str());
	}
	while( StringUtil::strlenIgnoreColors( Line1 ) < 38) strcat( Line1, " " );
	while( StringUtil::strlenIgnoreColors( Line2 ) < 40) strcat( Line2, " " );
	while( StringUtil::strlenIgnoreColors( Line3 ) < 40) strcat( Line3, " " );

	if( (Player = GetPlayerAtSeat( 7 )) )
	{
		strcat( Line1, Player->PlayerName(true).c_str() );
		strcpy( Line2 + strlen( Line2 ), Player->ChipStr(true).c_str());
		strcpy( Line3 + strlen( Line3 ), Player->BetStr(true).c_str());
	}
	sprintf( Buffer + strlen( Buffer ), "%s\r\n", Line1 );
	sprintf( Buffer + strlen( Buffer ), "%s\r\n", Line2 );
	sprintf( Buffer + strlen( Buffer ), "%s\r\n", Line3 );
	*Line1 = 0; *Line2 = *Line3 = 0;

	sprintf( Buffer + strlen( Buffer ), "\r\n");

	if( ch != NULL )
		ch->send( Buffer );
	else
		SendToTable( Buffer );

	if( ch && ch->PokerData )
		ch->PokerData->PrintHand();
	else if( !ch )
	{
		//Display player's hand...
		for(std::list< PokerPlayer* >::iterator P = Players.begin();P != Players.end();++P)
		{
			(*P)->PrintHand();
		}
	}
}

PokerPlayer* PokerTable::GetPlayerAtSeat( const int seat )
{
	for(std::list< PokerPlayer* >::iterator P = Players.begin();P != Players.end();++P)
	{
		if( (*P)->Seat() == seat)
			return (*P);
	}
	return 0;
}
bool PokerTable::AllConfirmed()
{
	for(std::list<PokerPlayer*>::iterator P = Players.begin();P != Players.end();++P)
	{
		if(!(*P)->Confirmed())
			return false;
	}
	return true;
}
void PokerTable::AccumulateBets()
{
	int quantity = 0;
	SetPotSizes();
	for(std::list< PokerPlayer* >::iterator P = Players.begin();P != Players.end();++P)
	{
		quantity += (*P)->Bet();
		(*P)->Bet( 0 );
	}
	Pot += quantity;
	current_bet = 0;
}
void PokerTable::RewardWinners( std::list<PokerPlayer* > &Winners )
{
	std::list< int > Pots;
	for(std::list< PokerPlayer* >::iterator P = Winners.begin();P != Winners.end();++P)
	{
		if( std::find(Pots.begin(), Pots.end(), (*P)->PotSize() ) == Pots.end() )
			InsertionSort( Pots, (*P)->PotSize(), 100 );
	}
	Pots.reverse();

	int deduction = 0;
	for(std::list< int >::iterator CurPot = Pots.begin();CurPot != Pots.end();++CurPot)
	{
		(*CurPot) -= deduction;
		deduction += (*CurPot);
		int num = Winners.size();
		for(std::list<PokerPlayer*>::iterator P = Winners.begin();P != Winners.end();)
		{
			Log("Rewarding: %s", (*P)->GetPlayer()->player.name.c_str());
			(*P)->Chips( (*P)->Chips() + (*CurPot) / num );

			if( (*P)->PotSize() == (*CurPot) )
			{
				if((P = Winners.erase( P )) == Winners.end())
					break;
			}
			else
			{
				++P;
			}
		}
		(*CurPot) = 0;
	}

	Pot -= deduction;
}
void PokerTable::SetPotSizes()
{
	for(std::list<PokerPlayer*>::iterator P = Players.begin();P != Players.end();++P)
	{
		for(std::list<PokerPlayer*>::iterator P2 = Players.begin();P2 != Players.end();++P2)
		{
			(*P)->PotSize( (*P)->PotSize() + MIN( (*P2)->Bet(), (*P)->Bet() ) );
		}
	}
}
void PokerTable::EliminateLostPlayers()
{
	std::list<PokerPlayer*> PlayersToRemove;
	for(std::list< PokerPlayer* >::iterator Player = Players.begin();Player != Players.end();++Player)
	{
		if( !(*Player)->Chips() )
		{
			PlayersToRemove.push_back( (*Player) );
		}
	}
	for(std::list< PokerPlayer* >::iterator Player = PlayersToRemove.begin();Player != PlayersToRemove.end();++Player)
	{
		SendToTable("%s has been eliminated!\r\n", (*Player)->PlayerName().c_str());
		Log("Removing Player: %s", (*Player)->PlayerName().c_str());
		RemovePlayer( (*Player)->GetPlayer() );
	}
}
void PokerTable::EndGame()
{
	if( !LastToShow && State == RIVER)
	{
		PokerShowCards( this );
		return;
	}

	AccumulateBets();

	if(NumberOfUnfoldedPlayers() == 1)
	{
		std::list<PokerPlayer*>::iterator P;
		for( P = Players.begin();P != Players.end() && (*P)->Folded();++P);
		SendToTable("%s wins by default.\r\n", (*P)->PlayerName().c_str());

		std::list< PokerPlayer* > TheWinner;
		TheWinner.push_back( (*P) );
		RewardWinners( TheWinner );
	}
	else
	{
		std::list< std::list<PokerPlayer* > > Winners;
		FindWinners( Winners );
		Log("Winners found: %lld", (__int64)Winners.size());

		int nr = 0;
		for(std::list< std::list< PokerPlayer* > >::iterator Group = Winners.begin();Pot > 0;++Group, ++nr)
		{
			if( Group == Winners.end() )
			{
				MudLog(BRF, LVL_APPR, TRUE, "Poker table in room %d still has pot remainder of %d after pot distribution.", this->InRoom->getName(), Pot);
				break;
			}
			if(!nr)
			{
				std::stringstream Stream;
				for(std::list< PokerPlayer* >::iterator P = (*Group).begin();P != (*Group).end();++P)
				{
					if( P != (*Group).begin() && (*Group).size() >= 3 )
						Stream << ",";
					if( (*P) == (*Group).back() && (*Group).size() > 1 )
						Stream << " and ";
					Stream << (*P)->PlayerName();
				}
				Stream << " " << ((*Group).size() == 1 ? "has" : "have") << " won with " << (*Group).front()->GetHandValue()->HandName() << "." << std::endl;
				SendToTable( Stream.str().c_str() );
			}
			RewardWinners( (*Group) );
		}
		EliminateLostPlayers();
	}
	Pot = 0;

	for(std::list< PokerPlayer* >::iterator Player = Players.begin();Player != Players.end();++Player)
	{
		(*Player)->ClearHand();
	}

	if(Players.size() == 1)//We have a winner!
	{
		CloseTable();
		return;
	}

	add_event(5, PokerShuffle, this);
}
void PokerTable::CloseTable()
{
	int reward = stakes*num_init_players;
	reward = (int)(reward * 0.9000f);

	Players.front()->RewardWithGold( reward );
	Players.front()->send("You have been rewarded %d coppers.\r\n", reward);
	RemovePlayer( Players.front()->GetPlayer() );
}
bool PokerTable::CollectWagers()
{
	bool cant_afford = false;
	for(std::list<PokerPlayer*>::iterator P = Players.begin();P != Players.end();++P)
	{
		if( (*P)->PlayerGold() < Stakes() )
		{
			cant_afford = true;
			SendToTable("%s cannot afford the wager.\r\n", (*P)->PlayerName().c_str());
		}
	}

	if(cant_afford) return false;

	for(std::list<PokerPlayer*>::iterator P = Players.begin();P != Players.end();++P)
	{
		(*P)->DivyUp();
	}
	num_init_players = Players.size();
	return true;
}
int PokerTable::NumberOfUnfoldedPlayers()
{
	int num = 0;
	for(std::list<PokerPlayer*>::iterator P = Players.begin();P != Players.end();++P)
	{
		if( !(*P)->Folded() )
			++num;
	}
	return num;
}
int PokerTable::NumberOfActivePlayers()
{
	int num = 0;
	for(std::list<PokerPlayer*>::iterator P = Players.begin();P != Players.end();++P)
	{
		if( !(*P)->Folded() && (*P)->Chips() > 0 )
			++num;
	}
	return num;
}
void PokerTable::PerformBlinds()
{
	TurnCycle.front()->Raise( MinimumRaise / 2 );
	TurnCycle.pop();
	TurnCycle.front()->Raise( MinimumRaise / 2 ); //Raise ANOTHER half.
	SetupTurns( TurnCycle.front(), false );
}
void PokerTable::Flop()
{
	this->GetCardFromDeck()->MoveTo( this->CommunityCards, this );
	this->GetCardFromDeck()->MoveTo( this->CommunityCards, this );
	this->GetCardFromDeck()->MoveTo( this->CommunityCards, this );
	State = FLOP;
	AccumulateBets();
	SetupTurns( Dealer, false );
}
void PokerTable::Turn()
{
	this->GetCardFromDeck()->MoveTo( this->CommunityCards, this );
	State = TURN;
	AccumulateBets();
	SetupTurns( Dealer, false );
}
void PokerTable::River()
{
	this->GetCardFromDeck()->MoveTo( this->CommunityCards, this );
	State = RIVER;
	AccumulateBets();
	SetupTurns( Dealer, false );
}
void PokerTable::NextGameStep()
{
	PokerState Before = State;

	bool end_game = false;
	if( State == SEATING )
	{
		SendToPlayers("The game is ready to begin. You will pay %d coppers if you lose. Please confirm or deny.\r\n", Stakes() );
		SendToWatchers("The game is ready to begin. Each player will pay %d coppers if they lose.\r\n", Stakes());
		State = CONFIRMING;
	}
	else if( State == CONFIRMING || State == DEAL )
	{
		if( AllConfirmed() )
		{
			if(State == CONFIRMING)
				MinimumRaise = 30;//This only happens once.
			PokerShuffle( this );
		}
	}
	else if( GameIsOver() )
	//Game over
	{
		end_game = true;
		EndGame();
	}
	else if( TurnCycle.empty() ) // Ready for next game-stage.
	{
		if( State == PREFLOP )
		{
			Flop();
		}
		else if( State == FLOP )
		{
			Turn();
			}
		else if( State == TURN )
		{
			River();
		}
		if( NumberOfActivePlayers() <= 1 && Before != RIVER) //Auto-complete.
		{
			add_event(16, PokerAutoComplete, this);
			return;
		}
	}

	if(Before == PREFLOP || Before == FLOP || Before == TURN || Before == RIVER)
	{
		if( Players.size() != 1 )
		{
			if( !end_game )
			{
				PrintBoard();
			}
		}
		else
			delete this;
	}
}
bool PokerTable::GameIsOver()
{
	return (NumberOfUnfoldedPlayers() <= 1 || (State == RIVER && TurnCycle.empty()));
}
void PokerTable::PrintPlayers(Character *ch)
{
	if( !this->Players.size() )
	{
		ch->send("The poker table currently is empty.\r\n");
		return;
	}
	else
	{
		int nr = 1;
		ch->send("Players currently seated as players at the table:\r\n\r\n");
		for( std::list<PokerPlayer*>::iterator P = Players.begin();P != Players.end();++P, ++nr )
		{
			ch->send("%d) %s\r\n", nr, (*P)->PlayerName().c_str());
		}
		nr = 1;
		ch->send("Players currently watching the table:\r\n\r\n");
		for( std::list<Character*>::iterator W = Watchers.begin();W != Watchers.end();++W, ++nr )
		{
			ch->send("%d) %s\r\n", nr, (*W)->player.name.c_str());
		}
	}
}
void PokerTable::PrintBanList()
{
	std::stringstream Format;

	if( !BanList.size() )
		Format << "No-one has been banned from this table." << std::endl;
	else
	{
		Format << "The following players have been banned from this table:\r\n" << std::endl;
		for(std::list< std::string >::iterator B = BanList.begin();B != BanList.end();++B)
		{
			Format << (*B) << std::endl;
		}
	}
	SendToTable( Format.str().c_str() );
}
//If adding a Character*, AddPlayer() will be used to initiate the PokerPlayer's
//data for the player. However, if adding a PokerPlayer*, no changes will be made,
//allowing it to be used for moving players between tables.
void PokerTable::AddPlayer( Character* _Player )
{
	PokerPlayer *P = new PokerPlayer( _Player, this );
	this->AddPlayer( P );
#ifdef POKER_DEBUG
	if( P->PlayerName() == "Galnor" )
		P->Chips( 550 );
	else if( P->PlayerName() == "Kison" )
		P->Chips( 1500 );
	else if( P->PlayerName() == "Test" )
		P->Chips( 1200 );
	else
#endif
		P->Chips( 800 );	//800 chips default ?

	_Player->PokerData = P;
}
void PokerTable::AddPlayer( PokerPlayer* _Player )
{
	SendToTable("%s has joined the table.\r\n", (_Player)->PlayerName().c_str());
	Players.push_back( _Player );

	int seat;
	for(seat = 0; GetPlayerAtSeat( ++seat ); );

	_Player->Seat( seat ); //Sit them at this seat.

}
void PokerTable::MinRaise( const int _MinRaise )
{
	MinimumRaise = _MinRaise;
}
int PokerTable::NumOfPlayers()
{
	return Players.size();
}
bool PokerTable::HasStarted()
{
	return State != SEATING;
}
void PokerTable::RemoveCardFromDeck( Card* _Card )
{
	std::vector< Card* >::iterator cIter;
	for(cIter = Deck.begin();(*cIter) != _Card && cIter != Deck.end();++cIter);

	if(cIter != Deck.end())
		this->Deck.erase( cIter );
#if (DEBUG == 1)
	else
	{
		std::cout << "PokerTable::RemoveCardFromDeck() : Attempting to erase invalid card: " << _Card->ToString() << std::endl;
		exit( 1 );
	}
#endif
}
const int PokerTable::MinRaise( void )
{
	return MinimumRaise;
}



	/**** Player Poker Data ****/
PokerPlayer::PokerPlayer( Character* _Player, PokerTable* _Table )
{
	Player = _Player;
	Table  = _Table;
	bet = 0;
	chips = 0;
	confirmed = false;
	folded = false;
}
PokerPlayer::PokerPlayer()
{
	Player = 0;
	Table  = 0;
	bet    = 0;
	chips  = 0;
	confirmed = false;
	folded = false;
}
void PokerPlayer::Fold()
{
	folded = true;
	this->Table->SendToTable("%s has folded.\r\n", PlayerName().c_str());
}
bool PokerPlayer::Folded()
{
	return folded;
}
void PokerPlayer::ShowHandToTable()
{
	Table->SendToTable("%s's hand: %s, %s\r\n",
		PlayerName().c_str(), GetCard(1)->ToString().c_str(), GetCard(2)->ToString().c_str());
	Table->SendToTable("%s has %s\r\n", PlayerName().c_str(), GetHandValue()->HandName().c_str());
}
std::string PokerPlayer::PlayerName( bool board )
{
	std::string Output;

	if(board && Table->NextUp() == this)
	{
		Output += BOLD;
		Output += GREEN;
	}

	if( Table->GetDealer() == this && board )
		Output += "*";

	Output += Player->player.name;
	if( board )
	{
		Output += ":";
		if(Folded())	Output += "Folded";
		else			Output += "Playing";
	}

	if(board && Table->NextUp() == this)
		Output += NORMAL;
	return Output;
}
Card* PokerPlayer::GetCard( int nr )
{
	if( Hand.size() >= (nr - 1) )
		return Hand.at( (nr - 1) );
	return 0;
}
std::string PokerPlayer::ChipStr( bool board )
{
	std::string Output;

	if(board && Table->NextUp() == this)
	{
		Output += BOLD;
		Output += GREEN;
	}

	Output  += "C:";
	Output += MiscUtil::Convert<std::string, int>(Chips());

	if(board && Table->NextUp() == this)
	{
		Output += NORMAL;
	}

	return Output;
}
std::string PokerPlayer::BetStr( bool board )
{
	std::string Output;

	if(board && Table->NextUp() == this)
	{
		Output += BOLD;
		Output += GREEN;
	}

	Output  += "B:";
	Output += MiscUtil::Convert<std::string, int>(Bet());

	if(board && Table->NextUp() == this)
	{
		Output += NORMAL;
	}

	return Output;
}
void PokerPlayer::send(const char* message, ...)
{
	char buffer[MAX_STRING_LENGTH];
	va_list args;
	va_start( args, message );
	vsprintf( buffer, message, args );
	va_end( args );

	this->Player->send( buffer );
}
void PokerPlayer::RaiseBetTo( const int _amt )
{
	Chips( Chips() - (_amt - Bet()) ); //Subtract new bet & old bet difference from player's chips.
	bet = _amt;//Set player's current bet to the amount.
	if(_amt > Table->CurrentBet())
		Table->CurrentBet( _amt );//Update the current stakes to his new bet.
}
//Pre-condition: _amt > Table->MinimumRaise
void PokerPlayer::Raise( const int amt )
{
	RaiseBetTo( MIN(Table->CurrentBet() + Chips(), Table->CurrentBet() + amt) );

	Table->SendToTable("%s raises to %d.\r\n", PlayerName().c_str(), Table->CurrentBet());
}
void PokerPlayer::Call()
{
	Table->SendToTable("%s calls.\r\n", PlayerName().c_str());

	RaiseBetTo( MIN(Table->CurrentBet(), Chips() + Bet()) );
	Table->SetLastToCall( this );
}

void PokerPlayer::Chips( const int _chips )
{
	chips = _chips;
}
const int PokerPlayer::Chips( void )
{
	return chips;
}
void PokerPlayer::ClearHand()
{
	Hand.clear();
}
void PokerPlayer::DealCard( class Card* _Card )
{
#if (DEBUG == 1)
	if(Hand.size() >= 2)
		std::cout << "Attempting to deal " << Player->getName() << " card #" << (Hand.size() + 1) << "." << std::endl;
#endif
	_Card->MoveTo( Hand, this->Table);
}
void PokerPlayer::PrintHand( void )
{
	std::string Output;

	if( !this->Hand.size() ) return;

	Output += this->GetCard( 1 )->ToString();
	Output += ",";
	Output += this->GetCard( 2 )->ToString();

	this->send("Your Hand: %s\r\n", Output.c_str());
}



	/**** Card Data ****/
Card::Card()
{
	value = suit = 0;
}
Card::Card( int _value, int _suit )
{
	value = _value;
	suit  = _suit;
}
template< class _Container >
void Card::MoveTo( _Container &Destination, PokerTable* Table )
{
	Destination.push_back( this );
	Table->RemoveCardFromDeck( this );
}

std::string Card::ToString( void )
{
	std::string Name = Poker::CardLetter( value );

	switch( suit )
	{
	case 0:Name += "C";break;
	case 1:Name += "D";break;
	case 2:Name += "H";break;
//	case 1:Name = std::string(RED) + std::string(BOLD) + Name + std::string("D") + std::string(NORMAL);break;
//	case 2:Name = std::string(RED) + std::string(BOLD) + Name + std::string("H") + std::string(NORMAL);break;
	case 3:Name += "S";break;
#if (DEBUG == 1)
	default:std::cout << "Card::ToString() : Invalid card suit " << suit << std::endl;
		exit(1);
#endif
	}
	return Name;
}
