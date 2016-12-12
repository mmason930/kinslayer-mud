// Rhollor January 2010
// Adapted from Alder's original Journal paging system
JSCharacter.prototype.viewMercList = function(cmd) {

	getCharCols(this);

	//Close List
	if ( strn_cmp(cmd[0],"close",1) )
	{
		this.detach(18607);
		this.send("You exit the mercenary list.");
		this.room.echoaround(this, capFirstLetter(this.name) + " stops viewing the mercenary list.");
		return;
	}

	var mercArray = MercUtil.getMercenaryArray(this.room.vnum);
	var currPage = MercUtil.getSavedListPage(this);

	var entryTotal = mercArray.length;
	var BUFFER = "--------------------------------------------------------------------------";

	if ( strn_cmp(cmd[0], "main menu",1) ) {
		currPage = -1; //Page -1 is the Main Menu
		MercUtil.setSavedListPage(this, currPage);
	}

	//Next page 
	else if(strn_cmp(cmd[0], "next", 1) && mercArray.length != 0) {
		currPage = MercUtil.getSavedListPage(this);
		if(currPage + 1 < entryTotal) {
			currPage += 1;
			MercUtil.setSavedListPage(this, currPage);
		}
		else { //go to main menu
			currPage = -1;
			MercUtil.setSavedListPage(this, currPage);
			this.viewMercList("main menu");
			this.send(red + "End of the list." + nrm);
			return;
		}
	}

	//previous a page
	else if ( strn_cmp(cmd[0],"previous",1) && mercArray != "empty") {
		currPage = MercUtil.getSavedListPage(this) - 1;
		if(currPage > -2) {
			MercUtil.setSavedListPage(this, currPage);
		}
		else {
			currPage = mercArray.length;
			MercUtil.setSavedListPage(this, currPage);
			this.viewMercList(currPage + "");
		}
	}
	//Review the page
	else if ( strn_cmp(cmd[0], "review", 1) && ( ( currPage >= 0 || mercArray.length != 0 ) || currPage == -1 )) {
		currPage = MercUtil.getSavedListPage(this);
	}

	//Page number	-- Which merc
	else if ( isNumber( cmd[0] ) && mercArray.length != 0 && cmd[0] < entryTotal + 1 && cmd[0] > 0) {
		currPage = Number(cmd[0]) - 1;
		MercUtil.setSavedListPage(this, currPage);
	}

	//Reload the mercenary array.
	else if(strn_cmp(cmd[0], "load", 1) && this.level > 99) {
		MercUtil.populateMercenaryArray(this.room.vnum);
	}

	//Delete the mercenary array.
	else if(strn_cmp(cmd[0], "delete", 6) && this.level > 99) {
		MercUtil.clearMercenaryArray(this.room.vnum);
		this.send("Deleting mercArray...");
	}

	//View the history for all classes.
	else if(strn_cmp(cmd[0], "history", 4) && this.level > 99) {

		this.viewMercList("main menu");
		this.send("Purchase History:");
		this.send("\tWarrior: \t" + cyn + fread("purchHistClass" + constants.CLASS_WARRIOR) + nrm);
		this.send("\tRanger: \t" + cyn + fread("purchHistClass" + constants.CLASS_RANGER) + nrm);
		this.send("\tThief: \t\t" + cyn + fread("purchHistClass" + constants.CLASS_THIEF) + nrm);
		this.send("\tChanneler: \t" + cyn + fread("purchHistClass" + constants.CLASS_CHANNELER) + nrm);
		this.send("\tDreadguard: \t" + cyn + fread("purchHistClass" + constants.CLASS_DREADGUARD) + nrm);
		return;
	}

	//Reset the purchas ehistory to 0 for all classes.
	else if(cmd[0] == "reset" && this.level > 99) {

		this.viewMercList("main menu");
		this.send(red + "Resetting Purchase History..." + nrm);
		fwrite("purchHistClass" + constants.CLASS_DREADGUARD,0);
		fwrite("purchHistClass" + constants.CLASS_WARRIOR,0);
		fwrite("purchHistClass" + constants.CLASS_RANGER,0);
		fwrite("purchHistClass" + constants.CLASS_THIEF,0);
		fwrite("purchHistClass" + constants.CLASS_CHANNELER,0);
		return;
	}

	// Hire the mercenary
	else if(strn_cmp(cmd[0], "hire", 1) && currPage >= 0 && currPage < entryTotal + 1 ) {

		var mercenariesInPlay = MercUtil.getMercenariesInPlayArray(this.room.vnum);
		//Check to make sure we don't excede maximum mecenaries allowed in game.
		if(mercenariesInPlay == null || mercenariesInPlay.length >= MercUtil.getMaximumMercenaries()) {
			this.viewMercList("main menu");
			this.send(red + "There is a hiring freeze, try again later." + nrm);
			return;
		}
		if(MercUtil.getMercenaryNumberArray(this).length >= MercUtil.MAX_MERCENARIES_OWNED) {
			this.viewMercList("main menu");
			this.send(red + "You can only hire " + intToText(MercUtil.MAX_MERCENARIES_OWNED) + " mercenaries at once." + nrm);
			return;
		}
		merc = mercArray[currPage];
		currPage = MercUtil.getSavedListPage(this);
		if(this.gold >= merc.price()) {

			this.gold -= merc.price();
			this.send(cyn + bld + "You hire " + merc.name + "." + nrm);
			echoaround(this, this.name + " hires " + merc.name + ".");
			var mercMob = this.room.loadMob(merc.vnum);
			// Set the Purchase History File
			fwrite("purchHistClass" + merc.className, parseInt(fread("purchHistClass" + merc.className)) + 1);
			if(this.level > 99) {
				this.send(bld + cyn + "Number purchased: " + fread("purchHistClass" + merc.className) +nrm);
			}

			// Apply Merc's svals for scripts
			MercUtil.setHomeRoom(mercMob, mercMob.room);
			MercUtil.setMercenaryData(mercMob, merc);
			MercUtil.addMercenaryToOwner(mercMob, this, merc.price());

			// Save the mercs eq
			MercUtil.saveMercenaryEquipment(mercMob);

			// Attach merc scripts
			mercMob.attach(18606);
			mercMob.attach(18605);
			mercMob.attach(18604);
			mercMob.attach(18603);

			// Attach Owner Scripts
			this.attach(18614);

			this.send(red + "If you rent or quit out you will lose this mercenary and " + mercMob.hisHer() + " equipment will not be saved." + nrm);
			mercMob.comm("follow " + this.name);

			// Set the mercenary mob's stats.
			mercMob.strength = merc.str;
			mercMob.intelligence = merc.intel;
			mercMob.wisdom = merc.wis;
			mercMob.dexterity = merc.dex;
			mercMob.constitution = merc.con;
			mercMob.maxHit = merc.hps;
			mercMob.maxMove = merc.mvs;
			mercMob.hps = merc.hps;
			mercMob.mvs = merc.mvs;
			mercMob.maxSpell = merc.maxSpell;
			mercMob.sps = merc.maxSpell;
		}
		else {
			this.send(red + "You don't have enough coin!" + nrm);
		}
		this.viewMercList("main menu");
		return;
	}
	else
	{
		//Invalid command
		if(mercArray.length == 0) {
			MercUtil.setSavedListPage(this, -1);
			this.viewMercList("main menu");
		}
		else {
			this.viewMercList("review");
		}
		this.send(red + "Invalid command." + nrm);
		return;
	}

	mercArray = MercUtil.getMercenaryArray(this.room.vnum);
	if(currPage == -1) {
		//Main Menu
		if(mercArray == null) {
			this.detach(18607);
			this.send("Sorry, but you cannot do that here!");
			return;
		}
		var clr = cyn;
		var mercenariesHiredOut = MercUtil.getMercenaryNumberArray(this).length;
		if(mercenariesHiredOut == MercUtil.MAX_MERCENARIES_OWNED) {
			clr = bld + cyn;
		}
		this.send("Mercenaries -- Main Menu             Mercenaries Hired Out: " + clr + mercenariesHiredOut + nrm + "/" + clr + MercUtil.MAX_MERCENARIES_OWNED + nrm);
		this.send(BUFFER);
		if(mercArray.length == 0) {
			this.send(red + " There are no mercenaries available to hire." + nrm);
		}
		else {
			for(var i = 0; i < mercArray.length; i++) {
				var buf1;
				var buf2 = "";
				buf1 = grn + (i+1) + nrm + ") " + capFirstLetter(mercArray[i].name);
				for( var j = buf1.length - 1; j < 45; j++) {
					buf2 += " ";
				}
				this.send(buf1 + buf2 + "Price: " + this.numCopperToText(mercArray[i].price(),true));
			}
		}
	}
	else if ( currPage > entryTotal - 1 || currPage < 0 ) {
		//The directed page is not within realms of the page limit
		this.viewMercList("review");
		this.send(bld + "Invalid number. There are only " + entryTotal + " mercenaries available." + nrm);
		return;
	}
	else if( currPage >= 0 ) {

		var merc = mercArray[currPage];
		var buf1 = bld + "Mercenary " + (currPage+1) + ":" + nrm;
		var buf2 ="";
		for( var i = buf1.length - 1; i < 40; i++) {
			buf2 += " ";
		}
		this.send(buf1 + buf2 + "Your Gold: " + this.numCopperToText(this.gold,true) + nrm);
		this.send(BUFFER);

		buf2 = "";
		buf1 = "Name: " + cyn + capFirstLetter(merc.name) + nrm;
		for( var i = buf1.length - 1; i < 66; i++) {
			buf2 += " ";
		}
		this.send(buf1 + buf2 + "Str: " + cyn + merc.str + nrm);
		buf2 = "";
		buf1 = "Class: " + cyn + capFirstLetter(classNumToText(merc.className)) + nrm;
		for( var i = buf1.length - 1; i < 66; i++) {
			buf2 += " ";
		}
		this.send(buf1 + buf2 + "Int: " + cyn + merc.intel + nrm);
		buf2 = "";
		buf1 = "Health Points: " + cyn + merc.hps + nrm;
		for( var i = buf1.length - 1; i < 66; i++) {
			buf2 += " ";
		}
		this.send(buf1 + buf2 + "Wis: " + cyn + merc.wis + nrm);
		buf2 = "";
		buf1 = "Moves Points: " + cyn + merc.mvs + nrm;
		for( var i = buf1.length - 1; i < 66; i++) {
			buf2 += " ";
		}
		this.send(buf1 + buf2 + "Dex: " + cyn + merc.dex + nrm);
		buf2 = "";
		if(merc.skill > -1) {
			buf1 = "Skill: " + cyn + merc.skillsList[merc.skill] + nrm;
		}
		else {
			buf1 = "Skill: " + cyn + "None" + nrm;
		}
		for( var i = buf1.length - 1; i < 66; i++) {
			buf2 += " ";
		}
		this.send(buf1 + buf2 + "Con: " + cyn + merc.con + nrm);

		//--READD WHEN CHANNIES GO IN-- this.send(buf1);
		buf2 = "";
		buf1 = "\n[" + grn + "H" + nrm + "] Hire";
		for( var i = buf1.length - 1; i < 46; i++) {
			buf2 += " ";
		}
		this.send(buf1 + buf2 + "Price: " + this.numCopperToText(merc.price(),true) + nrm);
	}
	MercUtil.setSavedListPage(this, currPage);
	var pagingPrompt = "[" + grn + "M" + nrm + "] Main Menu\t\t[" + grn + "N" + nrm + "] Next Page\n[" + grn + "R" + nrm + "] Review the Page\t[" + grn + "P" + nrm + "] Previous Page\n[" + grn + "C" + nrm + "] Close";
	this.send("\n" + BUFFER + "\n" + pagingPrompt);
};
//Eventually come up with a channeling script for mercs, allow them to turn "passive channeling" on
