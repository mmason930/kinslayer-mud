
#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "comm.h"
#include "interpreter.h"
#include "db.h"
#include "utils.h"
#include "handler.h"
#include "screen.h"
#include "olc.h"
#include "stats.h"
#include "constants.h"

#include "MiscUtil.h"
#include "Descriptor.h"

extern Descriptor *descriptor_list;

void StateditSetup(Descriptor *d);
void StateditParse(Descriptor *d, const std::string &arg);

void StateditDisplayMainMenu(Descriptor *d);
void StateditDisplayRaceMenu(Descriptor *d);
void StateditDisplayClassMenu(Descriptor *d);
void StateditDisplaySexMenu(Descriptor *d);
void StateditDisplayStatGroupMainMenu(Descriptor *d);
void StateditDisplayStatSelectMenu(Descriptor *d);
void StateditDisplayRollMenu(Descriptor *d);

void StateditParse(Descriptor *d, const std::string &arg)
{
    switch( OLC_MODE(d) )
    {
    case STATEDIT_MAIN_MENU:
        switch( UPPER( arg[0] ) )
        {
        case 'A':
            StateditDisplayStatGroupMainMenu(d);
            break;
        case 'B':
            d->Send("Currently disabled.\r\n");
            StateditDisplayMainMenu(d);
            break;
        case 'Q':
            MudLog(CMP, MAX(LVL_IMPL, GET_INVIS_LEV(d->character)), TRUE,
                "OLC: %s has modified the stat tables.", GET_NAME(d->character));
            cleanup_olc(d, CLEANUP_ALL);

            break;
        default:
            StateditDisplayMainMenu(d);
            d->Send("Invalid option. Try again:\r\n");
            break;
        }
        break;
    case STATEDIT_SG_MAIN_MENU:
        switch( UPPER( arg[0] ) )
        {
        case 'A':
        {
            StateditDisplayRaceMenu( d );
            break;
        }
        case 'B':
        {
            StateditDisplayClassMenu( d );
            break;
        }
        case 'C':
        {
            StateditDisplaySexMenu( d );
            break;
        }
        case 'E':
        {
            //Setup the statgroup that is being edited.
            if( !(d->olc->statgroup = StatManager::GetManager().FindStatGroup(
                d->olc->sg_class, d->olc->sg_race, d->olc->sg_sex)
                 ) )
            {
                StatManager::GetManager().StatGroups.push_back(
                    StatGroup(d->olc->sg_class, d->olc->sg_race, d->olc->sg_sex));
                d->olc->statgroup = &(StatManager::GetManager().StatGroups.back());
            }
            StateditDisplayStatSelectMenu( d );
            break;
        }
        case 'Q':
        {
            //INSERT SAVE/CONFIRMATION CODE HERE.
            StateditDisplayMainMenu( d );
            break;
        }
        default:
            StateditDisplayStatGroupMainMenu( d );
            d->Send("Invalid option. Try again:");
            break;
        }
        break;
    case STATEDIT_RACE:
    {
        if( atoi( arg.c_str() ) <= 0 || atoi( arg.c_str() ) - 1 >= NUM_RACES )
            d->Send("Invalid race. Try again:");
        else
        {
            d->olc->sg_race = atoi( arg.c_str() ) - 1;
            StateditDisplayStatGroupMainMenu( d );
        }
        break;
    }
    case STATEDIT_CLASS:
    {
        if( atoi( arg.c_str() ) <= 0 || atoi( arg.c_str() ) - 1 >= NUM_CLASSES )
            d->Send("Invalid class. Try again:");
        else
        {
            d->olc->sg_class = atoi( arg.c_str() ) - 1;
            StateditDisplayStatGroupMainMenu( d );
        }
        break;
    }
    case STATEDIT_SEX:
    {
        if( atoi( arg.c_str() ) <= 0 || atoi( arg.c_str() ) - 1 >= NUM_GENDERS )
            d->Send("Invalid gender. Try again:");
        else
        {
            d->olc->sg_sex = atoi( arg.c_str() ) - 1;
            StateditDisplayStatGroupMainMenu( d );
        }
        break;
    }
    case STATEDIT_CHOOSE_STAT:
    {
        if(arg[ 0 ] == 'q' || arg[ 0 ] == 'Q')
        {
            StateditDisplayStatGroupMainMenu( d );
        }
        else if(!MiscUtil::isNumber((char*)arg.c_str()) ||
        !(StatManager::GetManager().StatByID( (atoi(arg.c_str())) )))
        {
            d->Send("Invalid stat. Try again:");
        }
        else
        {
//            d->olc->sg_stat = atoi( arg.c_str() );
            d->olc->sgRolls.clear();
            d->olc->sgRolls.insert(d->olc->sgRolls.begin(),
                (d->olc->statgroup->StatRolls[ (int)atoi(arg.c_str()) ]).begin(),
                (d->olc->statgroup->StatRolls[ (int)atoi(arg.c_str()) ]).end()  );

            if( (int) d->olc->sgRolls.size() < 26)
                d->olc->sgRolls.resize( 26 ); //Minimum of size 26.
            d->olc->sg_stat = atoi( arg.c_str() );

            StateditDisplayRollMenu( d );
        }
        break;
    }
    case STATEDIT_CHOOSE_ROLL:
    {
        int roll = atoi( arg.c_str() );

        if(arg[ 0 ] == 'q' || arg[ 0 ] == 'Q')
        {
            d->Send("Save changes to this stat list? ");
            OLC_MODE( d ) = STATEDIT_CONFIRM_STAT_LIST_SAVE;
        }
        else if(MiscUtil::isNumber((char*)arg.c_str()) && roll >= 0 && roll < (int)d->olc->sgRolls.size() )
        {
            d->Send("Input the probablity of rolling this stat(0 - 100) : ");
            d->olc->sg_roll = roll;
            d->olc->sgRolls[ roll ] = 0;
            OLC_MODE( d ) = STATEDIT_CHOOSE_PROBABILITY;
        }
        else
            d->Send("Invalid input. Try again:");
        break;
    }
    case STATEDIT_CONFIRM_STAT_LIST_SAVE:
    {
        switch( UPPER( arg[ 0 ] ) )
        {
        case 'Y':
            d->olc->statgroup->StatRolls[ d->olc->sg_stat ].clear();
            d->olc->statgroup->StatRolls[ d->olc->sg_stat ].insert
            (
                d->olc->statgroup->StatRolls[ d->olc->sg_stat ].begin(),
                d->olc->sgRolls.begin(),
                d->olc->sgRolls.end()
            );
			StatManager::GetManager().Save();
            StateditDisplayStatSelectMenu( d );
            break;
        case 'N':
            d->Send("Not saved.\r\n");
            StateditDisplayStatSelectMenu( d );
            break;
        default:
            d->Send("You must specify [Y]es or [N]o : ");
            break;
        }
        break;
    }
    case STATEDIT_CHOOSE_PROBABILITY:
    {
        int prob = atoi( arg.c_str() );
        if( !MiscUtil::isNumber( (char*)arg.c_str() ) || prob < 0 || prob > 100 )
        {
            d->Send("Must be a number between 0 and 100. Try again:");
        }
        else
        {
            int total = 0;
            for(int i = 0;i < (int)d->olc->sgRolls.size();++i)
                total += d->olc->sgRolls[ i ];
            if( total + prob > 100)
            {
                d->Send("The sum of all rolls cannot be more than 100."
                        "Your input must be between %d & 0 to qualify.\r\n", 100 - total
                       );
            }
            else
            {
                d->olc->sgRolls[ d->olc->sg_roll ] = prob;
                StateditDisplayRollMenu( d );
            }
        }
        break;
    }
    default:
    {
        MudLog(BRF, LVL_IMPL, TRUE,
            "Invalid StatEdit mode(player %s): %d", GET_NAME(d->character), OLC_MODE( d ));
        break;
    }
    }
}

void StateditDisplayRollMenu(Descriptor *d)
{
    int total = 0;

    for(int i = 0;i < (int)d->olc->sgRolls.size();++i)
    {
        total += (int)d->olc->sgRolls[ i ];
    }

    d->Send("Total Used: %s%d%s/100\r\n", (total == 100 ? grn : cyn), total, nrm);
    for(int i = 0;i < (int)d->olc->sgRolls.size();++i)
    {
        d->Send("%s%2d%s) %d%%\r\n", grn, i, nrm,
            (int)(d->olc->sgRolls[ (byte) i ]) );
    }
    d->Send("\r\n%sQ%s) Quit\r\n", grn, nrm);
    d->Send("Choose a roll or option:");
    OLC_MODE( d ) = STATEDIT_CHOOSE_ROLL;
}

void StateditDisplayStatSelectMenu(Descriptor *d)
{
    get_char_cols( d->character );

    for(std::list<Stat>::iterator S = StatManager::GetManager().StatList.begin();
    S != StatManager::GetManager().StatList.end();++S)
    {
        d->Send("%s%2d%s) %s\r\n", grn, (*S).GetID(), nrm, (*S).GetAbbrev().c_str());
    }
    d->Send("%sQ%s) Quit\r\n", grn, nrm);
    d->Send("Choose a stat:");

    OLC_MODE( d ) = STATEDIT_CHOOSE_STAT;
}

void StateditDisplayRaceMenu(Descriptor *d)
{
    get_char_cols( d->character );
    for(int i = 0;*pc_race_types[ i ] != '\n';++i)
    {
        d->Send("%s%2d%s) %s\r\n", grn, i + 1, nrm, pc_race_types[ i ]);
    }
    d->Send("Choose a race:");

    OLC_MODE( d ) = STATEDIT_RACE;
}

void StateditDisplayClassMenu(Descriptor *d)
{
    get_char_cols( d->character );
    for(int i = 0;*pc_class_types[ i ] != '\n';++i)
    {
        d->Send("%s%2d%s) %s\r\n", grn, i + 1, nrm, pc_class_types[ i ]);
    }
    d->Send("Choose a class:");

    OLC_MODE( d ) = STATEDIT_CLASS;
}

void StateditDisplaySexMenu(Descriptor *d)
{
    get_char_cols( d->character );
    for(int i = 0;*sexes[ i ] != '\n';++i)
    {
        d->Send("%s%d%s) %s\r\n", grn, i + 1, nrm, sexes[ i ]);
    }
    d->Send("Choose a gender:");

    OLC_MODE( d ) = STATEDIT_SEX;
}

void StateditDisplayStatGroupMainMenu(Descriptor *d)
{
    get_char_cols( d->character );
    d->Send("%sA%s) Race  : %s%s%s\r\n",
        grn, nrm, cyn, d->olc->sg_race == -1 ?  "None" : pc_race_types[d->olc->sg_race], nrm);
    d->Send("%sB%s) Class : %s%s%s\r\n",
        grn, nrm, cyn, d->olc->sg_class == -1 ?  "None" : pc_class_types[d->olc->sg_class], nrm);
    d->Send("%sC%s) Sex   : %s%s%s\r\n",
        grn, nrm, cyn, d->olc->sg_sex == -1 ?  "None" : sexes[d->olc->sg_sex], nrm);

    d->Send("%sE%s) Edit\r\n"          , grn, nrm                  );
    d->Send("%sQ%s) Quit\r\n"          , grn, nrm                  );

    d->Send("Choose an Option:");
    d->olc->statgroup = 0;
    d->olc->sg_roll = 0;
    OLC_MODE( d ) = STATEDIT_SG_MAIN_MENU;
}

//Display stat-editor main "lobby" menu.
void StateditDisplayMainMenu(Descriptor *d)
{
    get_char_cols(d->character);

    d->Send("%sA%s) Edit a StatGroup\r\n"       , grn, nrm);
    d->Send("%sB%s) Edit Stat Configutation\r\n", grn, nrm);
    d->Send("%sQ%s) Quit\r\n"                   , grn, nrm);
    OLC_MODE(d) = STATEDIT_MAIN_MENU;
}

//Setting up the main stat editor.
void StateditSetup(Descriptor *d)
{
	STATE(d) = CON_SGEDIT;
	StateditDisplayMainMenu(d);
}

ACMD(do_statedit)
{
	Descriptor *d;
	char buf1[MAX_STRING_LENGTH];

	if( GET_LEVEL(ch) < LVL_IMPL && !PLR_FLAGGED(ch, PLR_STAT_EDITOR) )
	{
		ch->Send("You do not have permission to use this command.\r\n");
		return;
	}

	OneArgument(argument, buf1);

	for(d = descriptor_list;d;d = d->next)
	{
		if(STATE(d) == CON_STATEDIT && d->olc && d->olc->statgroup)
		{
			ch->Send("%s is currently editing the stat tables.\r\n",
                d->character ? GET_NAME(d->character) : "Someone");
			return;
		}
	}

	ch->desc->olc = new OLC();
	OLC_ZONE(ch->desc) = 0;
	STATE(ch->desc) = CON_SGEDIT;

    StateditSetup(ch->desc);

	Act("$n begins using OLC.", TRUE, ch, 0, 0, TO_ROOM);
	SET_BITK(PLR_FLAGS(ch), Q_BIT(PLR_WRITING));

	MudLog(BRF, LVL_IMPL, TRUE,  "OLC: %s starts editing the stat tables.", GET_NAME(ch));
}
