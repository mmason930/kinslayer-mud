var script635 = function(self, actor, here, args, extra) {
	//Alder
	//May 2010
	/*****************************
	 *          [ WHO ]          *
	 *****************************/
		_block;
		var vArgs = getArgList(args);
		var infoMsg = "";
		getCharCols(actor,constants.CL_OFF);
		if ( vArgs.length > 1 ) {
			infoMsg = parseWhoArgs(actor,vArgs);
			if ( !infoMsg ) {
				return;
			}
		}
		var disp = "";	// Who display
		////Set up preference variables\\\\
		var infoColor = grn;
		var legOnline = false;
		var idleSpam = actor.quest("WHO_IDLE_SPAM");
		var titleSpam = actor.quest("WHO_TITLE_SPAM");
		var stitleSpam = actor.quest("WHO_STITLE_SPAM");
		var colorOff = actor.quest("WHO_COLOR");
		var titleColor = actor.quest("WHO_TITLE_COLOR");
		var bolding = actor.quest("WHO_BOLDING");
		var condensed = actor.quest("WHO_CONDENSED");
		var hideImms = actor.quest("WHO_SHOW_IMMS");
		var hideToday = actor.quest("WHO_SHOW_TODAY");
		var hideRecord = actor.quest("WHO_SHOW_RECORD");
		var hideStats = actor.quest("WHO_SHOW_STATS");
		var legPos = actor.quest("WHO_LEG_POS");
		var raceColor = actor.quest("WHO_RACE_COLOR");
		var bShowKey = false;
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
			var arrColors = [grn,blu,cyn,mag,yel,red];
			var vBold = ( actor.quest("WHO_INFO_BOLD") ? bld : "" );
			infoColor = vBold+arrColors[parseInt(actor.quest("WHO_INFO_COLOR"))];
		}
		var nl = "\n";
		if ( condensed ) {
			nl = "";
		}
		var players = getCurrentPlayers();
		var connected = getConnectedPlayers();
		var legends = getLegends();
		var ds = [];		// Dark-side
		var ls = [];		// Light-side
		var ss = [];		// Seanchan-side
		var as = [];		// Aiel-side
		var immortals = []; // Immortals
		for (var _autoKey in players) {
			var player = players[_autoKey];
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
				if ( player.race == constants.RACE_TROLLOC ) {
					ds.push(player);
					continue;
				}
				if ( player.race == constants.RACE_HUMAN ) {
					ls.push(player);
					continue;
				}
				if ( player.race == constants.RACE_SEANCHAN ) {
					ss.push(player);
					continue;
				}
				if ( player.race == constants.RACE_AIEL ) {
					as.push(player);
					continue;
				}
			}
		}
		function makeList(playerArray) {
			var length = playerArray.length;
			for ( var i = 0; i < length; i++ ) {
				var player = playerArray[i];
				if ( playerArray != immortals && player.level >= 100 )
					continue;
				var sTit = player.getSpecTitle();
				var pref = "";
				var norm = "";
				var suff = "";
				//var cArr = [mag,mag+bld,cyn,cyn+bld,yel,yel+bld,red,red+bld,grn,grn+bld];
				if ( sTit && !stitleSpam ) {
					var clr = nrm;
					var cat = sTit.category;
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
					pref = ( sTit.type == "prefix" ? sTit.title+" " : "" );
					norm = ( sTit.type == "normal" ? nrm+" ["+(!titleColor ? clr : "")+sTit.title+nrm+"]" : "" );
					suff = ( sTit.type == "suffix" ? sTit.title : "" );
				}
				var B = "";
				if ( bolding ) {
					B = bld;
				}
				var rcol = "";
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
				var linkless = "";
				if ( player.linkless ) {
					linkless = nrm+B+" (linkless)"+nrm;
				}
				var idle = "";
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
				var left = "";
				var right = "";
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
				var wanted = "";
				var aClans = actor.findClans(true);
				for (var _autoKey in aClans) {
					var num = aClans[_autoKey];
					if ( player.wantedBy(num) ) {
						wanted += nrm+B+" ("+clanNumToText(num).toUpperCase()+" WARRANT)"+nrm;
					}
				}
				var invis = "";
				if ( actor.level >= 100 ) {
					if ( player.invis > 0 ) {
						invis = nrm+B+" (i"+player.invis+")"+nrm;
					}
				}
				var pTitle = "";
				if ( !titleSpam ) {
					pTitle = player.title;
					var arrTitle = pTitle.split("");
					var nTitle = [];
					var len = arrTitle.length;
					for ( var k = 0; k < len; k++ ) {
						var ch = arrTitle[k];
						if ( ch == "-" || ch == "[" || ch == "]" ) {
							continue;
						}
						nTitle.push(ch);
					}
					pTitle = " "+nTitle.join("");
				}
				var ylw = (playerArray == immortals && !raceColor ? nrm+yel : "");
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
		else if ( actor.race == constants.RACE_HUMAN ) {
			makeList(ls);
		}
		else if ( actor.race == constants.RACE_TROLLOC ) {
			makeList(ds);
		}
		else if ( actor.race == constants.RACE_SEANCHAN ) {
			makeList(ss);
		}			
		else if ( actor.race == constants.RACE_AIEL ) {
			makeList(as);
		}
		var lsCount = "";
		var dsCount = "";
		var ssCount = "";
		var asCount = "";
		function getActive(element, index, array) {
			return ( arrContains(connected,element) && !element.isIdle() );
		}	
		if ( ls.length ) {
			var plural = "";
			if ( ls.length > 1 ) {
				plural = "s";
			}
			var len = ls.filter(getActive).length;
			if ( len )
				lsCount = " "+infoColor+len+nrm+" human"+plural+nrm;
		}
		if ( ds.length ) {
			var plural = "";
			if ( ds.length > 1 ) {
				plural = "s";
			}
			var len = ds.filter(getActive).length;
			if ( len )
				dsCount = " "+infoColor+len+nrm+" trolloc"+plural+nrm;
		}
		if ( ss.length ) {
			var plural = "";
			if ( ss.length > 1 ) {
				plural = "s";
			}
			var len = ss.filter(getActive).length;
			if ( len )
				ssCount = " "+infoColor+len+nrm+" seanchan"+plural+nrm;
		}
		if ( as.length ) {
			var plural = "";
			if ( ds.length > 1 ) {
				plural = "s";
			}
			var len = as.filter(getActive).length;
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
				if (getWaitingPlayers("all").length == 1) {
					var isAre = "is";
					var playerS = "player";
				}
				else if (getWaitingPlayers("all").length > 1) {
					var isAre = "are";
					var playerS = "players";
				}
				if (getWaitingPlayers("ai").length > 0)
					var aiHere = " ("+infoColor+getWaitingPlayers("ai").length+nrm+" ai)";
				else
					var aiHere = "";
				disp += "There "+isAre+" currently "+infoColor+getWaitingPlayers("all").length+nrm+" "+playerS+aiHere+" waiting to enter the Arena.\n";
			}
			else if ( global.arenaStage == constants.ARENA_PLAYING) {
				if (global.arenaPlayers.length == 1) {
					var isAre = "is";
					var playerS = "player";
				}
				else if (global.arenaPlayers.length > 1) {
					var isAre = "are";
					var playerS = "players";
				}
				disp += "There "+isAre+" currently "+infoColor+global.arenaPlayers.length+nrm+" "+playerS+" battling in the Arena.\n";
			}
			if ( !hideToday ) {
				var highQuery = sqlQuery("SELECT * FROM player_highs ORDER BY tdate DESC LIMIT 0,1;");
				high = highQuery.getRow.get("tcount");
				disp += "Today there was a high of "+infoColor+high+nrm+" players.\n";
			}
			if ( !hideRecord ) {
				var gHighQuery = sqlQuery("SELECT * FROM player_highs ORDER BY tcount DESC LIMIT 0,1;");
				var gRow = gHighQuery.getRow;
				var gHigh = gRow.get("tcount");
				var hDate = new Date(parseInt(gRow.get("tdate"))*1000);
				hDate = hDate.strftime("%m-%d-%Y");
				var arr = hDate.split("-");
				hDate = arr.join(nrm+"-"+infoColor);
				disp += "The record high of "+infoColor+gHigh+nrm+" players was reached on "+infoColor+hDate+nrm+".";
			}
			if(global.global2014Util){ // global event
				if(global.global2014Util.eventIsActive){
					disp += cyn+bld+"\n\nThe Kinslayer global event is active! Type JOIN EVENT to enter.\nLight Points: "+global.global2014Util.pointMap[constants.RACE_HUMAN]+"  Dark Points: "+global.global2014Util.pointMap[constants.RACE_TROLLOC]+nrm;
				}
			}

			if(global.globalDec2014Util && actor.level > 105)
			{
				disp += cyn + bld + "The Kinslayer global event is active! Current score: " + global.globalDec2014Util.points[constants.RACE_HUMAN] + " LS, " + global.globalDec2014Util.points[constants.RACE_TROLLOC] + " DS." + nrm;
			}
		}
		else {
			disp += "\n---------------------------------------";
		}
		actor.send(disp);
	
}