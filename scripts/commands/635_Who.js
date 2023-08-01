let script635 = function(self, actor, here, args, extra) {
	//Alder
	//May 2010
	/*****************************
	 *          [ WHO ]          *
	 *****************************/
	_block;
	let vArgs = getArgList(args);
	let infoMsg = "";
	getCharCols(actor,constants.CL_OFF);
	if ( vArgs.length > 1 ) {
		infoMsg = parseWhoArgs(actor,vArgs);
		if ( !infoMsg ) {
			return;
		}
	}
	let disp = "";	// Who display
	////Set up preference variables\\\\
	let infoColor = grn;
	let legOnline = false;
	let idleSpam = actor.quest("WHO_IDLE_SPAM");
	let titleSpam = actor.quest("WHO_TITLE_SPAM");
	let stitleSpam = actor.quest("WHO_STITLE_SPAM");
	let colorOff = actor.quest("WHO_COLOR");
	let titleColor = actor.quest("WHO_TITLE_COLOR");
	let bolding = actor.quest("WHO_BOLDING");
	let condensed = actor.quest("WHO_CONDENSED");
	let hideImms = actor.quest("WHO_SHOW_IMMS");
	let hideToday = actor.quest("WHO_SHOW_TODAY");
	let hideRecord = actor.quest("WHO_SHOW_RECORD");
	let hideStats = actor.quest("WHO_SHOW_STATS");
	let legPos = actor.quest("WHO_LEG_POS");
	let raceColor = actor.quest("WHO_RACE_COLOR");
	let bShowKey = false;
	if ( colorOff ) {
		blu = "";
		mag = "";
		yel = "";
		grn = "";
		red = "";
		cyn = "";
		bld = "";
		infoColor = "";
	}
	else {
		let arrColors = [grn,blu,cyn,mag,yel,red];
		let vBold = ( actor.quest("WHO_INFO_BOLD") ? bld : "" );
		infoColor = vBold+arrColors[parseInt(actor.quest("WHO_INFO_COLOR"))];
	}
	let nl = "\n";
	if ( condensed ) {
		nl = "";
	}
	let players = getCurrentPlayers();
	let connected = getConnectedPlayers();
	let legends = getLegends();
	let ds = [];		// Dark-side
	let ls = [];		// Light-side
	let ss = [];		// Seanchan-side
	let as = [];		// Aiel-side
	let immortals = []; // Immortals
	for (let _autoKey in players) {
		let player = players[_autoKey];
		if ( !player.isValid ) {
			continue;
		}
		if ( player.level >= 100 && actor.level >= player.invis ) {
			immortals.push(player);
			continue;
		}
		if ( player.level < 100 ) {
			if ( arrContains(legends,player.name) ) {
				legOnline = true;
			}
			else if ( player.race === constants.RACE_TROLLOC ) {
				ds.push(player);
			}
			else if ( player.race === constants.RACE_HUMAN ) {
				ls.push(player);
			}
			else if ( player.race === constants.RACE_SEANCHAN ) {
				ss.push(player);
			}
			else if ( player.race === constants.RACE_AIEL ) {
				as.push(player);
			}
		}
	}
	function makeList(playerArray) {
		let length = playerArray.length;
		for ( let i = 0; i < length; i++ ) {
			let player = playerArray[i];
			if ( playerArray !== immortals && player.level >= 100 )
				continue;
			let sTit = player.getSpecTitle();
			let pref = "";
			let norm = "";
			let suff = "";
			//var cArr = [mag,mag+bld,cyn,cyn+bld,yel,yel+bld,red,red+bld,grn,grn+bld];
			if ( sTit && !stitleSpam ) {
				let clr = nrm;
				let cat = sTit.category;
				if ( strn_cmp(cat,"pk",1) )
					clr = red;
				else if ( strn_cmp(cat,"clans",1) )
					clr = cyn;
				else if ( strn_cmp(cat,"quests",1) )
					clr = grn;
				else if ( strn_cmp(cat,"trades",1) )
					clr = yel;
				else if ( strn_cmp(cat,"silly",1) )
					clr = bld+grn;
				else if ( strn_cmp(cat,"imm",1) )
					clr = bld+cyn;
				else if ( strn_cmp(cat,"arena",1) )
					clr = mag;
				pref = ( sTit.type === "prefix" ? sTit.title+" " : "" );
				norm = ( sTit.type === "normal" ? nrm+" ["+(!titleColor ? clr : "")+sTit.title+nrm+"]" : "" );
				suff = ( sTit.type === "suffix" ? sTit.title : "" );
			}
			let B = "";
			if ( bolding ) {
				B = bld;
			}
			let rcol = "";
			if ( raceColor ) {
				if ( player.level < 100 ) {
					switch(player.race) {
						case constants.RACE_HUMAN:
							rcol = yel;
							break;
						case constants.RACE_TROLLOC:
							rcol = red;
							break;
						case constants.RACE_SEANCHAN:
							rcol = grn;
							break;
						case constants.RACE_AIEL:
							rcol = mag;
							break;
					}
				}
				else {
					rcol = cyn;
				}
			}
			let linkless = "";
			if ( player.linkless ) {
				linkless = nrm+B+" (linkless)"+nrm;
			}
			let idle = "";
			if ( player.idle ) {
				if ( actor.level >= 100 ) {
					if ( !idleSpam ) {
						idle = nrm+B+" (idle "+player.idle+" min)";
					}
					else if ( player.isIdle() ) {
						idle = nrm+B+" (idle "+player.idle+" min)";
					}

				}
				else if ( player.isIdle() && !linkless.length  ) {
					idle = nrm+B+" (idle)";
				}
			}
			let left = "";
			let right = "";
			if ( arrContains(legends,player.name) ) {
				bShowKey = true;
				if ( legPos ) {
					right = nrm+" - Legend "+player.legend;
				}
				else if ( !legPos ) {
					left = infoColor+player.legend+nrm+" ";
				}
			}
			else if ( !legPos && legOnline ) {
				left += "  ";
			}
			// else if ( !legPos && !legOnline )
			// left = (raceColor ? bld+rcol+"*"+nrm : "");
			// if ( legPos )
			// right += (raceColor ? bld+rcol+" *"+nrm : "");
			let wanted = "";
			let aClans = actor.findClans(true);
			for (let _autoKey in aClans) {
				let num = aClans[_autoKey];
				if ( player.wantedBy(num) ) {
					wanted += nrm+B+" ("+clanNumToText(num).toUpperCase()+" WARRANT)"+nrm;
				}
			}
			let invis = "";
			if ( actor.level >= 100 ) {
				if ( player.invis > 0 ) {
					invis = nrm+B+" (i"+player.invis+")"+nrm;
				}
			}
			let pTitle = "";
			if ( !titleSpam ) {
				pTitle = player.title;
				let arrTitle = pTitle.split("");
				let nTitle = [];
				let len = arrTitle.length;
				for ( let k = 0; k < len; k++ ) {
					let ch = arrTitle[k];
					if ( ch === "-" || ch === "[" || ch === "]" ) {
						continue;
					}
					nTitle.push(ch);
				}
				pTitle = " "+nTitle.join("");
			}
			let ylw = (playerArray === immortals && !raceColor ? nrm+yel : "");
			disp += left+ylw+rcol+pref+player.name+pTitle+nrm+norm+right+wanted+invis+idle+linkless+nrm+"\n";
		}
	}
	if ( immortals.length > 0 && !hideImms ) {
		disp += "-------------- Immortals --------------\n"+nl+nrm;
		makeList(immortals);
	}
	disp += "\n--------------  Players  --------------\n"+(infoMsg.length ? infoMsg : nl)+nrm;
	if ( actor.level >= 100 ) {
		makeList(players);
	}
	else if ( actor.race === constants.RACE_HUMAN ) {
		makeList(ls);
	}
	else if ( actor.race === constants.RACE_TROLLOC ) {
		makeList(ds);
	}
	else if ( actor.race === constants.RACE_SEANCHAN ) {
		makeList(ss);
	}
	else if ( actor.race === constants.RACE_AIEL ) {
		makeList(as);
	}
	let lsCount = "";
	let dsCount = "";
	let ssCount = "";
	let asCount = "";
	function getActive(element, index, array) {
		return ( arrContains(connected,element) && !element.isIdle() );
	}
	if ( ls.length ) {
		let plural = "";
		if ( ls.length > 1 ) {
			plural = "s";
		}
		let len = ls.filter(getActive).length;
		if ( len )
			lsCount = " "+infoColor+len+nrm+" human"+plural+nrm;
	}
	if ( ds.length ) {
		let plural = "";
		if ( ds.length > 1 ) {
			plural = "s";
		}
		let len = ds.filter(getActive).length;
		if ( len )
			dsCount = " "+infoColor+len+nrm+" trolloc"+plural+nrm;
	}
	if ( ss.length ) {
		let plural = "";
		if ( ss.length > 1 ) {
			plural = "s";
		}
		let len = ss.filter(getActive).length;
		if ( len )
			ssCount = " "+infoColor+len+nrm+" seanchan"+plural+nrm;
	}
	if ( as.length ) {
		let plural = "";
		if ( ds.length > 1 ) {
			plural = "s";
		}
		let len = as.filter(getActive).length;
		if ( len )
			asCount = " "+infoColor+len+nrm+" aiel"+plural+nrm;
	}
	if ( bShowKey && !actor.quest("WHO_KEY_SHOW") && !legPos ) {
		disp += "\n"+infoColor+"#"+nrm+" Legends Ranking";
	}
	if ( !hideStats ) {
		disp += "\n--------------   Stats   --------------\n"+nl+nrm;
		disp += "Currently active:"+nrm+lsCount+dsCount+ssCount+asCount+".\n";
		if ( getWaitingPlayers("all").length > 0 ) {
			if (getWaitingPlayers("all").length === 1) {
				let isAre = "is";
				let playerS = "player";
			}
			else if (getWaitingPlayers("all").length > 1) {
				let isAre = "are";
				let playerS = "players";
			}
			if (getWaitingPlayers("ai").length > 0)
				let aiHere = " ("+infoColor+getWaitingPlayers("ai").length+nrm+" ai)";
			else
				let aiHere = "";
			disp += "There "+isAre+" currently "+infoColor+getWaitingPlayers("all").length+nrm+" "+playerS+aiHere+" waiting to enter the Arena.\n";
		}
		else if ( global.arenaStage === constants.ARENA_PLAYING) {
			if (global.arenaPlayers.length === 1) {
				let isAre = "is";
				let playerS = "player";
			}
			else if (global.arenaPlayers.length > 1) {
				let isAre = "are";
				let playerS = "players";
			}
			disp += "There "+isAre+" currently "+infoColor+global.arenaPlayers.length+nrm+" "+playerS+" battling in the Arena.\n";
		}
		if ( !hideToday ) {
			let highQuery = sqlQuery("SELECT * FROM player_highs ORDER BY tdate DESC LIMIT 0,1;");
			high = highQuery.getRow.get("tcount");
			disp += "Today there was a high of "+infoColor+high+nrm+" players.\n";
		}
		if ( !hideRecord ) {
			let gHighQuery = sqlQuery("SELECT * FROM player_highs ORDER BY tcount DESC LIMIT 0,1;");
			let gRow = gHighQuery.getRow;
			let gHigh = gRow.get("tcount");
			let hDate = new Date(parseInt(gRow.get("tdate"))*1000);
			hDate = hDate.strftime("%m-%d-%Y");
			let arr = hDate.split("-");
			hDate = arr.join(nrm+"-"+infoColor);
			disp += "The record high of "+infoColor+gHigh+nrm+" players was reached on "+infoColor+hDate+nrm+".";
		}
		if(global.global2014Util){ // global event
			if(global.global2014Util.eventIsActive){
				disp += cyn+bld+"\n\nThe Kinslayer global event is active! Type JOIN EVENT to enter.\nLight Points: "+global.global2014Util.pointMap[constants.RACE_HUMAN]+"  Dark Points: "+global.global2014Util.pointMap[constants.RACE_TROLLOC]+nrm;
			}
		}

		if(global.globalDec2014Util)
		{
			disp += cyn + bld + "\n\nThe Kinslayer global event is active! Current score: " + global.globalDec2014Util.points[constants.RACE_HUMAN] + " LS, " + global.globalDec2014Util.points[constants.RACE_TROLLOC] + " DS." + nrm + "\n";
		}
	}
	else {
		disp += "\n---------------------------------------";
	}
	actor.send(disp);

}