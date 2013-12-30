/* ************************************************************************
*   File: act.comm.c                                    Part of CircleMUD *
*  Usage: Player-level communication commands                             *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "spells.h"
#include "utils.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "screen.h"
#include "clans.h"
#include "olc.h"
#include "customcmds.h"

#include "js_functions.h"
#include "StringUtil.h"
#include "MiscUtil.h"
#include "ClanUtil.h"
#include "Descriptor.h"

class Descriptor;

/* extern variables */
extern float SHOUT_HEALTH_PERCENT;
extern Descriptor *descriptor_list;
extern Character *character_list;

/* local functions */
int is_tell_ok(Character *ch, Character *vict);
const char *DARKNESS_CHECK(Character *ch, Character *vict);

ACMD(do_say);
ACMD(do_tell);
ACMD(do_reply);
ACMD(do_spec_comm);
ACMD(do_write);
ACMD(do_page);
ACMD(do_gen_comm);
ACMD(do_speak);
ACMD(do_clan);
ACMD(do_rank);
ACMD(do_warrant);
ACMD(do_declan);
ACMD(do_pardon);

CommManager *CommManager::Self = NULL;
CommManager::CommManager() {}
CommManager::~CommManager() {}

CommManager &CommManager::GetManager()
{
	if( Self == 0 )
		Self = new CommManager();
	return (*Self);
}
void CommManager::Free()
{
	if( Self )
		delete (Self);
	Self = 0;
}
const int CommManager::SaveComm( const std::string &Type, const std::string &Message, Character *sender,
		const int room_vnum, Character *recipient)
{
	return SaveComm(Type, Message, sender->GetType(), sender->getVnumOrID(),
		(recipient ? recipient->GetType() : 'X'), (recipient ? recipient->getVnumOrID() : -1),
		sender->points.invis, room_vnum);
}
const int CommManager::SaveComm( const std::string &Type, const std::string &Message, Character *sender,
	const int room_vnum, const int rid, const int invis_level)
{
	//This is a special case. rid needs to represent something specific.
	return SaveComm(Type, Message, sender->GetType(), sender->getVnumOrID(), 'S', rid, invis_level, room_vnum);
}
const int CommManager::SaveComm( const std::string &Type, const std::string &Message, const char sType, 
	const int sid, const char rType, const int rid, const int invis_level, const int room_vnum)
{
	std::stringstream QueryBuffer;
	QueryBuffer << "INSERT INTO comm (type,message,sender_type,sender_id,recipient_type,recipient_id,room_vnum,timestamp,invis_level) VALUES (";
	QueryBuffer << SQLVal(sql::escapeString(Type));
	QueryBuffer << SQLVal(sql::escapeString(Message));
	QueryBuffer << SQLVal(sType);
	QueryBuffer << SQLVal(sid);
	QueryBuffer << SQLVal(rType);
	QueryBuffer << SQLVal(rid);
	QueryBuffer << SQLVal(room_vnum);
	QueryBuffer << SQLVal(time(0));
	QueryBuffer << SQLVal(invis_level,true);
	QueryBuffer << ");";

	try {
		gameDatabase->sendRawQuery(QueryBuffer.str());
	} catch( sql::QueryException e ) {
		e.report();
		return -1;
	}
	return (int)gameDatabase->lastInsertID();
}

//Returns two character's jointly best understood language skill index.
int Character::BestUnderstoodTongue(Character *target)
{
	int new_tongue		= GET_SKILL(this, SKILL_NEW_TONGUE)   + GET_SKILL(target, SKILL_NEW_TONGUE);
	int old_tongue		= GET_SKILL(this, SKILL_OLD_TONGUE)   + GET_SKILL(target, SKILL_OLD_TONGUE);
	int troll_tongue	= GET_SKILL(this, SKILL_TROLL_TONGUE) + GET_SKILL(target, SKILL_TROLL_TONGUE);

	if(new_tongue >= old_tongue && new_tongue >= troll_tongue)
		return SKILL_NEW_TONGUE;
	if(old_tongue >= new_tongue && new_tongue >= troll_tongue)
		return SKILL_OLD_TONGUE;
	if(troll_tongue >= new_tongue && troll_tongue >= old_tongue)
		return SKILL_TROLL_TONGUE;

	return (MAX(new_tongue, MAX(old_tongue, troll_tongue)));
}

//Returns the skill index for the character's native tongue.
int Character::NativeTongue()
{
	if(IS_HUMAN(this) || IS_AIEL(this) || IS_SEANCHAN(this))
		return SKILL_NEW_TONGUE;
	if(IS_TROLLOC(this))
		return SKILL_TROLL_TONGUE;
	return 0;
}

//Sets a character's default skills, such as languages.
void Character::SetSkillDefaults()
{
	if(	IS_NPC(this) || IS_HUMAN(this) || IS_DREADLORD(this) || IS_DREADGUARD(this) || IS_FADE(this) ||
	IS_GREYMAN(this) || IS_SEANCHAN(this) || IS_AIEL(this))
		SET_SKILL(this, SKILL_NEW_TONGUE, 100);
	if(IS_NPC(this) || IS_TROLLOC(this) || PLR_FLAGGED(this, PLR_DARKFRIEND))
		SET_SKILL(this, SKILL_TROLL_TONGUE, 100);
	SET_SKILL(this, SKILL_BAREHANDS, 100);

	if( !IS_NPC(this) && !this->userClans.empty() )
	{
		Clan *clan;
		for(auto userClanIter = userClans.begin();userClanIter != userClans.end();++userClanIter)
		{
			UserClan *userClan = (*userClanIter);
			clan = ClanUtil::getClan(userClan->getClanId());
			if(userClan->getRank() >= clan->bonus_skill_rank)
				SET_SKILL(this, clan->bonus_skill, clan->bonus_skill_val);
		}
	}
}

//Return true if they can understand eachother 99% of the time or more.
bool Character::FluentWith(Character *receiver)
{
	return (this->CommunicationLevel(receiver) >= 99 ? true : false);
}

//Returns the average of the speaker and listener's speaking capabilities in their jointly best understood tongue.
int Character::CommunicationLevel(Character *receiver)
{
	int skill = this->BestUnderstoodTongue(receiver);
	return (GET_SKILL(this, skill) + GET_SKILL(receiver, skill)) / 2;
}

//Return scrambled speech based on Speaker/Listener's skill in shared tongue
std::string Character::ScrambleSpeech(const std::string &Speech, Character *Listener)
{
	std::string FinalSpeech;
	int SkillLevel;

	if(GET_LEVEL(this) >= LVL_IMMORT || GET_LEVEL(Listener) >= LVL_IMMORT)
		return Speech;

	SkillLevel = this->CommunicationLevel(Listener);

	for(std::string::size_type i = 0;i < Speech.size();++i)
	{
		if(MiscUtil::random(1, 100) > SkillLevel && Speech[i] != ' ')
			FinalSpeech += MiscUtil::random(56, 122);
		else
			FinalSpeech += Speech[i];
	}
	return FinalSpeech;
}

ACMD(do_say)
{
	Character *vict;
	std::string SayType;
	
	if( ch->IsPurged() ) return;

	skip_spaces(&argument);

	if(IS_FADE(ch))
		SayType = "rasps";
	else if(IS_GREYMAN(ch))
		SayType = "breathes";
	else if(IS_OGIER(ch))
		SayType = "rumbles";
	else if(IS_TROLLOC(ch) && !IS_DREADLORD(ch) && !IS_DREADGUARD(ch))
		SayType = "growls";
	else if(IS_SEANCHAN(ch))
		SayType = "drawls";
	else
		SayType = "says";

	if (!*argument)
    {
		ch->send("Yes, but WHAT do you want to say?\r\n");
        return;
    }
	else if(!AFF_FLAGGED(ch, AFF_SILENCE) || GET_LEVEL(ch) >= LVL_IMMORT)
	{
		if( !IS_NPC(ch) )
			CommManager::GetManager().SaveComm("say", argument, ch, ch->in_room->vnum);
		for(vict = ch->in_room->people;vict;vict = vict->next_in_room)
		{
			if(vict != ch && GET_POS(vict) > POS_SLEEPING && (!AFF_FLAGGED(vict, AFF_DEAF) || GET_LEVEL(ch) >= LVL_IMMORT || GET_LEVEL(vict) >= LVL_IMMORT))
			{
				vict->send("%s %s '%s'\r\n", StringUtil::cap(PERS(ch, vict)), SayType.c_str(), ch->ScrambleSpeech(argument, vict).c_str());
			}
			else if(AFF_FLAGGED(vict, AFF_DEAF))
			{
				Act("$n appears to be talking, but you can't make out what $e is saying!", FALSE, ch, NULL, vict, TO_VICT);
			}
		}

		for(int i = 0; i < ch->in_room->eavesdropping.size(); i++)
		{
			vict = ch->in_room->eavesdropping.at(i);
			if(GET_POS(vict) > POS_SLEEPING)
			{
				vict->send("%s %s '%s'\r\n", StringUtil::cap(PERS(ch, vict)), SayType.c_str(), ch->ScrambleSpeech(argument, vict).c_str());
			}
		}

		if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_NOREPEAT))
			ch->send(OK);
		else
		{
			SayType.resize(SayType.size() - 1);
			ch->send("You %s '%s'\r\n", SayType.c_str(), argument);
		}
	}
	else
	{
		ch->send("You try to speak, but nothing comes out!\r\n");
		return;
	}

	// trigger check
    js_speech_triggers(ch, argument);
}

ACMD(do_speak)
{
	Descriptor *pt;
	char arg1[MAX_INPUT_LENGTH], *speech = new char[MAX_INPUT_LENGTH];
	int race;

	if( ch->IsPurged() ) return;

	const char *to[] =
	    {
	        "the Light",
	        "the Dark",
	        "your race",
	        "your race",
	        "your race",
	        "your race"
	    };

	HalfChop(argument, arg1, speech);

	skip_spaces(&speech);
	delete_doubledollar(speech);

	if(!*arg1)
	{
		ch->send("Valid Choices: Light, Dark, All\r\n");
		delete[] speech;
		return;
	}

	std::string spokenTo;
	if(!strn_cmp(arg1, "light", strlen(arg1))) {
		race = RACE_HUMAN;
		spokenTo="light";
	}
	else if(!strn_cmp(arg1, "dark", strlen(arg1))) {
		race = RACE_TROLLOC;
		spokenTo="dark";
	}
	else if(!strn_cmp(arg1, "all", strlen(arg1))) {
		race = -1;
		spokenTo="all";
	}
	else
	{
		ch->send("Valid Choices: Light, Dark, All\r\n");
		delete[] speech;
		return;
	}

	if (!*speech)
		ch->send("You begin to speak, but have nothing to say...\r\n");
	else
	{
		CommManager::GetManager().SaveComm(std::string("speak_") + spokenTo.at(0), speech, ch, ch->in_room->vnum);
		for (pt = descriptor_list; pt; pt = pt->next)
		{
			if(!pt->character || STATE(pt) != CON_PLAYING)
				continue;
			if( PRF_FLAGGED(pt->character, PRF_NOSPEAKS) )
				continue;

			sprintf(buf, "%s%s$n speaks loudly to %s '%s'%s",
			        COLOR_BOLD(ch, CL_SPARSE), COLOR_CYAN(ch, CL_SPARSE),
			        to[(int) GET_RACE(pt->character)] ? to[(int) GET_RACE(pt->character)] : "your race",
			        speech, COLOR_NORMAL(ch, CL_SPARSE));

			if(GET_LEVEL(pt->character) < LVL_IMMORT)
			{
				if (STATE(pt) == CON_PLAYING && pt->character && pt->character != ch &&
				        (GET_RACE(pt->character) == race || race == -1))
				{
					Act(buf, FALSE, ch, 0, pt->character, TO_VICT | TO_SLEEP);
				}
			}

			else
				if(pt->character != ch)
				{
					if(race == -1)
					{
						sprintf(buf, "%s%s$n speaks loudly to all creation '%s'%s",COLOR_BOLD(ch, CL_SPARSE),
						        COLOR_CYAN(ch, CL_SPARSE), speech, COLOR_NORMAL(ch, CL_SPARSE));
						Act(buf, FALSE, ch, 0, pt->character, TO_VICT | TO_SLEEP);
					}

					else
					{

						sprintf(buf, "%s%s$n speaks loudly to %s '%s'%s", COLOR_BOLD(ch, CL_SPARSE),
						        COLOR_CYAN(ch, CL_SPARSE), to[race], speech, COLOR_NORMAL(ch, CL_SPARSE));
						Act(buf, FALSE, ch, 0, pt->character, TO_VICT | TO_SLEEP);
					}
				}

		}

		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			ch->send(OK);
		else
			ch->send("%s%sYou speak loudly to %s '%s'%s\r\n",
			         COLOR_BOLD(ch, CL_SPARSE), COLOR_CYAN(ch, CL_NORMAL), race == -1 ? "all creation" : to[race],
			         speech, COLOR_NORMAL(ch, CL_NORMAL));
	}
	delete[] speech;
}

void Character::NewbieTip(const char *msg, ...)
{
	char TempMessage[MAX_STRING_LENGTH], FinalMessage[MAX_STRING_LENGTH];

	if(!msg)
		return;

	va_list args;
	va_start(args, msg);

	//	vsnprintf(TempMessage, sizeof(FinalMessage), msg, args);
	vsprintf(TempMessage, msg, args);

	sprintf(FinalMessage, "%s%sTip: %s%s\r\n", COLOR_BOLD(this, CL_NORMAL), COLOR_CYAN(this, CL_NORMAL),
	        TempMessage, COLOR_NORMAL(this, CL_NORMAL));
	this->send(FinalMessage);
	va_end(args);
}

void Character::SendTell(Character *target, char *arg)
{
	target->send(COLOR_RED(target, CL_NORMAL));
	sprintf(buf, "$n tells you, '%s'", (char*)this->ScrambleSpeech(arg, target).c_str());
	Act(buf, FALSE, this, 0, target, TO_VICT | TO_SLEEP);
	target->send(COLOR_NORMAL(target, CL_NORMAL));

	if(!IS_NPC(target) && NEWB_FLAGGED(target, NEW_TELL) && !IS_NPC(this))
	{
		REMOVE_BIT_AR(NEWB_FLAGS(target), NEW_TELL);
		target->NewbieTip("You can reply to %s by typing 'reply' followed by the message you wish to send.\r\n",
		                GET_NAME(this), GET_NAME(this));
	}

	if (!IS_NPC(this) && PRF_FLAGGED(this, PRF_NOREPEAT))
		this->send(OK);
	else
	{
		CommManager::GetManager().SaveComm(std::string("tell"), arg, this, this->in_room->vnum, target);
		sprintf(buf, "You tell $N, '%s'", arg);
		Act(buf, FALSE, this, 0, target, TO_CHAR);

		if(IS_NPC(target) && !IS_NPC(this))
			MudLog(CMP, MAX(GET_LEVEL(this), LVL_GOD), TRUE, "%s tells %s '%s' in room %d.",
			       GET_NAME(this), GET_NAME(target), arg, this->in_room->vnum);

		if(this->IsIgnoring(target->player.name))
			this->RemoveIgnore(target->player.name);
	}

	if (!IS_NPC(target) && !IS_NPC(this))
		target->last_tell = this->player.idnum;

	js_tell_triggers(this, target, arg);
}

int is_tell_ok(Character *ch, Character *vict)
{
	if( ch->IsPurged() )
		return FALSE;
	else if( vict->IsPurged() )
		ch->send(NOPERSON);
	else if(GET_RACE(vict) != GET_RACE(ch) && GET_LEVEL(ch) < LVL_IMMORT && GET_LEVEL(vict) < LVL_IMMORT && !IS_NPC(ch))
		ch->send(NOPERSON);
	else if (ch == vict)
		ch->send("You try to tell yourself something.\r\n");
	else if (!IS_NPC(ch) && PRF_FLAGGED(ch, PRF_NOTELL))
		ch->send("You can't tell other people while you have notell on.\r\n");
	else if ( AFF_FLAGGED(ch, AFF_SILENCE) && GET_LEVEL(ch) <= LVL_IMMORT )
		ch->send("You try to speak, but nothing comes out!\r\n");
	/*else if(ch->in_room == ch->StartRoom() && GET_LEVEL(ch) < LVL_IMMORT && GET_LEVEL(vict) < LVL_IMMORT &&
	        GET_LEVEL(ch) > 5)
		ch->send("Your attempt to communicate into the Pattern fails.\r\n");*/
	//	else if(vict->in_room == vict->StartRoom() && GET_LEVEL(vict) < LVL_IMMORT && GET_LEVEL(ch) < LVL_IMMORT)
	//		ch->send("Your attempt to communicate outside of the Pattern fails.\r\n");
	else if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF))
		ch->send("The walls seem to absorb your words.\r\n");
	else if (!IS_NPC(vict) && !vict->desc)        /* linkless */
		Act("$E's linkless at the moment.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	else if (PRF_FLAGGED(ch, PRF_TELL_MUTE))
		ch->send("You are mute to tells. You need an immortal to remove this.\r\n");
	else if (PRF_FLAGGED(vict, PRF_TELL_MUTE))
		Act("$E's is mute to tells... Try again later.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	else if (PLR_FLAGGED(vict, PLR_WRITING))
		Act("$E's writing a message right now; try again later.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	else if ((!IS_NPC(vict) && PRF_FLAGGED(vict, PRF_NOTELL)) || ROOM_FLAGGED(vict->in_room, ROOM_SOUNDPROOF) ||
	         vict->IsIgnoring(GET_NAME(ch)))
		Act("$E can't hear you.", FALSE, ch, 0, vict, TO_CHAR | TO_SLEEP);
	else
		return TRUE;

	return FALSE;
}

/*
 * Yes, do_tell probably could be combined with whisper and ask, but
 * called frequently, and should IMHO be kept as tight as possible.
 */
ACMD(do_tell)
{
	Character *vict;

	if( ch->IsPurged() ) return;

	HalfChop(argument, buf, buf2);
	if (!*buf || !*buf2)
		ch->send("Who do you wish to tell what??\r\n");
	else if (!(vict = get_char_vis(ch, buf)))
		ch->send(NOPERSON);
	else if (is_tell_ok(ch, vict))
		ch->SendTell(vict, buf2);
}

ACMD(do_reply)
{
	Character *tch = character_list;

	if (IS_NPC(ch) || ch->IsPurged())
		return;

	skip_spaces(&argument);

	if (ch->last_tell == NOBODY)
		ch->send("You have no-one to reply to!\r\n");

	else if (!*argument)
		ch->send("What is your reply?\r\n");

	else
	{
		/*
		 * Make sure the person you're replying to is still playing by searching
		 * for them.  Note, now last tell is stored as player IDnum instead of
		 * a pointer, which is much better because it's safer, plus will still
		 * work if someone logs out and back in again.
		 */

		/*
		 * XXX: A descriptor list based search would be faster although
		 *      we could not find link dead people.  Not that they can
		 *      hear tells anyway. :) -gg 2/24/98
		 */

		while (tch != NULL && (IS_NPC(tch) || tch->player.idnum != ch->last_tell))
			tch = tch->next;

		if (tch == NULL)
			ch->send("They are no longer playing.\r\n");

		else if (is_tell_ok(ch, tch))
			ch->SendTell(tch, argument);
	}
}

ACMD(do_spec_comm)
{
	Character *vict;
	const char *action_sing, *action_plur, *action_others;

	if( ch->IsPurged() ) return;

	if (subcmd == SCMD_WHISPER)
	{
		action_sing = "whisper to";
		action_plur = "whispers to";
		action_others = "$n whispers something to $N.";
	}
	else
	{
		action_sing = "ask";
		action_plur = "asks";
		action_others = "$n asks $N a question.";
	}

	HalfChop(argument, buf, buf2);

	if (!*buf || !*buf2)
	{
		ch->send("Whom do you want to %s.. and what??\r\n", action_sing);
	}
	else if (!(vict = get_char_room_vis(ch, buf)))
		ch->send(NOPERSON);
	else if (vict == ch)
		ch->send("You can't get your mouth close enough to your ear...\r\n");
	else
	{
		sprintf(buf, "$n %s you, '%s'", action_plur, ch->ScrambleSpeech(buf2, vict).c_str());
		Act(buf, FALSE, ch, 0, vict, TO_VICT);

		if (PRF_FLAGGED(ch, PRF_NOREPEAT))
			ch->send(OK);
		else
		{
			sprintf(buf, "You %s %s, '%s'\r\n", action_sing, GET_NAME(vict), buf2);
			Act(buf, FALSE, ch, 0, 0, TO_CHAR);

			if(IS_NPC(vict) && !IS_NPC(ch))
				MudLog(CMP, MAX(GET_LEVEL(ch), LVL_GOD), TRUE, "%s %s %s '%s' in room %d.",
				       GET_NAME(ch), action_plur, GET_NAME(vict), buf2, ch->in_room->vnum);

		}
		Act(action_others, FALSE, ch, 0, vict, TO_NOTVICT);
	}
}

const int MAX_NOTE_LENGTH = 1000; // arbitrary

ACMD(do_write)
{
	Object *paper = 0, *pen = 0;
	char *papername, *penname, msg[MAX_STRING_LENGTH], cur[MAX_STRING_LENGTH], waste[MAX_STRING_LENGTH];
	int i = 0, ammount = 0;

	if( ch->IsPurged() ) return;

	papername = buf1;
	penname = buf2;

	HalfChop(argument, papername, argument);
	HalfChop(argument, penname, msg);
	HalfChop(msg, waste, msg);
	//skip_spaces(&msg);

	for(i = 0, ammount = atoi(waste);i < ammount && *waste;++i)
	{
		if(*waste)
			HalfChop(msg, waste, msg);
	}
	if (!*papername)
	{		/* nothing was delivered */
		ch->send("Write?  With what?  ON what?  What are you trying to do?!?\r\n");
		return;
	}

	if (*penname)
	{		/* there were two arguments */
		if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying)))
		{
			ch->send("You have no %s.\r\n", papername);
			return;
		}

		if (!(pen = get_obj_in_list_vis(ch, penname, ch->carrying)))
		{
			ch->send("You have no %s.\r\n", penname);
			return;
		}
	}

	else
	{		/* there was one arg.. let's see what we can find */
		if (!(paper = get_obj_in_list_vis(ch, papername, ch->carrying)))
		{
			ch->send("There is no %s in your inventory.\r\n", papername);
			return;
		}

		if (GET_OBJ_TYPE(paper) == ITEM_PEN)
		{	/* oops, a pen.. */
			pen = paper;
			paper = 0;
		}

		else if (GET_OBJ_TYPE(paper) != ITEM_NOTE)
		{
			ch->send("That thing has nothing to do with writing.\r\n");
			return;
		}

		/* One object was found.. now for the other one. */
		if (!GET_EQ(ch, WEAR_HOLD))
		{
			ch->send("You can't write with %s %s alone.\r\n", AN(papername),
			         papername);
			return;
		}

		if (!CAN_SEE_OBJ(ch, GET_EQ(ch, WEAR_HOLD)))
		{
			ch->send("The stuff in your hand is invisible!  Yeech!!\r\n");
			return;
		}

		if (pen)
			paper = GET_EQ(ch, WEAR_HOLD);

		else
			pen = GET_EQ(ch, WEAR_HOLD);
	}


	/* ok.. now let's see what kind of stuff we've found */
	if (GET_OBJ_TYPE(pen) != ITEM_PEN)
		Act("$p is no good for writing with.", FALSE, ch, pen, 0, TO_CHAR);

	else if (GET_OBJ_TYPE(paper) != ITEM_NOTE)
		Act("You can't write on $p.", FALSE, ch, paper, 0, TO_CHAR);

	else if (paper->action_description && ch->desc && !IS_NPC(ch))
		ch->send("There's something written on it already.\r\n");

	else
	{
		/* we can write - hooray!
		this is the PERFECT code example of how to set up:
		a) the text editor with a message already loaed
		b) the abort buffer if the player aborts the message
		*/



		if (!ch->desc || IS_NPC(ch))
		{
			if(!*msg)
			{
				ch->send("You must write a message right away if you are a mob!");
				return;
			}

			Act("$n writes down a quick note.", TRUE, ch, 0, 0, TO_ROOM);

			if(paper->action_description)
			{
				strcpy(cur, paper->action_description);
				sprintf(cur + strlen(cur), "%s\r\n", msg);
			}

			else
				sprintf(cur, "%s\r\n", msg);

			paper->action_description = new char[MAX_STRING_LENGTH];
			paper->action_description = str_dup(cur);
			return;
		}

		ch->desc->backstr = NULL;
		ch->send("Write your note.  (/s saves /h for help)\r\n");

		/* ok, here we check for a message ALREADY on the paper */

		if (paper->action_description)
		{
			/* we str_dup the original text to the descriptors->backstr */
			ch->desc->backstr = str_dup(paper->action_description);
			/* send to the player what was on the paper (cause this is already */
			/* loaded into the editor) */
			ch->send(paper->action_description);
		}

		Act("$n begins to jot down a note.", TRUE, ch, 0, 0, TO_ROOM);
		/* assign the descriptor's->str the value of the pointer to the text */
		/* pointer so that we can reallocate as needed (hopefully that made */
		/* sense :>) */
		ch->desc->str = &paper->action_description;
		ch->desc->max_str = MAX_NOTE_LENGTH;
	}
}

ACMD(do_page)
{
	Descriptor *d;
	Character *vict;

	if( ch->IsPurged() ) return;

	HalfChop(argument, arg, buf2);

	if (IS_NPC(ch))
		ch->send("Monsters can't page.. go away.\r\n");
	else if (!*arg)
		ch->send("Whom do you wish to page?\r\n");
	else
	{
		sprintf(buf, "\007*%s* %s\r\n", GET_NAME(ch), buf2);

		if (!str_cmp(arg, "all"))
		{
			if (GET_LEVEL(ch) > LVL_GOD)
			{

				for (d = descriptor_list; d; d = d->next)
					if (STATE(d) == CON_PLAYING && d->character)
						Act(buf, FALSE, ch, 0, d->character, TO_VICT);
			}
			else
				ch->send("You will never be godly enough to do that!\r\n");
			return;
		}

		if ((vict = get_char_vis(ch, arg)) != NULL)
		{
			Act(buf, FALSE, ch, 0, vict, TO_VICT);
			if (PRF_FLAGGED(ch, PRF_NOREPEAT))
				ch->send(OK);
			else
				Act(buf, FALSE, ch, 0, vict, TO_CHAR);
			return;
		}
		else
			ch->send("There is no such person in the game!\r\n");
	}
}

/**********************************************************************
 * generalized communication func, originally by Fred C. Merkel (Torg) *
  *********************************************************************/

ACMD(do_gen_comm)
{
	Descriptor *i;
	char color_on[24];
	char name[MAX_INPUT_LENGTH];
	
	if( ch->IsPurged() ) return;

	/* Array of flags which must _not_ be set in order for comm to be heard */
	int channels[] =
	    {
	        //		0,
	        PRF_NOYELL,
	        PRF_NOSHOUT,
	        PRF_NOCHAT,
	        PRF_NONARR,
			PRF_NOGLOBAL,
	        0
	    };

	/*
	com_msgs:	[0] Message if you can't perform the action because of noshout
				[1] name of the action
				[2] message if you're not on the channel
				[3] a color string.
	*/

	const char *com_msgs[][4] =
	    {
	        {"You cannot yell!!\r\n",
	         "yell",
	         "",
			YELLOW
	        },

	        {"You cannot shout!!\r\n",
	         "shout",
	         "Turn off your noshout flag first!\r\n",
			YELLOW},

	        {"You cannot chat!!\r\n",
	         "chat",
	         "You aren't even on the channel!\r\n",
	         YELLOW},

	        {"You cannot narrate!!\r\n",
	         "narrate",
	         "You aren't even on the channel!\r\n",
			YELLOW},

			{"You cannot global!!\r\n",
			 "global",
			 "You aren't even on the channel!\r\n",
			 YELLOW}
	    };

	if(GET_LEVEL(ch) >= LVL_IMMORT)
	{
		ch->send("Use speak!\r\n");
		return;
	}

	if (PLR_FLAGGED(ch, PLR_NOSHOUT))
	{
		ch->send(com_msgs[subcmd][0]);
		return;
	}

	if(PLR_FLAGGED(ch, PLR_NOGLOBAL) && subcmd == SCMD_GLOBAL)
	{
		ch->send("Your global channel privileges have been revoked.\r\n");
		return;
	}

	if (ROOM_FLAGGED(ch->in_room, ROOM_SOUNDPROOF))
	{
		ch->send("The walls seem to absorb your words.\r\n");
		return;
	}

	/* level_can_shout defined in config.c */
	if (GET_LEVEL(ch) < CONFIG_LEVEL_CAN_SHOUT)
	{
		ch->send("You must be at least level %d before you can %s.\r\n",
		         CONFIG_LEVEL_CAN_SHOUT, com_msgs[subcmd][1]);
		return;
	}
	if(ch->in_room == ch->StartRoom() && GET_LEVEL(ch) < LVL_IMMORT)
	{
		ch->send("Your attempt to communicate into the Pattern fails.\r\n");
		return;
	}
	/* make sure the char is on the channel */
	if (PRF_FLAGGED(ch, channels[subcmd]))
	{
		ch->send(com_msgs[subcmd][2]);
		return;
	}

	/* prevent speaking if silenced */
	if (AFF_FLAGGED(ch, AFF_SILENCE) && GET_LEVEL(ch) <= LVL_IMMORT)
	{
		ch->send("You try to speak, but nothing comes out!\r\n");
		return;
	}

	if(subcmd == SCMD_GLOBAL)
	{
		int amountOfNoquit;
		for (affected_type *af = ch->affected; af; af = af->next)
		{
			if(af->bitvector == AFF_NOQUIT && af->duration > 2)
			{
				ch->send("You are too distracted to do that. Kill! Kill! Kill!\r\n");
				return;
			}
		}
	}
	/* skip leading spaces */
	skip_spaces(&argument);

	/* make sure that there is something there to say! */
	if (!*argument)
	{
		ch->send("Yes, %s, fine, %s we must, but WHAT???\r\n",
		         com_msgs[subcmd][1], com_msgs[subcmd][1]);
		return;
	}

	if (subcmd == SCMD_SHOUT && !IS_NPC(ch) && ch->player.idnum != 332)
	{
		if (GET_HIT(ch) <= (GET_MAX_HIT(ch) * SHOUT_HEALTH_PERCENT) )
		{
//			ch->send("You are too exhausted to shout.\r\n");
//			return;
		}
		else
			GET_HIT(ch) = (int)MAX(1, (GET_MAX_HIT(ch) * SHOUT_HEALTH_PERCENT));
	}

	/* set up the color on code */
	strcpy(color_on, com_msgs[subcmd][3]);

	/* first, set up strings to be given to the communicator */
	if (PRF_FLAGGED(ch, PRF_NOREPEAT))
		ch->send(OK);
	else
	{
		if (COLOR_LEV(ch) >= CL_COMPLETE)
			sprintf(buf1, "%sYou %s, '%s'%s", color_on, com_msgs[subcmd][1],
			        argument, NORMAL);
		else
			sprintf(buf1, "You %s, '%s'", com_msgs[subcmd][1], argument);
		Act(buf1, FALSE, ch, 0, 0, TO_CHAR | TO_SLEEP);
		CommManager::GetManager().SaveComm(com_msgs[subcmd][1], argument, ch, ch->in_room->vnum);
	}

	/* now send all the strings out */
	for (i = descriptor_list; i; i = i->next)
	{
		if(i->character && i->character->IsIgnoring(GET_NAME(ch)))
			continue;

		if (STATE(i) == CON_PLAYING && i != ch->desc && i->character &&
		        !PRF_FLAGGED(i->character, channels[subcmd]) &&
		        !PLR_FLAGGED(i->character, PLR_WRITING) &&
		        !ROOM_FLAGGED(i->character->in_room, ROOM_SOUNDPROOF) && (GET_POS(i->character) > POS_SLEEPING))

		{
			/* To prevent crossrace communication */
			if(GET_LEVEL(ch) < LVL_IMMORT && GET_LEVEL(i->character) < LVL_IMMORT)
			{
				if(subcmd == SCMD_NARRATE && GET_RACE(i->character) != GET_RACE(ch))
					continue;

				if(subcmd == SCMD_CHAT && GET_RACE(i->character) != GET_RACE(ch))
					continue;
			}
			
			/* Prevent a deafened mortal from hearing */
			if (AFF_FLAGGED(i->character, AFF_DEAF) && GET_LEVEL(i->character) <= LVL_IMMORT)
				continue;

			if(subcmd == SCMD_GLOBAL)
			{
				strcpy(name, GET_NAME(ch));
			}
			else
			{
				strcpy(name, DARKNESS_CHECK(i->character, ch));

				if((IS_DARK(i->character->in_room) || IS_DARK(ch->in_room)) && !CAN_SEE_IN_DARK(i->character))
					strcpy(name, "Someone");
			}

			std::string speech = argument;

			if(subcmd != SCMD_GLOBAL)
				speech = ch->ScrambleSpeech(argument, i->character);

			if(GET_LEVEL(i->character) >= LVL_IMMORT && (GCOMM_LEV(i->character) == GCOMM_FLAGS || GCOMM_LEV(i->character) == GCOMM_ALL))
			{
				std::string speech = argument;
				// append (DS) if gcomm pref = flags or all & race = ds
				if (GET_RACE(ch) == RACE_TROLLOC) {
					sprintf(buf, "(DS) %s %ss, '%s'", name, com_msgs[subcmd][1], speech.c_str());
				}
				// append (LS) if gcomm pref = flags or all & race = ls
				else {
					sprintf(buf, "(LS) %s %ss, '%s'", name, com_msgs[subcmd][1], speech.c_str());
				}
			}
			// don't append anything since player != Immortal
			else {
				sprintf(buf, "%s %ss, '%s'", name, com_msgs[subcmd][1], speech.c_str());
			}

			if (subcmd == SCMD_YELL &&
			        ((ch->in_room->zone != i->character->in_room->zone) ||
			         GET_POS(i->character) < POS_RESTING))
				continue;

			// Change global comm colors (for Imms) dependant on race
			char commColor[300];
			if (GET_LEVEL(i->character) >= LVL_IMMORT) {
				if (GET_RACE(ch) == RACE_TROLLOC && (GCOMM_LEV(i->character) == GCOMM_COLORS || 
					GCOMM_LEV(i->character) == GCOMM_ALL)) {
					// set trolloc race to magenta if gcomm pref = colors or all
					strcpy(commColor,MAGENTA);
				}
				else {
					// every other race (currently only human, set to yellow)
					strcpy(commColor,YELLOW);
				}
			}
			else {
				// if (recipient) character is not Imm level, use regular (yellow) color
				strcpy(commColor,YELLOW);
			}

			if (COLOR_LEV(i->character) >= CL_NORMAL)
			//	i->send(color_on); // original code
				i->send(commColor); // new code for colors

			Act(buf, FALSE, ch, 0, i->character, TO_VICT | TO_SLEEP);

			if (COLOR_LEV(i->character) >= CL_NORMAL)
				i->send(NORMAL);
		}
	}
}
