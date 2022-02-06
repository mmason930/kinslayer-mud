#include "../../conf.h"
#include "../../utils.h"

#include "Social.h"

void Social::Extract()
{
	if ( command )
		delete [] command;
	if ( sort_as )
		delete [] sort_as;
	if ( char_no_arg )
		delete [] char_no_arg;
	if ( others_no_arg )
		delete [] others_no_arg;
	if ( char_found )
		delete [] char_found;
	if ( others_found )
		delete [] others_found;
	if ( vict_found )
		delete [] vict_found;
	if ( char_body_found )
		delete [] char_body_found;
	if ( others_body_found )
		delete [] others_body_found;
	if ( vict_body_found )
		delete [] vict_body_found;
	if ( not_found )
		delete [] not_found;
	if ( char_auto )
		delete [] char_auto;
	if ( others_auto )
		delete [] others_auto;
	if ( char_obj_found )
		delete [] char_obj_found;
	if ( others_obj_found )
		delete [] others_obj_found;
}
Social::Social()
{
	memset( this, 0, sizeof( Social ) );
}
Social::~Social()
{
	Extract();
}

void Social::Copy( Social* Source )
{
	Extract();

	command				= str_dup( Source->command );
	sort_as				= str_dup( Source->sort_as );
	char_no_arg			= str_dup( Source->char_no_arg );
	others_no_arg		= str_dup( Source->others_no_arg );
	char_found			= str_dup( Source->char_found );
	others_found		= str_dup( Source->others_found );
	vict_found			= str_dup( Source->vict_found );
	char_body_found		= str_dup( Source->char_body_found );
	others_body_found	= str_dup( Source->others_body_found );
	vict_body_found		= str_dup( Source->vict_body_found );
	not_found			= str_dup( Source->not_found );
	char_auto			= str_dup( Source->char_auto );
	others_auto			= str_dup( Source->others_auto );
	char_obj_found		= str_dup( Source->char_obj_found );
	others_obj_found	= str_dup( Source->others_obj_found );

	act_nr				= Source->act_nr;
	hide				= Source->hide;
	min_victim_position	= Source->min_victim_position;
	min_level_char		= Source->min_level_char;
	min_char_position	= Source->min_char_position;
}

Social *Social::clone()
{
	Social *social = new Social();
	social->Copy(this);

	return social;
}

Social::Social( Social* Source )
{
	Copy( Source );
}
