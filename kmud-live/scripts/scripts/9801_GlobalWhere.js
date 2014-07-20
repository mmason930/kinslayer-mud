var script9801 = function(self, actor, here, args, extra) {
	_block;
	vArgs = getArgList(args);
	if ( strn_cmp(vArgs[0],"reveal",3) == true ) {																																									//
		if ( !isNumber(vArgs[1]) ) {
			var rNum = here.vnum;
		}
		else {
			rNum = vArgs[1];
		}
		if ( getRoom(rNum) == undefined ) {
			actor.send("No characters are in that room.");
			return;
		}
		var buf = "Characters at '"+capFirstLetter(getRoom(rNum).name)+"' ["+actor.green(constants.CL_OFF)+rNum+actor.normal(constants.CL_OFF)+"]";
		buf += strPadding("\n","-",buf.length-6,"right");
		for (var _autoKey in getRoom(rNum).people) {
			var person = getRoom(rNum).people[_autoKey];
			var clr = "";
			if ( person.vnum == -1 ) {
				if ( person.level >= 100 )
					clr = actor.cyan(constants.CL_OFF);
				else {
					if ( person.race == constants.RACE_TROLLOC )
						clr = actor.magenta(constants.CL_OFF);
					else if ( person.race == constants.RACE_HUMAN )
						clr = actor.yellow(constants.CL_OFF);
				}
			buf += "\n"+clr+capFirstLetter(person.name)+actor.normal(constants.CL_OFF)+" ("+person.level+")";
			}
		}
		actor.send(buf);
	}
	else if ( strn_cmp(vArgs[0],"where",3) == true ) {																																	eval(global.hGCPExecute);
		getCharCols(actor,constants.CL_OFF);
		var buf = "Characters in the game";
		buf += strPadding("\n","-",buf.length+3,"right");
		var people = getCurrentPlayers();
		for (var _autoKey in people) {
			var person = people[_autoKey];
			if ( person.room == undefined )
				continue;
			var clr;
			if( person.level >= 100 && person.invis > actor.level )
				continue;
			if( person.level >= 100 )
				clr = cyn;
			else if( person.race == constants.RACE_TROLLOC )
				clr = mag;
			else if( person.race == constants.RACE_HUMAN )
				clr = yel;
			else
				clr = nrm;
			buf += "\n"+strPadding(clr+capFirstLetter(person.name),"-",15,"right")+capFirstLetter(person.room.name)+nrm+" ["+person.room.vnum+"]";
		}
	/***
		var room;
		for ( var i = 0;(room=getRoomByRnum( i ));++i ) {
			var people = room.people;
			if ( !people ) {
				continue;
			}
			for (var _autoKey in people) {
				var person = people[_autoKey];
				if ( person.vnum == -1 ) {
					if ( person.level >= 100 )
						clr = actor.cyan(constants.CL_OFF);
					else {
						if ( person.race == constants.RACE_TROLLOC )
							clr = actor.magenta(constants.CL_OFF);
						else if ( person.race == constants.RACE_HUMAN )
							clr = actor.yellow(constants.CL_OFF);
					}
					buf += "\n"+strPadding(clr+capFirstLetter(person.name),"-",15,"right")+capFirstLetter(room.name)+nrm+" ["+i+"]";
				}
			}
		}
	***/
		actor.send(buf);
	}
	else
		_noblock;
	
}