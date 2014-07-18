// Alder
// Achievement Editor OLC (achedit)
function bootAcheditOLC()
{
	var oConfig = new Object();
	var mode;
	oConfig.type = "achedit";
	oConfig.modes = [];
	/*** Main Menu ***/
	mode = new Object();
	mode.mode = "MODE_MAIN";
	mode.parser = function(actor,fLetter,vArgs) {
		var currMode = actor.getOLC().mode;
		if ( fLetter == "Q" ) {
			actor.cleanupOLC();
		}
		else if ( fLetter == "1" ) {
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_LIST_BY_VNUM");
		}
		else if ( fLetter == "2" ) {
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_LIST_BY_TAG");
		}
		else if ( fLetter == "N" ) {
			actor.getOLC().modeStack.push(mode.mode);
			actor.getOLC().switchToMode("MODE_ACH_DATA");
		}
		else {
			actor.send("Valid inputs are 1, 2, N, Q.\nTry again: ");
		}
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		////Initialize OLC properties\\\\
		actor.getOLC().achVnum = undefined;				//Vnum
		actor.getOLC().achName = undefined;				//Name
		actor.getOLC().achDesc = undefined;				//Description
		actor.getOLC().achExecutions = [];				//JSCode to execute when achievement is accomplished
		actor.getOLC().achFeatPoints = 1;				//Number of feat points awarded by achievement (1 by default)
		actor.getOLC().alwaysAwardFeatPoints = false;	//Boolean: award feat points each time ach. conditions met, even if already unlocked
		actor.getOLC().achHide = false;					//Boolean: hidden achievement?
		actor.getOLC().achTags = [];					//Tags
		actor.getOLC().achDelete = false;				//Boolean: delete achievement?
		actor.getOLC().selTag = undefined;				//Stores the selected tag
		actor.getOLC().selAch = undefined;				//Stores the selected achievement
		actor.getOLC().checkLoad = false;				//Checks if an achievement was loaded
		actor.getOLC().control = undefined;				//Used for OLC controlling
		actor.getOLC().modeStack = [];					//Holds the stack of modes, last element is last visited mode
		actor.send(" Achievement Editor\n"+strPadding("","_",65,"right")+"\n");
		actor.send(grn+"1"+nrm+") Achievements By Vnum");
		actor.send(grn+"2"+nrm+") Achievements By Tag");
		actor.send("");
		actor.send(grn+"N"+nrm+") Create New Achievement");
		actor.send(grn+"Q"+nrm+") Quit");
	}
	oConfig.modes.push(mode);
	/*** List By Tag ***/
	mode = new Object();
	mode.mode = "MODE_LIST_BY_TAG";
	mode.parser = function(actor,fLetter,vArgs) {
		var currMode = actor.getOLC().mode;
		var cmd = vArgs[0];
		var tags = getAllAchTags();
		var tagCount = tags.length;
		if ( fLetter == "Q" ) {
			actor.getOLC().switchToMode(actor.getOLC().modeStack.pop());
		}
		else if ( fLetter == "N" ) {
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_ACH_DATA");
		}
		else if ( isNumber(cmd) && cmd > 0 && cmd <= tagCount ) {
			actor.getOLC().selTag = tags[cmd-1];
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_LIST_BY_VNUM");
		}
		else {
			actor.send("Input was invalid.\nTry any of the green characters: ");
		}
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var tags = getAllAchTags();//FUNCTION NEEDS TO BE MADE
		actor.send(" Achievements By Tag\n"+strPadding("","_",65,"right")+"\n");
		var iPad = tags.length.toString().length+1;
		for ( var i = 0; i < tags.length; i++ ) {
			actor.send(grn+strPadding(i+1+""," ",iPad,"left")+nrm+") "+bld+tags[i]+nrm);
		}
		actor.send("");
		actor.send(grn+"N"+nrm+") Create New Achievement");
		actor.send(grn+"Q"+nrm+") Quit");
	}
	oConfig.modes.push(mode);
	/*** List By Vnum ***/
	mode = new Object();
	mode.mode = "MODE_LIST_BY_VNUM";
	mode.parser = function(actor,fLetter,vArgs) {
		var currMode = actor.getOLC().mode;
		var cmd = vArgs[0];
		var c = actor.getOLC().selTag;
		var achs = [];
		if ( c != undefined ) {
			achs = getAchievementsByTag(c);
		}
		else {
			achs = global.vAchievements;
		}
		if ( fLetter == "Q" ) {
			actor.getOLC().switchToMode(actor.getOLC().modeStack.pop());
		}
		else if ( fLetter == "N" ) {
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_ACH_DATA");
		}
		else if ( isNumber(cmd) && cmd > 0 && cmd <= achs.length ) {
			actor.getOLC().selAch = achs[cmd-1];
			mudLog(3,102,actor.name+" begins editing achievement "+actor.getOLC().selAch.achVnum);
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_ACH_DATA");
		}
		else {
			actor.send("Input was invalid.\nTry any of the green characters: ");
		}
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var c = actor.getOLC().selTag;
		var title;
		var achs = [];
		if ( c != undefined ) {
			achs = getAchievementsByTag(c);
			title = " Achievements With Tag: "+bld+c+nrm;
		}
		else {
			achs = global.vAchievements;
			title = " Achievements By Vnum";
		}
		actor.send(title+"\n"+strPadding("","_",65,"right")+"\n");
		var iPad = achs.length.toString().length+1;
		for ( var i = 0; i < achs.length; i++ ) {
			actor.send(grn+strPadding(i+1+""," ",iPad,"left")+nrm+") "+grn+bld+achs[i].achName+nrm+" ["+cyn+achs[i].achVnum+nrm+"]");
		}
		actor.send("");
		actor.send(grn+"N"+nrm+") Create New Achievement");
		actor.send(grn+"Q"+nrm+") Quit");
	}
	oConfig.modes.push(mode);
	/*** Save ***/
	var mode = new Object();
	mode.mode = "MODE_SAVE";
	mode.parser = function(actor,fLetter,vArgs) {
		if ( actor.getOLC().control == 5 ) {
			if( fLetter == "Y" ) {
				actor.getOLC().control = 10;
				actor.getOLC().switchToMode("MODE_SAVE");
			}
			else if( fLetter == "N" ) {
				actor.getOLC().switchToMode("MODE_ACH_DATA");
			}
			else {
				actor.send("Invalid input. Try again: ");
			}
			return;
		}
		if( fLetter == "Y" ) {
			if ( actor.getOLC().achVnum == undefined ) {
				getCharCols(actor);
				actor.send(bld+red+"You have to supply a vnum for this achievement before saving."+nrm);
				actor.getOLC().switchToMode("MODE_ACH_DATA");
				return;
			}
			var ach = new Object();
			ach.achVnum					= actor.getOLC().achVnum;
			ach.achName					= actor.getOLC().achName;
			ach.achDesc					= actor.getOLC().achDesc;
			ach.achHide					= actor.getOLC().achHide;
			ach.achExecutions			= actor.getOLC().achExecutions;
			ach.achTags					= actor.getOLC().achTags;
			ach.achFeatPoints			= actor.getOLC().achFeatPoints;
			ach.alwaysAwardFeatPoints	= actor.getOLC().alwaysAwardFeatPoints
			;
			mudLog(3,102,actor.name+" has finished editing achievement "+ach.achVnum);
			
			if ( actor.getOLC().selAch == undefined ) {
				global.vAchievements.push( ach );
			}
			else {
				for ( var i = 0; i < global.vAchievements.length; i++ ) {
					if ( global.vAchievements[i].achVnum == actor.getOLC().selAch.achVnum ) {
						global.vAchievements[i] = ach;
						var tempIndex = i;
						break;
					}
				}
			}
			if ( actor.getOLC().achDelete == true ) {
				if ( actor.getOLC().selAch != undefined ) {
					deleteAchievementFromDatabase( ach, true );
					global.vAchievements = reconArray(global.vAchievements,tempIndex,"remove");
				}
				else {
					global.vAchievements.pop();
				}
			}
			else {
				deleteAchievementFromDatabase( actor.getOLC().selAch,false );
				saveAchievement( ach );
			}
			function fSort(a,b) {
				return ( a.achVnum - b.achVnum );
			}
			global.vAchievements.sort(fSort);
		}
		else if( fLetter == "N" ) {
			actor.send("Changes discarded...\n");
			mudLog(3,102,actor.name+" has exited editing an achievement without saving");
		}
		else {
			actor.send("Invalid input. Try again: ");
			return;
		}
		actor.getOLC().switchToMode("MODE_MAIN");
	}
	mode.display = function(actor) {
		getCharCols(actor);
		if ( actor.getOLC().achDelete == true  && actor.getOLC().control != 5 && actor.getOLC().control != 10 ) {
			actor.send(bld+"This achievement is set to delete, continue? (Y/N)"+nrm);
			actor.getOLC().control = 5;
			return;
		}
		actor.send("Would you like to save this achievement (Y/N) : ");
	}
	oConfig.modes.push(mode);
	/*** Data ***/
	var mode = new Object();
	mode.mode = "MODE_ACH_DATA";
	mode.parser = function(actor,fLetter,vArgs) {
		var currMode = actor.getOLC().mode;
		if ( fLetter == "Q" ) {
			actor.getOLC().switchToMode("MODE_SAVE");
		}
		else if ( fLetter == "A" ) {
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_ENTER_VNUM");
		}
		else if ( fLetter == "B" ) {
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_ENTER_NAME");
		}
		else if ( fLetter == "E" ) {
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_ENTER_EXECUTIONS");
		}
		else if ( fLetter == "F" ) {
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_ENTER_FEAT");
		}
		else if ( fLetter == "G" ) {
			actor.getOLC().alwaysAwardFeatPoints = !actor.getOLC().alwaysAwardFeatPoints;
			actor.getOLC().switchToMode("MODE_ACH_DATA");
		}
		else if ( fLetter == "D" ) {
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_ENTER_DESC");
		}
		else if ( fLetter == "H" ) {
			actor.getOLC().achHide = !actor.getOLC().achHide;
			actor.getOLC().switchToMode("MODE_ACH_DATA");
		}
		else if ( fLetter == "T" ) {
			actor.getOLC().modeStack.push(currMode);
			actor.getOLC().switchToMode("MODE_ENTER_TAGS");
		}
		else if ( fLetter == "*" ) {
			actor.getOLC().achDelete = !actor.getOLC().achDelete;
			actor.getOLC().switchToMode("MODE_ACH_DATA");
		}
		else {
			actor.send("Input was invalid.\nTry any of the green characters: ");
		}
		return;
	}
	mode.display = function(actor) {
		actor.getOLC().control = undefined;
		////Load the Selected Achievement\\\\
		if ( actor.getOLC().selAch != undefined && actor.getOLC().checkLoad == false ) {
			actor.getOLC().checkLoad = true;
			var ach = actor.getOLC().selAch;					//Achievement object
			actor.getOLC().achVnum = ach.achVnum;				//Copy vnum
			actor.getOLC().achFeatPoints = ach.achFeatPoints;	//Copy feat points
			actor.getOLC().achHide = ach.achHide;				//Copy hidden value
			actor.getOLC().achName = ach.achName;				//Copy name
			actor.getOLC().achDesc = ach.achDesc;				//Copy desc
			actor.getOLC().alwaysAwardFeatPoints = ach.alwaysAwardFeatPoints;	//Copy always reward value
			for each ( var thing in ach.achExecutions ) {
				actor.getOLC().achExecutions.push(thing);		//Non-linked copy of JSCode lines
			}
			for each ( var tag in ach.achTags ) {
				actor.getOLC().achTags.push(tag);				//Non-linked copy of tags
			}
		}
		getCharCols(actor);
		actor.send(" Achievement Data\n"+strPadding("","_",65,"right")+"\n");
		actor.send(grn+"A"+nrm+") Vnum         : "+cyn+(actor.getOLC().achVnum != undefined ? actor.getOLC().achVnum : "<none>")+nrm);
		actor.send(grn+"B"+nrm+") Name         : "+cyn+(actor.getOLC().achName != undefined ? actor.getOLC().achName : "<none>")+nrm);
		actor.send(grn+"D"+nrm+") Description  : "+cyn+(actor.getOLC().achDesc != undefined ? actor.getOLC().achDesc : "<none>")+nrm);
		actor.send(grn+"E"+nrm+") Executions   : "+cyn+(actor.getOLC().achExecutions.length > 0 ? strAbbrev(actor.getOLC().achExecutions.join(", "),40) : "<none>")+nrm);
		actor.send(grn+"F"+nrm+") Feat Points  : "+cyn+actor.getOLC().achFeatPoints+nrm);
		actor.send(grn+"G"+nrm+") Always Award : "+(!actor.getOLC().alwaysAwardFeatPounts ? cyn : bld+red)+capFirstLetter(yesNo(actor.getOLC().alwaysAwardFeatPoints))+nrm+bld+" (Should feat points be awarded each time achievement conditions are met even if it is already unlocked?)"+nrm);
		actor.send(grn+"H"+nrm+") Hidden       : "+(!actor.getOLC().achHide ? cyn : bld+red)+capFirstLetter(yesNo(actor.getOLC().achHide))+nrm);
		actor.send(grn+"T"+nrm+") Tags         : "+cyn+(actor.getOLC().achTags.length > 0 ? strAbbrev(actor.getOLC().achTags.join(", "),40) : "<none>")+nrm);
		actor.send("");
		actor.send(grn+"*"+nrm+") Delete       : "+(!actor.getOLC().achDelete ? cyn : bld+red)+capFirstLetter(yesNo(actor.getOLC().achDelete))+nrm);
		actor.send("");
		actor.send(grn+"Q"+nrm+") Quit");
	}
	oConfig.modes.push(mode);
	/*** Enter Vnum ***/
	var mode = new Object();
	mode.mode = "MODE_ENTER_VNUM";
	mode.parser = function(actor,fLetter,vArgs) {
		var currMode = actor.getOLC().mode;
		var cmd = vArgs[0];
		if ( isNumber(cmd) && cmd > 0 ) {
			var vnums = [];
			for each ( var ach in global.vAchievements ) {
				if ( ach != actor.getOLC().selAch )
					vnums.push(ach.achVnum);
			}
			vnums.sort();
			if ( binarySearch(vnums,cmd,false,0,vnums.length-1) != -1 ) {
				actor.send("That achievement vnum already exists.");
				actor.getOLC().switchToMode(currMode);
				return;
			}
			actor.getOLC().achVnum = parseInt(cmd);
			actor.getOLC().switchToMode(actor.getOLC().modeStack.pop());
		}
		else {
			actor.send("Input must be an integer greater than zero.\nTry again: ");
		}
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld+"Input a unique achievement vnum: "+nrm);
	}
	oConfig.modes.push(mode);
	/*** Enter Feat Points ***/
	var mode = new Object();
	mode.mode = "MODE_ENTER_FEAT";
	mode.parser = function(actor,fLetter,vArgs) {
		var currMode = actor.getOLC().mode;
		var cmd = vArgs[0];
		if ( isNumber(cmd) && cmd >= 0 ) {
			actor.getOLC().achFeatPoints = parseInt(cmd);
			actor.getOLC().switchToMode(actor.getOLC().modeStack.pop());
		}
		else {
			actor.send("Input must be an integer greater than or equal to zero.\nTry again: ");
		}
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld+"Input number of feat points to award: "+nrm);
	}
	oConfig.modes.push(mode);
	/*** Enter Name ***/
	var mode = new Object();
	mode.mode = "MODE_ENTER_NAME";
	mode.parser = function(actor,fLetter,vArgs) {
		var currMode = actor.getOLC().mode;
		actor.getOLC().achName = vArgs.join(" ");
		actor.getOLC().switchToMode(actor.getOLC().modeStack.pop());
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld+"Input a name for this achievement: "+nrm);
	}
	oConfig.modes.push(mode);
	/*** Enter Desc ***/
	var mode = new Object();
	mode.mode = "MODE_ENTER_DESC";
	mode.parser = function(actor,fLetter,vArgs) {
		var currMode = actor.getOLC().mode;
		actor.getOLC().achDesc = vArgs.join(" ");
		actor.getOLC().switchToMode(actor.getOLC().modeStack.pop());
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send(bld+"Input a description of this achievement: "+nrm);
	}
	oConfig.modes.push(mode);
	/*** Enter Executions ***/
	var mode = new Object();
	mode.mode = "MODE_ENTER_EXECUTIONS";
	mode.parser = function(actor,fLetter,vArgs) {
		var currMode = actor.getOLC().mode;
		var cmd = vArgs[0];
		var execs = actor.getOLC().achExecutions;
		if ( actor.getOLC().control != undefined ) {
			if ( actor.getOLC().control == "ADD" ) {
				if ( isNumber(cmd) ) {
					actor.getOLC().achExecutions.splice((cmd-1),0,"<JS Code here>");
				}
				else {
					actor.send("Input must be an integer.\nTry again: ");
				}
			}
			else if ( actor.getOLC().control == "REMOVE" ) {
				if ( isNumber(cmd) && cmd > 0 ) {
					actor.getOLC().achExecutions.splice((cmd-1),1);
				}
				else if ( fLetter == "Q" ) {
				}
				else {
					actor.send("Input must be an integer from the list above, or Q.\nTry again: ");
				}
			}
			else if ( isNumber(actor.getOLC().control) ) {
				actor.getOLC().achExecutions[actor.getOLC().control] = vArgs.join(" ");
			}
			actor.getOLC().control = undefined;
			actor.getOLC().switchToMode("MODE_ENTER_EXECUTIONS");
			return;
		}
		if ( fLetter == "Q" ) {
			actor.getOLC().switchToMode(actor.getOLC().modeStack.pop());
		}
		else if ( fLetter == "A" ) {
			if ( execs.length == 0 ) {
				actor.getOLC().achExecutions.push("<JS Code here>");
			}
			else {
				actor.getOLC().control = "ADD";
			}
			actor.getOLC().switchToMode("MODE_ENTER_EXECUTIONS");
		}
		else if ( fLetter == "B" ) {
			if ( execs.length == 0 ) {
				actor.send("There is nothing to remove.");
				return;
			}
			actor.getOLC().control = "REMOVE";
			actor.getOLC().switchToMode("MODE_ENTER_EXECUTIONS");
		}
		else if ( fLetter == "B" ) {
			actor.getOLC().control = "REMOVE";
			actor.getOLC().switchToMode("MODE_ENTER_EXECUTIONS");
		}
		else if ( isNumber(cmd) && cmd > 0 && cmd <= execs.length ) {
			actor.getOLC().control = parseInt(cmd-1);
			actor.getOLC().switchToMode("MODE_ENTER_EXECUTIONS");
		}
		else {
			actor.send("Input was invalid.\nTry any of the green characters: ");
		}
		return;
	}
	mode.display = function(actor) {
		var execs = actor.getOLC().achExecutions;
		getCharCols(actor);
		if ( actor.getOLC().control != undefined ) {
			if ( actor.getOLC().control == "ADD" ) {
				actor.send(bld+"Input list position of new JS Code execution: "+nrm);
			}
			else if ( actor.getOLC().control == "REMOVE" ) {
				actor.send(bld+"Select a JS Code execution to remove(Q to quit): "+nrm);
			}
			else if ( isNumber(actor.getOLC().control) ) {
				actor.send(bld+"Enter line of JS Code to execute upon unlocking this achievement: "+nrm);
			}
			return;
		}
		actor.send(" JS Code Executions\n"+strPadding("","_",65,"right")+"\n");
		for ( var i = 0; i < execs.length; i++ ) {
			actor.send(grn+(i+1)+nrm+") "+cyn+execs[i]+nrm);
		}
		actor.send("");
		actor.send(grn+"A"+nrm+") Add Execution");
		actor.send(grn+"B"+nrm+") Remove Execution");
		actor.send(grn+"Q"+nrm+") Quit");
	}
	oConfig.modes.push(mode);
	/*** Enter Tags ***/
	var mode = new Object();
	mode.mode = "MODE_ENTER_TAGS";
	mode.parser = function(actor,fLetter,vArgs) {
		var currMode = actor.getOLC().mode;
		var cmd = vArgs[0];
		var tags = actor.getOLC().achTags;
		if ( actor.getOLC().control != undefined ) {
			if ( actor.getOLC().control == "ADD" ) {
				if ( isNumber(cmd) ) {
					actor.getOLC().achTags.splice((cmd-1),0,"<tag here>");
				}
				else {
					actor.send("Input must be an integer.\nTry again: ");
				}
			}
			else if ( actor.getOLC().control == "REMOVE" ) {
				if ( isNumber(cmd) && cmd > 0 ) {
					actor.getOLC().achTags.splice((cmd-1),1);
				}
				else if ( fLetter == "Q" ) {
				}
				else {
					actor.send("Input must be an integer from the list above, or Q.\nTry again: ");
				}
			}
			else if ( isNumber(actor.getOLC().control) ) {
				actor.getOLC().achTags[actor.getOLC().control] = vArgs.join(" ");
			}
			actor.getOLC().control = undefined;
			actor.getOLC().switchToMode("MODE_ENTER_TAGS");
			return;
		}
		if ( fLetter == "Q" ) {
			actor.getOLC().switchToMode(actor.getOLC().modeStack.pop());
		}
		else if ( fLetter == "A" ) {
			if ( tags.length == 0 ) {
				actor.getOLC().achTags.push("<tag here>");
			}
			else {
				actor.getOLC().control = "ADD";
			}
			actor.getOLC().switchToMode("MODE_ENTER_TAGS");
		}
		else if ( fLetter == "B" ) {
			if ( tags.length == 0 ) {
				actor.send("There is nothing to remove.");
				return;
			}
			actor.getOLC().control = "REMOVE";
			actor.getOLC().switchToMode("MODE_ENTER_TAGS");
		}
		else if ( fLetter == "B" ) {
			actor.getOLC().control = "REMOVE";
			actor.getOLC().switchToMode("MODE_ENTER_TAGS");
		}
		else if ( isNumber(cmd) && cmd > 0 && cmd <= tags.length ) {
			actor.getOLC().control = parseInt(cmd-1);
			actor.getOLC().switchToMode("MODE_ENTER_TAGS");
		}
		else {
			actor.send("Input was invalid.\nTry any of the green characters: ");
		}
		return;
	}
	mode.display = function(actor) {
		var tags = actor.getOLC().achTags;
		getCharCols(actor);
		if ( actor.getOLC().control != undefined ) {
			if ( actor.getOLC().control == "ADD" ) {
				actor.send(bld+"Input list position of new tag: "+nrm);
			}
			else if ( actor.getOLC().control == "REMOVE" ) {
				actor.send(bld+"Select a tag to remove(Q to quit): "+nrm);
			}
			else if ( isNumber(actor.getOLC().control) ) {
				actor.send(bld+"Enter name of tag(used to relate achievements): "+nrm);
			}
			return;
		}
		actor.send(" Tags\n"+strPadding("","_",65,"right")+"\n");
		for ( var i = 0; i < tags.length; i++ ) {
			actor.send(grn+(i+1)+nrm+") "+cyn+tags[i]+nrm);
		}
		actor.send("");
		actor.send(grn+"A"+nrm+") Add Tag");
		actor.send(grn+"B"+nrm+") Remove Tag");
		actor.send(grn+"Q"+nrm+") Quit");
	}
	oConfig.modes.push(mode);
	vOLC.push( oConfig );
}
bootOLC();
