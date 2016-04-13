/*Rhollor Dec 2009*/
/*Modified by Galnor on 12/28/2009 - Adding cross-race alias targets*/
function xRaceAlias( ch, vict, sAlias )
{
	if( vict.vnum != -1 || ch.race == vict.race || vict.level >= 100 || ch.level >= 100 )
		return false;
	sAlias = sAlias.toLowerCase();
	if( vict.race == constants.RACE_HUMAN && sAlias == "human" )
		return true;
	if( vict.race == constants.RACE_TROLLOC )
	{
		if( sAlias == "dark" ) return true;
		if( vict.class == constants.CLASS_GREYMAN ) return true;
		if( vict.class == constants.CLASS_FADE ) return true;
		if( vict.class == constants.CLASS_DREADGUARD ) return true;
		if( vict.class == constants.CLASS_DREADLORD ) return true;
		if( sAlias == "trolloc" ) return true;
	}
	if( vict.race == constants.RACE_SEANCHAN && sAlias == "seanchan" )
		return true;
	if( vict.race == constants.RACE_AIEL && sAlias == "aiel" )
		return true;
	return false;
}
function getCharInListVis(ch, charName, list, nr)
{
    if( list.length == 0 )
        return null;
    if( !nr ) {
        var nameList = charName.split(".");
        nr = new Object();
        nr.val = 1;
        if( nameList.length == 2 ) {//3.man, 2.horse, 5.rhollor, etc...
            nr.val = nameList[0];
            charName = nameList[1];
            if( nr.val <= 0 )
                return null;
        }
    }
    var type = 2;//Chars
    if( getObjectClass(list[0]) == "Array" )
        type = 3;//List of chars...
    for(var i = 0;i < list.length;++i)
    {
        if( type == 2 ) {
			if( ch && !ch.canSee(list[i]) )
				continue;
			if( !isName(charName, list[i].namelist) && (!ch || !xRaceAlias( ch, list[i], charName )) )
				continue;
			if( --nr.val > 0 )
				continue;
			return list[i];
        }
        else {
            var mob = getCharInListVis(ch, charName, list[i], nr);
            if( mob )
                return mob;
        }
    }
    return null;
}
