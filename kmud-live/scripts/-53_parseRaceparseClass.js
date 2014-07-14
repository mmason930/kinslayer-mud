function parseRace( sRaceName, bAbbrev ) {
	if( bAbbrev == true ) {
		if( strn_cmp(sRaceName,"human",1) )
			return constants.RACE_HUMAN;
		if( strn_cmp(sRaceName,"trolloc",1) )
			return constants.RACE_TROLLOC;
		if( strn_cmp(sRaceName,"seanchan",1) )
			return constants.RACE_SEANCHAN;
		if( strn_cmp(sRaceName,"aiel",1) )
			return constants.RACE_AIEL;
		if( strn_cmp(sRaceName,"other",1) )
			return constants.RACE_OTHER;
		}
	else {
		if( sRaceName == "human" )
			return constants.RACE_HUMAN;
		if( sRaceName == "trolloc" )
			return constants.RACE_TROLLOC;
		if( sRaceName == "seanchan" )
			return constants.RACE_SEANCHAN;
		if( sRaceName == "aiel" )
			return constants.RACE_AIEL;
		if( sRaceName == "other" )
			return constants.RACE_OTHER;
	}
	return null;
}
function parseClass( sClassName, bAbbrev ) {
	if( bAbbrev ) {
		if( strn_cmp(sClassName,"warrior",1) )
			return constants.CLASS_WARRIOR;
		if( strn_cmp(sClassName,"ranger",1) )
			return constants.CLASS_RANGER;
		if( strn_cmp(sClassName,"thief",1) )
			return constants.CLASS_THIEF;
		if( strn_cmp(sClassName,"channeler",1) )
			return constants.CLASS_CHANNELER;
		if( strn_cmp(sClassName,"blademaster",1) )
			return constants.CLASS_BLADEMASTER;
		if( strn_cmp(sClassName,"fade",1) )
			return constants.CLASS_FADE;
		if( strn_cmp(sClassName,"dreadlord",1) )
			return constants.CLASS_DREADLORD;
		if( strn_cmp(sClassName,"dreadguard",1) )
			return constants.CLASS_DREADGUARD;
	}
	else {
		if( sClassName == "warrior" )
			return constants.CLASS_WARRIOR;
		if( sClassName == "ranger" )
			return constants.CLASS_RANGER;
		if( sClassName == "thief" )
			return constants.CLASS_THIEF;
		if( sClassName == "channeler" )
			return constants.CLASS_CHANNELER;
		if( sClassName == "blademaster" )
			return constants.CLASS_BLADEMASTER;
		if( sClassName == "fade" )
			return constants.CLASS_FADE;
		if( sClassName == "dreadlord" )
			return constants.CLASS_DREADLORD;
		if( sClassName == "dreadguard" )
			return constants.CLASS_DREADGUARD;
	}
	return null;
}
