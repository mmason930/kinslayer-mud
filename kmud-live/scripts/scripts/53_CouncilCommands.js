var script53 = function(self, actor, here, args, extra) {
	//Alder
	//May 2010
	//global.alder.send(self);
	if ( actor.vnum != -1 ) {
		_noblock;
		return;
	}
	var vArgs = getArgList(args);
	if ( vArgs.length != 3 ) {
		_noblock;
		return;
	}
	// Variables to determine what the command is
	var cmdClan = strn_cmp(vArgs[0],"clan",2);
	var cmdDeclan = strn_cmp(vArgs[0],"declan",3);
	var cmdCouncil = strn_cmp(vArgs[0],"council",4);
	var cmdWarrant = strn_cmp(vArgs[0],"warrant",3);
	var cmdPardon = strn_cmp(vArgs[0],"pardon",3);
	var cmdRank = strn_cmp(vArgs[0],"rank",3);
	if ( (!cmdClan && !cmdDeclan && !cmdCouncil && !cmdPardon && !cmdWarrant && !cmdRank) ) {
		_noblock;
		return;
	}
	_block;
	// if ( actor.level < 103 ) {
		// actor.send("You can't use that command.");
		// return;
	// }
	// actor.send("You are a 103+ immortal, intercepting clan/declan/council command to integrate achievements/titles.");
	var cPlayers = getCurrentPlayers();
	var target = false;
	var name = vArgs[1];
	// Find target in game
	for each ( var player in cPlayers ) {
		if ( !str_cmp(player.name,name) && player.invis <= actor.level ) {
			target = player;
		}
	}
	// If target is invis above actor's level don't execute
	if ( target.invis > actor.level )
		target = false;
	var tClan = vArgs[2];
	var actorClans = actor.findClans(true);
	var vMax = constants.CLANS_MAX;
	var clanNames = [];
	for ( var i = 1; i <= vMax; i++ ) {
		clanNames.push(clanNumToText(i));
	}
	// Determine clan number
	if ( !isNumber(tClan) ) {
		var len = tClan.length;
		for ( var i = 0; i < clanNames.length; i++ ) {
			if ( strn_cmp(tClan,clanNames[i],len) ) {
				tClan = i+1;
				break;
			}
		}
	}
	// Bad clan number
	if ( !isNumber(tClan) || tClan < 1 || tClan > 30 ) {
		actor.send("Invalid clan.");
		return;
	}
	// Not authorized to use these commands
	if ( (!actor.isCouncil(tClan) || !arrContains(actorClans,tClan)) && actor.level < 103 ) {
		actor.send("You are not a member of that clan's council.");
		return;
	}
	// Target is not online and command is not clan/declan
	if ( !target && ( cmdCouncil || cmdPardon || cmdWarrant || cmdRank ) ) {
		actor.send("That person is not online right now.");
		return;
	}
	// Target is actor and mortal: don't execute
	if ( target == actor && actor.level < 103 ) {
		actor.send("You cannot do that to yourself.");
		return;
	}
	// Actor is not an imm and target is of opposite race
	if ( actor.race != target.race && actor.level < 100 ) {
		actor.send("You cannot do this to that person.");
		return;
	}
	if ( target )
		getCharCols(target);
	var cName = clanNumToText(tClan);
	// Command: clan
	if ( cmdClan ) {
		if ( target ) {
			if ( ( actor.level < 103 && target.level > 100 ) ) {
				actor.send("You cannot do this to that person.");
				return;
			}
			if ( target.clanCheck(tClan) ) {
				actor.send(target.name+" is already in that clan.");
				return;
			}
			actor.send("You clan "+capFirstLetter(target.name)+" into the "+bld+cName+nrm+".");
			target.clan(tClan);
				target.setPval("CLAN_SCRIPT_NAME_SAVE",cName);
			target.unlockAchievement(53,true);
			target.comm("save");
		}
		else if ( !target ) {
			var rs = sqlQuery("SELECT userClan.* FROM userClan LEFT JOIN users ON userClan.user_id=users.user_id WHERE users.username='"+sqlEsc(name)+"' AND userClan.clan_id="+sqlEsc(tClan)+";");
			if ( rs.hasNextRow ) {
				actor.send(capFirstLetter(name)+" is already in that clan.");
				return;
			}
			actor.send("You clan "+capFirstLetter(name)+" into the "+bld+cName+nrm+" (Offline).");
			clanOffline(name,tClan);
		}
		return;
	}
	// Command: declan (imm only)
	if ( cmdDeclan ) {
		if ( actor.level < 103 ) {
			actor.send("This ability is currently unavailable to you.");
			return;
		}
		if ( target ) {
			if ( actor.level < 103 && target.level > 100 ) {
				actor.send("You cannot do this to that person.");
				return;
			}
			if ( !target.clanCheck(tClan) ) {
				actor.send(target.name+" is not even in that clan.");
				return;
			}
			if ( target.isCouncil(tClan) ) {
				getMobAtRoom(84,84).comm("council "+target.name+" "+tClan);
				target.send("You have been removed from the "+bld+cName+nrm+" Council.");
				target.lockTitle(cName+" Council","normal",54);
			}
			target.lockTitle(cName,"normal",53);
			actor.send("You remove "+capFirstLetter(target.name)+" from the "+bld+cName+nrm+".");
			target.declan(tClan);
			target.comm("save");
		}
		else if ( !target ) {
			var rs = sqlQuery("SELECT userClan.* FROM userClan LEFT JOIN users ON userClan.user_id=users.user_id WHERE users.username='"+sqlEsc(name)+"' AND userClan.clan_id="+sqlEsc(tClan)+";");
			if ( !rs.hasNextRow ) {
				actor.send(capFirstLetter(name)+" is not even in that clan.");
				return;
			}
			actor.send("You declan "+capFirstLetter(name)+" from the "+bld+cName+nrm+" (Offline).");
			declanOffline(name,tClan);
			var qBuffer = "SELECT user_id FROM users WHERE username LIKE '" + name + "';";
			var query = sqlQuery(qBuffer);
			var id = sqlEsc(query.getRow.get("user_id"));
			sqlQuery("DELETE FROM userTitle WHERE user_id='"+sqlEsc(id)+"' && title LIKE '"+sqlEsc(cName)+"%';");
		}
		return;
	}
	// Command: council (imm only)
	if ( cmdCouncil ) {
		if ( actor.level < 103 ) {
			actor.send("You do not have that ability.");
			return;
		}
		if ( !target.clanCheck(tClan) ) {
			actor.send(target.name+" is not even in that clan.");
			return;
		}
		if ( target.isCouncil(tClan) ) {
			actor.send(target.name+" has been removed from the "+bld+cName+nrm+" Council.");
			target.send("You have been removed from the "+bld+cName+nrm+" Council.");
			target.lockTitle(cName+" Council","normal",54);
		}
		if ( !target.isCouncil(tClan) ) {
			actor.send(target.name+" has been added to the "+bld+cName+nrm+" Council.");
			target.send("You have been inducted into the "+bld+cName+nrm+" Council.");
			target.setPval("CLAN_SCRIPT_NAME_SAVE",cName+" Council");
			target.unlockAchievement(54,true);
		}
		getMobAtRoom(84,84).comm("council "+target.name+" "+tClan);
		target.comm("save");
		return;
	}
	// Command: warrant
	if ( cmdWarrant ) {
		if ( ( actor.level < 103 && target.level > 99 ) || ( actor.race != target.race ) ) {
			actor.send("You cannot do this to that person.");
			return;
		}
		if ( target.clanCheck(tClan) && actor.level < 103 ) {
			actor.send("You cannot warrant someone by their own clan.");
			return;
		}
		target.warrantByClan(tClan);
		target.send("You have been warranted by the "+bld+cName+nrm+".");
		actor.send(target.name+" has been warranted by the "+cName+".");
		mudLog(3, Math.max(actor.invis, 103), actor.name+" warranted "+target.name+" for "+cName+".");
		return;
	}
	// Command: pardon
	if ( cmdPardon ) {
		// if ( ( actor.level < 103 && target.level > 100 ) || ( actor.race != target.race ) ) {
			// actor.send("You cannot do this to that person.");
			// return;
		// }
		target.pardonByClan(tClan);
		target.send("You have been pardoned by the "+bld+cName+nrm+".");
		actor.send(target.name+" has been pardoned by the "+bld+cName+nrm+".");
		mudLog(3, Math.max(actor.invis, 103), actor.name+" pardoned "+target.name+" for "+cName+".");
		return;
	}
	// Command: rank
	if ( cmdRank ) {
		if ( !target.clanCheck(tClan) ) {
			actor.send(target.name+" is not even in that clan.");
			return;
		}
		var rank = target.getRank(tClan);
		if ( target.qp(tClan) < getRankRequirement(rank+1) ) {
			actor.send(target.name+" does not have enough quest points to advance.");
			return;
		}
		getPermaMob().comm("rank "+target.name+" "+tClan);
		var rankName = getClanRankName(tClan, rank+1);
		var vowels = "aeiou";
		var a_an = (vowels.search(rankName[0].toLowerCase()) != -1 ? "an" : "a");
		// actor.send(target.name+" has been promoted to a "+bld+rankName+nrm+".");
		target.send("You have been promoted to "+a_an+" "+bld+rankName+nrm+".");
		// Echo message about person's rank to the game if clan is not Black Tower or Wolfbrother
		if ( tClan != 15 && tClan != 13) {
			function gechoMsg(player) {
				getCharCols(player);
				return (bld+grn+"**"+target.name+" has been promoted to "+a_an+" "+rankName+"!**"+nrm);
			}
			gecho(gechoMsg);
		}
		else {
			actor.send(bld+target.name+" has been promoted to "+a_an+" "+rankName+".");
		}
		// Inform about rank 6 skill
		if ( rank == 6 ) {
			target.send("You are now able to select a special skill at your clan leader.\nType "+cyn+"show skills"+nrm+" at your clan leader to view the skills available.");
		}
		target.comm("save");
		mudLog(3, Math.max(actor.invis, 103), target.name+" promoted to rank "+(rank+1)+" by "+actor.name+" for "+cName+".");
		return;
	}
	
	
	
	
}