//Alder
//May 2010
/*****************************
 *    Parse who arguments    *
 *****************************/
function parseWhoArgs(actor,vArgs) {
	var infoMsg = "";
	var condensed = actor.quest("WHO_CONDENSED");
	var nl = "\n";
	if ( condensed ) {
		nl = "";
	}
	if ( strn_cmp(vArgs[1],"condensed",3) ) {
		var cVal = actor.quest("WHO_CONDENSED");
		actor.qval("WHO_CONDENSED",!cVal);
		if ( !cVal ) {
			nl = "";
			infoMsg = (cyn+"Condensed who list ON.\n"+nl+nrm);
		}
		else {
			nl = "\n"
			infoMsg = (cyn+"Condensed who list OFF.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"setcolor",3) ) {
		if ( vArgs.length > 2 ) {
			var color = "";
			var magenta = strn_cmp(vArgs[2],"magenta",1);
			var blue = strn_cmp(vArgs[2],"blue",2);
			var green = strn_cmp(vArgs[2],"green",1);
			var yellow = strn_cmp(vArgs[2],"yellow",1);
			var cyan = strn_cmp(vArgs[2],"cyan",1);
			var redc = strn_cmp(vArgs[2],"red",1);
			var boldc = strn_cmp(vArgs[2],"bold",2);
			if ( green ) {
				actor.qval("WHO_INFO_COLOR",0);
				color = "green";
			}
			else if ( blue ) {
				actor.qval("WHO_INFO_COLOR",1);
				color = "blue";
			}
			else if ( cyan ) {
				actor.qval("WHO_INFO_COLOR",2);
				color = "cyan";
			}
			else if ( magenta ) {
				actor.qval("WHO_INFO_COLOR",3);
				color = "magenta";
			}
			else if ( yellow ) {
				actor.qval("WHO_INFO_COLOR",4);
				color = "yellow";
			}
			else if ( redc ) {
				actor.qval("WHO_INFO_COLOR",5);
				color = "red";
			}
			else if ( boldc ) {
				var cVal = actor.quest("WHO_INFO_BOLD");
				actor.qval("WHO_INFO_BOLD",!cVal);
				if ( !cVal ) {
					color = "bold";
				}
				else {
					color = "un-bold";
				}
			}
			else
				infoMsg = cyn+"Valid colors are green, magenta, blue, yellow, cyan, red, bold.\n"+nl+nrm;
		}
		else {
			color = "default (green)";
			actor.qval("WHO_INFO_COLOR",0);
			actor.qval("WHO_INFO_BOLD",0);
		}
		if ( !infoMsg.length )
			infoMsg = cyn+"Info color set to "+color+".\n"+nl+nrm;
	}
	else if ( strn_cmp(vArgs[1],"racecolors",1) && actor.level >= 100 ) {
		var cVal = actor.quest("WHO_RACE_COLOR");
		actor.qval("WHO_RACE_COLOR",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Race color indicator on.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Race color indicator off.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"legends",1) ) {
		var cVal = actor.quest("WHO_LEG_POS");
		actor.qval("WHO_LEG_POS",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Legend rankings moved to right margin.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Legend rankings moved to left margin.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"key",1) ) {
		var cVal = actor.quest("WHO_KEY_SHOW");
		actor.qval("WHO_KEY_SHOW",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Legends Ranking key no longer displaying.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Legends Ranking key now displaying.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"idle",1) && actor.level >= 100 ) {
		var cVal = actor.quest("WHO_IDLE_SPAM");
		actor.qval("WHO_IDLE_SPAM",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Spammy idle timers OFF.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Spammy idle timers ON.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"titles",2) ) {
		var cVal = actor.quest("WHO_TITLE_SPAM");
		actor.qval("WHO_TITLE_SPAM",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Titles no longer displaying.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Titles now displaying.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"stitles",2) && actor.level >= 100 ) {
		var cVal = actor.quest("WHO_STITLE_SPAM");
		actor.qval("WHO_STITLE_SPAM",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Special titles no longer displaying.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Special titles now displaying.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"tcolors",2) ) {
		var cVal = actor.quest("WHO_TITLE_COLOR");
		actor.qval("WHO_TITLE_COLOR",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Title colors OFF.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Title colors ON.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"colors",3) ) {
		var cVal = actor.quest("WHO_COLOR");
		actor.qval("WHO_COLOR",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Who colors OFF.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Who colors ON.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"bolding",1) ) {
		var cVal = actor.quest("WHO_BOLDING");
		actor.qval("WHO_BOLDING",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Bolding for player status tags ON.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Bolding for player status tags OFF.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"immortals",2) || strn_cmp(vArgs[1],"imms",2) ) {
		var cVal = actor.quest("WHO_SHOW_IMMS");
		actor.qval("WHO_SHOW_IMMS",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Immortals no longer displaying.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Immortals now displaying.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"todayhigh",3) ) {
		var cVal = actor.quest("WHO_SHOW_TODAY");
		actor.qval("WHO_SHOW_TODAY",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Today's player high no longer displaying.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Today's player high now displaying.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"recordhigh",3) ) {
		var cVal = actor.quest("WHO_SHOW_RECORD");
		actor.qval("WHO_SHOW_RECORD",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Record player high no longer displaying.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Record player high now displaying.\n"+nl+nrm);
		}
	}
	else if ( strn_cmp(vArgs[1],"stats",2) ) {
		var cVal = actor.quest("WHO_SHOW_STATS");
		actor.qval("WHO_SHOW_STATS",!cVal);
		if ( !cVal ) {
			infoMsg = (cyn+"Stats no longer displaying.\n"+nl+nrm);
		}
		else {
			infoMsg = (cyn+"Stats now displaying.\n"+nl+nrm);
		}
	}
	else {
		var idleSpam = (actor.quest("WHO_IDLE_SPAM") ? nrm+bld+cyn : "" );
		var titleSpam = (actor.quest("WHO_TITLE_SPAM") ? nrm+cyn : "" );
		var stitleSpam = (actor.quest("WHO_STITLE_SPAM") ? nrm+cyn : "" );
		var colorOff = (actor.quest("WHO_COLOR") ? nrm+cyn : "" );
		var titleColor = (actor.quest("WHO_TITLE_COLOR") ? nrm+cyn : "" );
		var bolding = (actor.quest("WHO_BOLDING") ? nrm+cyn : "" );
		var condensed = (actor.quest("WHO_CONDENSED") ? nrm+cyn : "" );
		var hideImms = (actor.quest("WHO_SHOW_IMMS") ? nrm+cyn : "" );
		var hideToday = (actor.quest("WHO_SHOW_TODAY") ? nrm+cyn : "" );
		var hideRecord = (actor.quest("WHO_SHOW_RECORD") ? nrm+cyn : "" );
		var hideStats = (actor.quest("WHO_SHOW_STATS") ? nrm+cyn : "" );
		var legPos = (actor.quest("WHO_LEG_POS") ? nrm+cyn : "" );
		var raceColor = (actor.quest("WHO_RACE_COLOR") ? nrm+cyn : "" );
		var key =  ( actor.quest("WHO_KEY_SHOW") ? nrm+cyn : "" );
		var rCol =  ( actor.quest("WHO_RACE_COLOR") ? nrm+bld+cyn : "" );
		var arrColors = [grn,blu,cyn,mag,yel,red];
		var vBold = ( actor.quest("WHO_INFO_BOLD") ? bld : "" );
		var infoColor = vBold+arrColors[parseInt(actor.quest("WHO_INFO_COLOR"))];
		var prompt = hangIndentStr("Who command arguments: "+legPos+"legends"+nrm+","+key+" key"+nrm+","+colorOff+" colors"+nrm+","+titleColor+" tcolors"+nrm+","+bolding+" bolding"+nrm+","+bld+condensed+" condensed"+nrm+","+hideToday+" todayhigh"+nrm+","+bld+hideRecord+" recordhigh"+nrm+","+titleSpam+" titles"+nrm+","+hideImms+" immortals"+nrm+","+infoColor+" setcolor "+nrm+"<"+grn+"green"+nrm+"/"+mag+"magenta"+nrm+"/"+blu+"blue"+nrm+"/"+yel+"yellow"+nrm+"/"+red+"red"+nrm+"/"+cyn+"cyan"+nrm+"/"+bld+infoColor+"bold"+nrm+">"+nrm+(actor.level >= 100 ? nrm+","+idleSpam+" idle"+nrm+","+bld+stitleSpam+" stitles"+nrm+","+rCol+" racecolors" : "")+nrm+".",55,23);
		actor.send(prompt+"\n"+cyn+"  **Toggled**\n"+nrm+bld+"**Not toggled**"+nrm);
		return false;
	}
	return infoMsg;
}
