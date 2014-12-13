
#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "db.h"
#include "md5.h"
#include "screen.h"
#include "accounts.h"
#include "interpreter.h"
#include "olc.h"
#include "comm.h"
#include "kuDescriptor.h"
#include "editor-interface/EditorInterface.h"

#include "stats.h"
#include "telnet.h"
#include "Descriptor.h"
#include "rooms/Room.h"
#include "rooms/RoomSector.h"
#include "UserMacro.h"
#include "UserEmailAddress.h"
#include "StringUtil.h"
#include "CharacterUtil.h"
#include "MobLoadLogger.h"

extern kuDescriptor *gatewayConnection;
extern Descriptor *descriptor_list;
extern sql::Connection gameDatabase;

void show_string(Descriptor *d, char *input);
void string_add(Descriptor *d, char *str);
int performDupeCheck( Descriptor *d );
extern int circle_restrict;
extern char *imotd;
extern char *motd;
extern Character *character_list;
extern int boot_high;
void UpdateBootHigh( const int new_high, bool first=false );
void js_enter_game_trigger(Character *self, Character *actor);

Descriptor::Descriptor()
{
	descriptor = 0;
	memset( &host, 0, sizeof( host ) );
	bad_pws = 0;
	idle_tics = 0;
	connected = 0;
	wait = 0;
	desc_num = 0;
	showstr_head = 0;
	showstr_vector= 0;
	showstr_count = 0;
	showstr_page = 0;
	str = 0;
	max_str = 0;
	backstr = 0;
	character = 0;
	original = 0;
	snooping = 0;
	snoop_by = 0;
	next = 0;
	olc = 0;
	loggedIn = false;
}

Descriptor::~Descriptor()
{
}

void Descriptor::initMock(Character *ch)
{
	descriptor = new kuDescriptor(nullptr);
	strcpy(host, "127.0.0.1");
	setGatewayDescriptorType(GatewayDescriptorType::unknown);
	connected = CON_PLAYING;
	idle_tics = 0;
	wait = 0;
	loginTime = DateTime();
	hadInput = false;
	hadOutput = false;
	desc_num = -1;
	character = ch;
	next = nullptr;
}

void Descriptor::closeSocketClean()
{
	gatewayConnection->send("Close " + session + "\n");

	this->descriptor->socketClose();
}

void Descriptor::processInput()
{
	if(commandQueue.empty())
		return;
	if(character && character->wait > 0)
		return;

	std::string command = commandQueue.front();
	commandQueue.pop_front();
	
	int aliased = 0;
	hadInput = true;

	//Process Telnet input.

	for(int pos = 0;pos < (int)command.size();++pos)
	{
		char ch = command[pos];
		if(ch == '\b' || ch == 127) {

			if(pos == 0) {

				command.erase(pos,1);
				--pos;
			}
			else {

				command.erase((pos-1),2);
				pos -= 2;
			}
		}
		else if(!isascii(ch) || !isprint(ch)) {
			command.erase(pos, 1);
			--pos;
		}
	}

	Character *loggingCharacter = character;
	if(original)
		loggingCharacter = original;

	if(loggingCharacter && STATE(this) != CON_PASSWORD)
		loggingCharacter->LogOutput(command + "\n");

	if(command.length() > MAX_INPUT_LENGTH) {

		send("Input too long... Truncated.\r\n");

		command.resize(MAX_INPUT_LENGTH);
	}

	if(STATE(this) != CON_PASSWORD) {

		StringUtil::replace(command, "$", "$$");
	}

	if(snoop_by && snoop_by->descriptor && snoop_by->character && snoop_by->character->hasPermissionToSnoop()) {

		snoop_by->sendInstant(std::string("% ") + command + std::string("\n"));
	}

	char comm[MAX_INPUT_LENGTH+1];//Keeping this way for legacy reasons... Will change someday. 2011-04-10

	strcpy(comm, command.c_str());
			
	if ( character && character->ShieldBlock )
	{
		character->CancelTimer(false);
		return;
	}
	if ( this->character )
	{
		// Reset the idle timer & pull char back from void if necessary

		if ( character->player.timer >= CONFIG_IDLE_VOID ) {

			MudLog( CMP, MAX( GET_LEVEL( character ), LVL_GOD ), TRUE, "%s has un-idled.", GET_NAME( character ) );
		}
		character->player.timer = 0;
		if ( STATE( this ) == CON_PLAYING && this->character->was_in_room )
		{
			if ( this->character->in_room )
				character->RemoveFromRoom();

			character->MoveToRoom( character->was_in_room );
			character->was_in_room = 0;
			Act( "$n has returned.", TRUE, character, 0, 0, TO_ROOM );
		}
		character->wait = 0;
	}

	if ( showstr_count )  // Reading something w/ pager
	{
		show_string( this, comm );
	}
	else if ( str )
	{
		string_add( this, comm );
	}
	else if ( STATE( this ) != CON_PLAYING )  // In menus, etc.
	{
		nanny( comm );
	}

	else if( character && character->editorInterfaceInstance )
	{
		character->editorInterfaceInstance->parse(comm);
	}

	else
	{// else: we're playing normally.
		CommandUtil::get()->interpretCommand( character, comm ); /* Send it to interpreter */
		if ( character && !character->command_ready ) {
			character->CancelTimer( false );
		}
	}
}

//Calling this should disconnect the player from the game, clean up the descriptor,
//and also send a message to the gateway to close the client connection.
void Descriptor::disconnect()
{
	if(gatewayConnection != NULL && !session.empty()) {

		gatewayConnection->send("Close " + session + "\n");
	}

	cleanup();
}

//Calling this should disconnect the player from the game, clean up the descriptor,
//but send a message to the gateway telling it to hold on to the connection.
void Descriptor::persistentDisconnect()
{
	if(gatewayConnection != NULL) {

		gatewayConnection->send("PersistentClose " + session + "\n");
	}

	cleanup();
}

void Descriptor::cleanup()
{
	Descriptor* temp;

	REMOVE_FROM_LIST( this, descriptor_list, next );

	Character *ch;
	if( ((ch = character) || (ch = original)) && loggedIn ) {
		Account account = SwitchManager::GetManager().GetAccountByPlayerID( ch->player.idnum );
		//We do not want to put the mob's idnum or their name in the switch manager if this descriptor is switched.
		SwitchManager::GetManager().AddSwitch( this->host, ch->player.name, time(0), account.IsValid() ? account.GetID() : (-1) );
	}

	/* Forget snooping */
	if ( this->snooping )
		this->snooping->snoop_by = NULL;

	if ( this->snoop_by )
	{
		this->snoop_by->send( "Your snoop target is no longer among us.\r\n" );
		this->snoop_by->snooping = NULL;
	}

	/*. Kill any OLC stuff .*/
	switch ( this->connected )
	{
	case CON_OEDIT:
	case CON_REDIT:
	case CON_ZEDIT:
	case CON_MEDIT:
	case CON_SEDIT:
	case CON_AEDIT:
	case CON_SGEDIT:
	case CON_CEDIT:
	case CON_CLEDIT:
	case CON_KEDIT:
	case CON_WEDIT:
	case CON_AUCTION:
	case CON_JEDIT:
	cleanup_olc( this, CLEANUP_ALL );

	default:
		break;
	}

	if ( this->character )
	{
		/*
		 Plug memory leak, from Eric Green.
		*/

		if ( PLR_FLAGGED( this->character, PLR_MAILING ) && this->str )
		{
			if ( *( this->str ) )
				delete[] ( *( this->str ) );

			delete[] ( this->str );
		}

		if ( STATE( this ) == CON_PLAYING || STATE( this ) == CON_DISCONNECT )
		{
			Act( "$n has lost $s link.", TRUE, this->character, 0, 0, TO_ROOM );
			MudLog( NRM, MAX( LVL_IMMORT,
				GET_INVIS_LEV( this->character ) ), TRUE, "Closing link to: %s.", GET_NAME( this->character ) );
			this->character->desc = NULL;
		}

		else
		{
			MudLog( CMP, MAX( GET_INVIS_LEV( this->character ), LVL_APPR ), TRUE, "Losing player: %s.",
			        !this->character->player.name.empty() ? GET_NAME( this->character ) : "<null>" );
		}
	}

	else
		MudLog( CMP, LVL_IMMORT, TRUE, "Losing descriptor without char." );

	/* JE 2/22/95 -- part of my unending quest to make switch stable */

	if ( this->original && this->original->desc )
		this->original->desc = NULL;

	if ( this->showstr_head )
		delete[] ( this->showstr_head );

	if ( this->showstr_count )
		delete[] ( this->showstr_vector);
	if ( this->character )
		this->character->desc = 0;

	delete ( this );
}

void Descriptor::sendInstant( const std::string &str )
{
	if(snoop_by && snoop_by->descriptor && snoop_by->hasPermissionToSnoop())
	{
		snoop_by->sendInstant("% ");
		snoop_by->sendInstant(str);
	}

	if(this->getGatewayDescriptorType() == GatewayDescriptorType::websocket)
	{
		Character *loggingCharacter = this->character;

		if(this->original)
			loggingCharacter = this->original;
		if(loggingCharacter)
			loggingCharacter->LogOutput(str);

		socketWriteInstant(encodeWebSocketOutputCommand(str.c_str()), false);
	}
	else
		socketWriteInstant(str, true);
}

void Descriptor::socketWriteInstant( const std::string &str, bool recordToUserLog )
{
	this->descriptor->socketWriteInstant( str );

	Character *loggingCharacter = this->character;

	if(this->original)
		loggingCharacter = this->original;

	if(loggingCharacter && recordToUserLog)
	{
		loggingCharacter->LogOutput(str);
	}
}

GatewayDescriptorType *Descriptor::getGatewayDescriptorType() const
{
	return gatewayDescriptorType;
}

void Descriptor::setGatewayDescriptorType(GatewayDescriptorType *gatewayDescriptorType)
{
	this->gatewayDescriptorType = gatewayDescriptorType;
}

void Descriptor::sendWebSocketCommands(const int pulse)
{
	if(pulse % (PASSES_PER_SEC * 1) == 0)
	{
		int playersOnline = 0;

		for(auto descriptor = descriptor_list;descriptor;descriptor = descriptor->next)
		{
			if(descriptor->character)
				++playersOnline;
		}

		for(auto descriptor = descriptor_list;descriptor;descriptor = descriptor->next)
		{
			if(descriptor->getGatewayDescriptorType() != GatewayDescriptorType::websocket)
				continue;
			if(!descriptor->loggedIn)
				continue;

			descriptor->sendWebSocketPlayersOnlineCommand(playersOnline);
		}
	}
}


void Descriptor::sendWebSocketDisplaySignInLightboxMessage()
{
	Json::FastWriter writer;
	Json::Value commandObject;

	commandObject["method"] = "Display Sign In Lightbox";

	sendWebSocketCommand(writer.write(commandObject));
}

void Descriptor::sendWebSocketUsernameCommand(const std::string &username, const std::list<UserMacro *> &userMacros)
{
	Json::Value commandObject;
	Json::FastWriter writer;

	commandObject["method"] = "Username";
	commandObject["username"] = username;
	commandObject["macros"] = Json::Value();

	for(auto userMacroIter = userMacros.begin();userMacroIter != userMacros.end();++userMacroIter)
	{
		Json::Value userMacroObject;
		userMacroObject["keyCode"] = (*userMacroIter)->getKeyCode();
		userMacroObject["replacement"] = (*userMacroIter)->getReplacement();
		userMacroObject["id"] = (*userMacroIter)->getId();
		
		commandObject["macros"].append(userMacroObject);
	}

	sendWebSocketCommand(writer.write(commandObject));
}

void Descriptor::sendWebSocketPlayersOnlineCommand(const int playersOnline)
{
	Json::Value commandObject;
	Json::FastWriter writer;

	commandObject["method"] = "Players Online";
	commandObject["numberOfPlayers"] = playersOnline;

	if(character)
	{
		commandObject["level"] = GET_LEVEL(character);
		commandObject["expToLevel"] = character->ExperienceToLevel();
		commandObject["weavePoints"] = character->points.weave;
		commandObject["hitPoints"] = GET_HIT(character);
		commandObject["maxHitPoints"] = GET_MAX_HIT(character);
		commandObject["spellPoints"] = GET_MANA(character);
		commandObject["maxSpellPoints"] = GET_MAX_MANA(character);
		commandObject["movePoints"] = GET_MOVE(character);
		commandObject["maxMovePoints"] = GET_MAX_MOVE(character);
		commandObject["title"] = GET_TITLE(character);
		commandObject["offensive"] = character->Offense();
		commandObject["dodge"] = character->Dodge();
		commandObject["parry"] = character->Parry();
		commandObject["asbsorb"] = character->Absorb();
	}

	sendWebSocketCommand(writer.write(commandObject));
}

std::string Descriptor::encodeWebSocketOutputCommand(const char *output)
{
	Json::Value commandObject;
	Json::FastWriter writer;

	commandObject["method"] = "Output";
	commandObject["data"] = output;

	return encodeWebSocketCommand(writer.write(commandObject));
}

std::string Descriptor::encodeWebSocketCommand(const std::string &command)
{
	return command + (char)0x06;
}

void Descriptor::sendWebSocketCommand(const std::string &command)
{
		this->socketWriteInstant(encodeWebSocketCommand(command), false);
}

void Descriptor::processWebSocketSaveUserMacroCommand(Json::Value &commandObject)
{
	if(this->character)
	{
		UserMacro *userMacro = NULL;
		unsigned short keyCode;
		std::string replacement;

		if(commandObject["keyCode"].isNull() || !commandObject["keyCode"].isInt())
		{
			MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(character)), TRUE, "%s attempting to save macro with invalid keyCode.", GET_NAME(character));
			return;
		}
		if(commandObject["replacement"].isNull() || !commandObject["replacement"].isString())
		{
			MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(character)), TRUE, "%s attempting to save macro with invalid replacement.", GET_NAME(character));
			return;
		}

		keyCode = (unsigned short)commandObject["keyCode"].asInt();
		replacement = commandObject["replacement"].asString();

		if(!commandObject["id"].isNull() && commandObject["id"].isInt())
		{
			int userMacroId = commandObject["id"].asInt();
			userMacro = CharacterUtil::getUserMacro(gameDatabase, userMacroId);

			if(userMacro == NULL)
			{
				MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(character)), TRUE, "%s attempting to save user macro that does not exist. ID: %d. Key: %d, Replacement: %s", GET_NAME(character), userMacroId, keyCode, StringUtil::vaEscape(replacement).c_str());
				return;
			}

			if(userMacro->getUserId() != character->getUserId())
			{
				MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(character)), TRUE, "%s attempting to save macro that belongs to another user. ID: %d.", GET_NAME(character), userMacroId);
				delete userMacro;
				return;
			}
		}
		else
		{
			CharacterUtil::deleteUserMacro(gameDatabase, character->getUserId(), keyCode);

			userMacro = new UserMacro();
			userMacro->setUserId(character->getUserId());
			userMacro->setCreatedDatetime(DateTime());
		}

		userMacro->setKeyCode(keyCode);
		userMacro->setReplacement(replacement);

		CharacterUtil::putUserMacro(gameDatabase, userMacro);

		Json::Value commandResponse;
		commandResponse["method"] = "Save User Macro";
		commandResponse["wasSuccessful"] = true;
		commandResponse["userMacroId"] = userMacro->getId();
		commandResponse["keyCode"] = userMacro->getKeyCode();
		commandResponse["replacement"] = userMacro->getReplacement();

		Json::FastWriter writer;

		this->sendWebSocketCommand(writer.write(commandResponse));

		delete userMacro;
	}
}

void Descriptor::processWebSocketDeleteUserMacroCommand(Json::Value &commandObject)
{
	if(!character)
		return;

	if(commandObject["keyCode"].isNull() || !commandObject["keyCode"].isInt())
	{
		MudLog(BRF, MAX(LVL_APPR, GET_INVIS_LEV(character)), TRUE, "%s attempting to delete user macro with invalid ID.", GET_NAME(character));
		return;
	}

	unsigned short keyCode = (unsigned short)commandObject["keyCode"].asInt();
	CharacterUtil::deleteUserMacro(gameDatabase, character->getUserId(), keyCode);
}

void Descriptor::processWebSocketSignInCommand(Json::Value &commandObject)
{
	Json::Value response;
	response["method"] = "Sign In";
	Json::FastWriter writer;
	
	if(this->character)
		response["error"] = "You are already signed in.";
	else if(commandObject["username"].isNull())
		response["error"] = "You must enter a username.";
	else if(commandObject["password"].isNull())
		response["error"] = "You must enter a password.";

	if(!response["error"].isNull())
	{
		this->sendWebSocketCommand(writer.write(response));
		return;
	}

	std::string username = commandObject["username"].asString();
	std::string password = commandObject["password"].asString();

	if(!BanManager::GetManager().IsValidName( username ))
	{
		response["error"] = "The username you have entered is invalid.";
		this->sendWebSocketCommand(writer.write(response));
		return;
	}


	if( Conf->play.switch_restriction )
	{
		Switch *sw = NULL;
		if(SwitchManager::GetManager().WillBeMultiplaying( this->host, username ) )
		{
			response["error"] = "You are already signed in to another character.";
			this->sendWebSocketCommand(writer.write(response));
			return;
		}

		if( (sw = SwitchManager::GetManager().GetGreatestSwitch( this->host, username )) != NULL )
		{
			if( !SwitchManager::GetManager().HasWaitedLongEnough(username, host, sw) )
			{
				Time remainder = SwitchManager::GetManager().TimeRemaining(username, host, sw);
				response["error"] = "You must wait " + MiscUtil::toString(remainder.Minutes()) + " seconds and " + MiscUtil::toString(remainder.Seconds()) + " seconds.";
				this->sendWebSocketCommand(writer.write(response));
				return;
			}

			//This player was on the switch list, but has waited the required time.
			//SwitchManager::GetManager().RemoveSwitchByIP( this->host );
		}
	}

	Character *ch = CharacterUtil::loadCharacter(username);

	if(ch == NULL || PLR_FLAGGED(ch, PLR_DELETED))
	{
		if(ch)
			delete ch;
		response["error"] = "There is no character by that username.";
		this->sendWebSocketCommand(writer.write(response));
		return;
	}
		
	if(!ch->passwordMatches(password))
	{
		response["error"] = "The password you entered is incorrect.";
		++ch->PlayerData->bad_pws;
		ch->basicSave();
		delete ch;
		this->sendWebSocketCommand(writer.write(response));
		return;
	}

	if ( BanManager::GetManager().IsBanned( this->host ) == BAN_SELECT && !PLR_FLAGGED( this->character, PLR_SITEOK ) )
	{
		response["error"] = "Sorry, this char has not been cleared for login from your site!";
		STATE( this ) = CON_CLOSE;
		MudLog( NRM, LVL_GOD, TRUE, "Connection attempt for %s denied from %s", GET_NAME( this->character ), this->host );
		this->sendWebSocketCommand(writer.write(response));
		delete ch;
		return;
	}

	int load_result = ch->PlayerData->bad_pws;
	ch->PlayerData->bad_pws = 0;
	this->bad_pws = 0;

	//We need to convert the password entered to MD5.
	if( !ch->PasswordUpdated() )
	{
		ch->player.passwd = str_dup(MD5::getHashFromString(password.c_str()).c_str());
		ch->PasswordUpdated( true );
	}

	if ( GET_LEVEL( ch ) < circle_restrict )
	{
		response["error"] = "The game is temporarily restricted.. try again later.";
		STATE( this ) = CON_CLOSE;
		MudLog( NRM, LVL_GOD, TRUE, "Request for login denied for %s [%s] (wizlock)", GET_NAME( ch ), this->host );
		this->sendWebSocketCommand(writer.write(response));
		delete ch;
		return ;
	}
	

	if(this->getGatewayDescriptorType() == GatewayDescriptorType::websocket)
	{
		std::list<UserMacro*> userMacros = CharacterUtil::getUserMacros(gameDatabase, ch->getUserId());
		this->sendWebSocketUsernameCommand(GET_NAME(ch), userMacros);
		CharacterUtil::freeUserMacros(userMacros);
	}

	this->echoOn();
	this->idle_tics = 0;
	this->character = ch;
	ch->desc = this;

	// check to make sure that no other copies of this player are logged in.
	if ( performDupeCheck( this ) )
	{
		this->sendWebSocketCommand(writer.write(response));
		return;
	}

	MudLog( BRF, MAX( LVL_GRGOD, GET_INVIS_LEV( this->character ) ), TRUE, "%s [%s] has connected.", GET_NAME( this->character ), this->host );

	if ( GET_LEVEL( this->character ) >= LVL_IMMORT )
		this->send( imotd );
	else
		this->send( motd );
	if ( load_result )
	{
		this->send( "\r\n\r\n\007\007\007"
	         "%s%d LOGIN FAILURE%s SINCE LAST SUCCESSFUL LOGIN.%s\r\n",
	         COLOR_RED( this->character, CL_SPARSE ), load_result,
	         ( load_result > 1 ) ? "S" : "", COLOR_NORMAL( this->character, CL_SPARSE ) );
		this->character->PlayerData->bad_pws = 0;
	}
	
	STATE( this ) = CON_MENU;
	this->completeEnterGame();

	Log("Sending response.");
	this->sendWebSocketCommand(writer.write(response));
}

void Descriptor::processWebSocketUserRegistrationDetailsCommand(Json::Value &commandObject)
{
	Json::Value response;

	response["method"] = "User Creation Details";
	response["maxUsernameLength"] = (int)MAX_NAME_LENGTH;
	response["minUsernameLength"] = (int)MIN_NAME_LENGTH;
	response["maxPasswordLength"] = (int)MAX_PWD_LENGTH;
	response["minPasswordLength"] = (int)MIN_PWD_LENGTH;

	Json::FastWriter writer;
	this->sendWebSocketCommand(writer.write(response));
}

bool isGenderOpen(int genderValue);
bool isRaceOpen(int raceValue);
bool isClassOpen(int classValue, int raceValue);

void Descriptor::sendWebSocketErrorMessage(Json::Value &command, const std::string &errorMessage)
{
	command["errors"] = Json::Value(Json::arrayValue);
	command["errors"].append(errorMessage);

	Json::FastWriter writer;
	this->sendWebSocketCommand(writer.write(command));
}

void Descriptor::processWebSocketUserCreationCommand(Json::Value &commandObject)
{
	Json::Value response;
	std::vector<std::string> errors;
	std::string username;
	std::string password;
	std::string emailAddress;
	Switch *sw;
	Character *previousCharacter;

	response["method"] = "User Creation";

	try {
		username = commandObject["username"].asString();
	}
	catch (std::runtime_error e) {
		sendWebSocketErrorMessage(commandObject, "Please enter a username.");
		return;
	}

	try {
		password = commandObject["password"].asString();
	}
	catch (std::runtime_error e) {
		sendWebSocketErrorMessage(commandObject, "Please enter a password.");
		return;
	}

	try {
		emailAddress = commandObject["emailAddress"].asString();
	}
	catch (std::runtime_error e) {
		sendWebSocketErrorMessage(commandObject, "Please enter an email.");
		return;
	}


	if (circle_restrict)
	{
		errors.push_back("Sorry, new characters can't be created at the moment.");
	}

	if (username.length() < MIN_NAME_LENGTH || username.length() > MAX_NAME_LENGTH)
		errors.push_back("Username must be between " + MiscUtil::toString(MIN_NAME_LENGTH) + " and " + MiscUtil::toString(MAX_NAME_LENGTH) + " characters.");
	else if (!CharacterUtil::isValidUserName(username))
		errors.push_back("Usernames must contain only alphabetical characters.");
	else if (!BanManager::GetManager().IsValidName(username) || fill_word(username.c_str()) || reserved_word(username.c_str()))
		errors.push_back("The username you entered is invalid.");
	else if (Conf->play.switch_restriction && SwitchManager::GetManager().WillBeMultiplaying(this->host, username))
		errors.push_back("You are already logged into another character. You must log out before switching.");
	else if (Conf->play.switch_restriction && (sw = SwitchManager::GetManager().GetGreatestSwitch(this->host, username)) != NULL)
	{
		if (!SwitchManager::GetManager().HasWaitedLongEnough(username, host, sw))
		{
			Time Remainder = SwitchManager::GetManager().TimeRemaining(username, host, sw);
			errors.push_back("You must wait " + MiscUtil::toString(((int)Remainder.Minutes())) + " minute" + (Remainder.Minutes() == 1 ? "" : "s") + ", " + MiscUtil::toString((int)Remainder.Seconds() % 60) + " second" + (Remainder.Seconds() % 60 == 1 ? "" : "s") + " before you may log into another character.");
		}
	}
	else if (playerExists(username) && (previousCharacter = CharacterUtil::loadCharacter(username)) != NULL)
	{
		if (!PLR_FLAGGED(previousCharacter, PLR_DELETED))
			errors.push_back("A character by that name already exists.");
		delete previousCharacter;
		previousCharacter = NULL;
	}

	if (password.length() < MIN_PWD_LENGTH || password.length() > MAX_PWD_LENGTH)
		errors.push_back("Password must be between " + MiscUtil::toString(MIN_PWD_LENGTH) + " and " + MiscUtil::toString(MAX_PWD_LENGTH) + " characters.");
	else if (!str_cmp(password, username))
		errors.push_back("Password and username must not be the same.");

	if (!MiscUtil::isValidEmailAddress(emailAddress))
		errors.push_back("The email you entered is invalid.");

	if (commandObject["genderValue"].isNull() || !commandObject["genderValue"].isInt())
		errors.push_back("Please select a gender.");
	else if (!isGenderOpen(commandObject["genderValue"].asInt()))
		errors.push_back("The gender you selected is invalid.");

	if (commandObject["raceValue"].isNull() || !commandObject["raceValue"].isInt())
		errors.push_back("Please select a race.");
	else if (!isRaceOpen(commandObject["raceValue"].asInt()))
		errors.push_back("The race you selected is invalid.");

	if (commandObject["classValue"].isNull() || !commandObject["classValue"].isInt())
		errors.push_back("Please select a class.");
	else if (!isClassOpen(commandObject["classValue"].asInt(), commandObject["raceValue"].asInt()))
		errors.push_back("The class you selected is invalid.");
	
	if (BanManager::GetManager().IsBanned(this->host) >= BAN_NEW)
	{
		MudLog(NRM, LVL_APPR, TRUE, "Request for new char %s denied from [%s] (siteban)", GET_NAME(this->character), this->host);
		errors.push_back("Sorry, new characters are not allowed from your site!");
		STATE(this) = CON_CLOSE;
	}

	if(errors.size() > 0)
	{
		response["errors"] = Json::Value(Json::arrayValue);

		for(auto error : errors)
		{
			response["errors"].append(error);
		}

		Json::FastWriter writer;
		this->sendWebSocketCommand(writer.write(response));
		return;
	}

	this->character = new Character(CharPlayer);
	this->character->desc = this;
	this->character->player.name = StringUtil::cap(StringUtil::allLower(username));
	GET_PASSWD(this->character) = MD5::getHashFromString(password.c_str());
	GET_SEX(this->character) = commandObject["genderValue"].asInt();
	GET_RACE(this->character) = commandObject["raceValue"].asInt();
	GET_CLASS(this->character) = commandObject["classValue"].asInt();
	this->setEmailAddress(emailAddress);
	this->character->PasswordUpdated(true);
	
	MudLog(NRM, MAX(LVL_APPR, GET_INVIS_LEV(this->character)), TRUE, "%s [%s] new player.", GET_NAME(this->character), this->host);

	this->echoOn();
	this->character->Init();
	StatManager::GetManager().RollStats(this->character);
	this->newbieMenuFinish();
	this->completeEnterGame();

	Json::FastWriter writer;
	this->sendWebSocketCommand(writer.write(response));
}

void Descriptor::processWebSocketCommands()
{
	while(true)
	{
		const char *inputDataBuffer = this->descriptor->getInputDataBuffer();
		const char *endOfCommandPointer = strchr(inputDataBuffer, 0x06);

		if(endOfCommandPointer != NULL)
		{
			std::string jsonCommand = std::string(inputDataBuffer, endOfCommandPointer - inputDataBuffer);
			Json::Value commandObject;
			Json::Reader reader;

			if(!reader.parse(jsonCommand, commandObject, false))
			{
				MudLog(BRF, LVL_APPR, TRUE, "Could not process websocket command. Input: %s", StringUtil::vaEscape(jsonCommand).c_str());
				break;
			}

			this->descriptor->eraseInput(0, (endOfCommandPointer - inputDataBuffer) + 1);
			
			std::string method = commandObject["method"].asString();
			if(method == "Input")
			{
				this->commandQueue.push_back(commandObject["data"].asString());
			}
			else if(method == "Save User Macro")
			{
				processWebSocketSaveUserMacroCommand(commandObject);
			}
			else if(method == "Delete User Macro")
			{
				processWebSocketDeleteUserMacroCommand(commandObject);
			}
			else if(method == "Sign In")
			{
				processWebSocketSignInCommand(commandObject);
			}
			else if (method == "User Creation Details")
			{
				processWebSocketUserRegistrationDetailsCommand(commandObject);
			}
			else if (method == "User Creation")
			{
				processWebSocketUserCreationCommand(commandObject);
			}
		}
		else
			break;
	}
}

void Descriptor::completeEnterGame()
{
	loggedIn = true;
	this->character->loadItems();
	Room *loadRoom;

	//We need to update the player's points over the time they were not logged in.
	time_t timeDiff = (time(0) - this->character->points.last_logout);

	//Regen no more than 60 tics.
	for(int tics = MIN(60,(timeDiff / SECS_PER_MUD_HOUR));tics > 0;--tics)
	{
		this->character->PointUpdate(true);
	}

	LAST_LOGON( this->character ) = time( 0 );
	this->character->player.time.logon.setTime(time( 0 ));
	this->send( "\r\n\n%s\r\n", CONFIG_WELC_MESSG );
	this->character->next = character_list;
	character_list = this->character;

	if(GET_LEVEL(this->character) <= 5)
		this->character->PlayerData->wimp_level = GET_MAX_HIT(this->character) / 2;

	if ( this->character->NeedsReset() )
	{
		this->character->ResetAllSkills();
		this->send( "%s%sYour skills have been automatically reset for free as a result of a global reset.%s\r\n",
		    COLOR_RED( this->character, CL_SPARSE ), COLOR_BOLD( this->character, CL_SPARSE ),
			COLOR_NORMAL( this->character, CL_SPARSE ) );
		this->character->reset_time = time( 0 );
	}

	int currentPlayerCount;
	if( (currentPlayerCount = NumPlayers(true,false)) > boot_high )
	{
		UpdateBootHigh( currentPlayerCount );
	}

	if ( !( loadRoom = FindRoomByVnum( this->character->PlayerData->load_room ) ) )
		loadRoom = this->character->StartRoom();
	this->character->MoveToRoom( loadRoom );
	Act( "$n has entered the game.", TRUE, this->character, 0, 0, TO_ROOM );

	MudLog( CMP, MAX( GET_INVIS_LEV( this->character ), LVL_APPR ), TRUE, "%s logging in at room %d.", GET_NAME( this->character ), this->character->PlayerData->load_room );

	this->character->AddLogin(this->host, DateTime(), this->getGatewayDescriptorType());
	Character *mount = NULL;

	if ( this->character->PlayerData->mount_save > 0 )
	{
		if((mount = new Character(this->character->PlayerData->mount_save, VIRTUAL ))->nr == -1)
		{
			delete mount;
			mount = 0;
		}
		else
		{
			char logBuffer[256];
			snprintf(logBuffer, sizeof(logBuffer), "Login mount load for %s(UID %d)", GET_NAME(this->character), this->character->player.idnum);
			mobLoadLogger.logMobLoad(mount->getVnum(), logBuffer);
			mount->MoveToRoom( loadRoom );
			if ( this->character->in_room->getSector() == RoomSector::inside )
				do_follow( mount, ( char* ) GET_NAME( this->character ), 0, 0 );
			else
			{
				MOUNT( this->character ) = mount;
				RIDDEN_BY( mount ) = this->character;
			}
		}
	}

	STATE( this ) = CON_PLAYING;
	if ( !GET_LEVEL( this->character ) )
	{
		this->send( CONFIG_START_MESSG, this );
	}

	look_at_room( this->character, 0 );
	js_enter_game_trigger(character,character);

	//If the user has been registered for over seven days, display a reminder to register their email if they haven't already done so.
	if( (DateTime().getTime() - this->character->player.time.birth.getTime()) / (60 * 60 * 24) >= 7)
	{
		//Check to see that the user has at least one confirmed email address.
		std::list<UserEmailAddress *> userEmailAddresses = CharacterUtil::getUserEmailAddresses(gameDatabase, this->character->getUserId());
		bool hasConfirmedEmail = false;

		for(auto iter = userEmailAddresses.begin();iter != userEmailAddresses.end();++iter)
		{
			UserEmailAddress *userEmailAddress = (*iter);
			if(userEmailAddress->getConfirmed())
			{
				hasConfirmedEmail = true;
				break;
			}
		}

		CharacterUtil::freeUserEmailAddresses(userEmailAddresses);

		if(!hasConfirmedEmail)
		{
			this->send("\r\n%s%s ** You have not registered and confirmed an email address. Please type `email` to do so.\r\n"
					   " ** Registering an email will allow you to retrieve your password if you ever forget it.%s\r\n", COLOR_BOLD(this->character, CL_NORMAL), COLOR_RED(this->character, CL_NORMAL), COLOR_NORMAL(this->character, CL_NORMAL));
		}
	}
}

std::string Descriptor::getEmailAddress()
{
	return emailAddress;
}

void Descriptor::setEmailAddress(const std::string &emailAddress)
{
	this->emailAddress = emailAddress;
}

bool Descriptor::hasPermissionToSnoop()
{
	return (character && character->PermissionToSnoop);
}

// Turn off echoing (specific to telnet client)
void Descriptor::echoOff()
{
	char off_string[] =
	{
		(char)IAC,
		(char)WILL,
		(char)TELOPT_ECHO,
		(char)0,
	};

	this->sendRaw(off_string);
}


// Turn on echoing (specific to telnet client)
void Descriptor::echoOn()
{
	char on_string[] =
	{
		(char)IAC,
		(char)WONT,
		(char)TELOPT_ECHO,
		(char)TELOPT_NAOFFD,
		(char)TELOPT_NAOCRD,
		(char)0,
	};

	sendRaw(on_string);
}

const char *Descriptor::makePrompt()
{
	static char prompt[256];
	Character *victim;
	*prompt = '\0';
	
	// Note, prompt is truncated at MAX_PROMPT_LENGTH chars (structs.h )
	// These two checks were reversed to allow page_string() to work in the online editor.

	if (showstr_count)
		sprintf(prompt, "\r[ Return to continue, (q)uit, (r)efresh, (b)ack, or page number (%d/%d) ]",
		showstr_page, showstr_count);
	else if (str)
		strcpy(prompt, "] ");
	else if (STATE(this) == CON_PLAYING)
	{
		*prompt = '\0';

		if (GET_INVIS_LEV(character))
			sprintf(prompt, "i%d ", GET_INVIS_LEV(character));

		if (PRF_FLAGGED(character, PRF_DISPHP))
			sprintf(prompt + strlen(prompt), "HP:%s ", this->character->Health());

		if (PRF_FLAGGED(character, PRF_DISPMANA) && (IS_CHANNELER(character) || IS_DREADLORD(character) || IS_DREADGUARD(character)))
			sprintf(prompt + strlen(prompt), "SP:%s ", mana(GET_MAX_MANA(character) > 0 ?
			100 * GET_MANA(character) / (GET_MAX_MANA(character)) : 1));

		if (PRF_FLAGGED(character, PRF_DISPMANA) && (IS_FADE(character)))
			sprintf(prompt + strlen(prompt), "SHP:%s ", mana(GET_MAX_SHADOW(character) > 0 ?
			100 * GET_SHADOW(character) / (GET_MAX_SHADOW(character)) : 1));

		if (PRF_FLAGGED(character, PRF_DISPMOVE))
			sprintf(prompt + strlen(prompt), "MV:%s ", moves(GET_MAX_MOVE(character) > 0 ?
			100 * GET_MOVE(character) / (GET_MAX_MOVE(character)) : 1));

		victim = FIGHTING(character);

		if (victim)
			sprintf(prompt + strlen(prompt), "  - %s: %s",
			GET_NAME(victim), victim->Health());


		if (victim && FIGHTING(victim) && FIGHTING(victim) != character)
			sprintf(prompt + strlen(prompt), " --- %s: %s",
			GET_NAME(FIGHTING(victim)), FIGHTING(victim)->Health());

		if (IS_NPC(this->character))
		{
			sprintf(prompt + strlen(prompt), "  -%s%s%s%s", COLOR_BOLD(character, CL_COMPLETE),
				COLOR_GREEN(character, CL_COMPLETE), GET_NAME(this->character), COLOR_NORMAL(character, CL_COMPLETE));
		}

		strcat(prompt, ">\r\n");
	}

	else if (STATE(this) == CON_PLAYING && IS_NPC(character))
		sprintf(prompt, "\n%s> \n", GET_NAME(character));

	return prompt;
}

void Descriptor::sendRaw(const char *outputBuffer)
{
#ifdef WIN32
	va_list args = 0;
#else
	va_list args;
#endif
	if (!outputBuffer) return;
	writeToOutput(false, outputBuffer, args);
}

void Descriptor::send(const char *messg, ...)
{
	if (!messg)
		return;

	va_list args;
	va_start(args, messg);
	writeToOutput(true, messg, args);
	va_end(args);
}

// Add a new string to a player's output queue
void Descriptor::writeToOutput(bool swapArguments, const char *format, va_list args)
{
	unsigned int bufferSize = LARGE_BUFSIZE, bytesWritten;
	char *outputBufferFormatted = new char[bufferSize];
	std::string finalOutput;

	// Galnor 12/21/2009 - Use dynamically allocated buffer and restrict buffer size
	if (swapArguments)
		vsnprintf(outputBufferFormatted, bufferSize - 1, format, args);
	else
		strncpy(outputBufferFormatted, format, bufferSize - 1);

	outputBufferFormatted[bufferSize - 1] = '\0';

	//Strip all straggling carriage returns or invalid characters. Prepend all newlines with carriage returns.
	for (char *bufferCharacter = outputBufferFormatted; *bufferCharacter; ++bufferCharacter)
	{
		if ( (*bufferCharacter) < 0 || (*bufferCharacter) == '\r')
			continue;
		else if (*bufferCharacter == '\n')
			finalOutput += "\r\n";
		else
			finalOutput += (*bufferCharacter);
	}

	delete[] outputBufferFormatted;

	Character *loggerCharacter = this->character;

	if (this->original)
		loggerCharacter = this->original;

	// if we have enough space, just write to buffer and that's it!
	if (this->descriptor->getOutputBufferSize() < LARGE_BUFSIZE)
	{
		appendToOutputBuffer(finalOutput);
		if (loggerCharacter)
			loggerCharacter->LogOutput(finalOutput);
	}
	else
	{
		appendToOutputBuffer("***OVERFLOW***");
		
		if (loggerCharacter)
			loggerCharacter->LogOutput("***OVERFLOW***");
	}
}

std::string Descriptor::getOutputBuffer() const
{
	return outputBuffer;
}

void Descriptor::appendToOutputBuffer(const std::string &str)
{
	outputBuffer.append(str);
}

void Descriptor::appendToOutputBuffer(const char *str)
{
	outputBuffer.append(str);
}

void Descriptor::clearOutputBuffer()
{
	outputBuffer.clear();
}

void Descriptor::flushOutputBuffer()
{
	if(outputBuffer.empty())
		return;
	
	if(getGatewayDescriptorType() == GatewayDescriptorType::websocket)
		descriptor->send(encodeWebSocketOutputCommand(outputBuffer.c_str()));
	else
		descriptor->send(outputBuffer);

	if(snoop_by && snoop_by->descriptor && snoop_by->hasPermissionToSnoop())
		snoop_by->writeToOutput(false, outputBuffer.c_str(), 0);

	clearOutputBuffer();
}

// Descriptor operator<< overloading
Descriptor& Descriptor::operator<< (const std::string &s)
{
	this->send(s.c_str());
	return *this;
}

Descriptor& Descriptor::operator<< (const char * s)
{
	this->send(s);
	return *this;
}

Descriptor& Descriptor::operator<< (const char s)
{
	this->send(ToString(s).c_str());
	return *this;
}

Descriptor& Descriptor::operator<< (const int s)
{
	this->send(ToString(s).c_str());
	return *this;
}
Descriptor& Descriptor::operator<< (const float s)
{
	this->send(ToString(s).c_str());
	return *this;

}

Descriptor& Descriptor::operator<< (const double s)
{
	this->send(ToString(s).c_str());
	return *this;

}

Descriptor& Descriptor::operator<< (const bool s)
{
	this->send(ToString(s).c_str());
	return *this;

}

void Descriptor::newbieMenuFinish()
{
	//this->send( Conf->operation.NEWBIE_MSG );
	if (this->character->GetCon() != this->character->real_abils.con)
	{
		this->character->SetCon(this->character->real_abils.con);
		this->character->ResetHitRolls(true);
		GET_HIT(this->character) = GET_MAX_HIT(this->character);
	}

	SET_BIT_AR(PRF_FLAGS(this->character), PRF_COLOR_2);
	SET_BIT_AR(PRF_FLAGS(this->character), PRF_DISPHP);
	SET_BIT_AR(PRF_FLAGS(this->character), PRF_DISPMOVE);
	SET_BIT_AR(PRF_FLAGS(this->character), PRF_DISPMANA);
	SET_BIT_AR(PRF_FLAGS(this->character), PRF_AUTOEXIT);

	this->character->SetStr(this->character->real_abils.str);
	this->character->SetInt(this->character->real_abils.intel);
	this->character->SetWis(this->character->real_abils.wis);
	this->character->SetDex(this->character->real_abils.dex);
	this->character->restat_time.setTime(time(0));

	if (playerExists(this->character->player.name))
		MySQLDeleteAll(this->character->player.name);

	this->character->mysqlInsertQuery();
	this->character->save();
	this->character->createPlayerIndex();

	//Record the user's email address.
	UserEmailAddress userEmailAddress;
	userEmailAddress.setUserId(this->character->getUserId());
	userEmailAddress.setEmailAddress(this->getEmailAddress());
	userEmailAddress.setConfirmed(false);
	userEmailAddress.setCreatedDatetime(DateTime());

	CharacterUtil::putUserEmailAddress(gameDatabase, &userEmailAddress);
}

bool Descriptor::shouldMakePrompt()
{
	return STATE(this) == CON_PLAYING && (character == nullptr || character->editorInterfaceInstance == nullptr);
}