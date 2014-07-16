// Rhollor January 2010
// Adapted from Alder's original Journal paging system.
function mercObj(className, race) {
	const HUMAN = constants.RACE_HUMAN;
	const TROLLOC = constants.RACE_TROLLOC;
	this.maxSpell = 3; //Spell bonus -- adds to max_maxSpell (which is really just an sval)
	/**********************************************************************
	 * Class:		Skills:												  *
	 *--------------------------------------------------------------------*
	 * Channeler	Healing[0]											  *
	 * Warrior		Search[2], Precise Strike[3], Shield Block[4]		  *
	 * Ranger		Precise Strike[3], Shield Block[4], Pick[5]			  *
	 * Thief		Bash[1], Search[2]									  *
	 **********************************************************************/
	this.skillsList = ["Healing","Bash","Search","Precise Strike","Shield Block","Pick","Channeling"];
	this.skillBonus = 4; //Multiplier for skills(channeling or other class skills)
	this.skill = -1; //Which skill does the merc possess?
	this.str = 15; 
	this.intel = 15;
	this.wis = 15;
	this.dex = 15;
	this.con = 15;
	this.className = className; //This is actually an int corresponding to constants.CLASS_XXX
	this.chan = false; //Merc can channel?
	if(this.className == constants.CLASS_CHANNELER || this.className == constants.DREADGUARD) {
		/** Channeler Stats**/
		if(race == HUMAN) {
			this.vnum = 18603;
		}
		else if(race == TROLLOC){
			this.vnum = 18607;
		}
		if(random(-2,1) > 0) {
			this.skill = random(-3,0);
		}
		this.wis = 19 + random(-1,2);
		this.intel = 19 + random(-1,2);
		this.dex = 16 + random(-1,1);
		this.str = 15 + random(-3,1);
		this.con = 15 + random(-2,1);
		this.chan = true;
	}
	else if(this.className == constants.CLASS_WARRIOR) {
		/** Warrior Stats **/
		if(race == HUMAN) {
			this.vnum = 18600;
		}
		else if(race == TROLLOC){
			this.vnum = 18604;
		}
		if(random(-2,1) > 0) {
			this.skill = random(2,4);
		}
		this.str = 18 + random(0,4);
		this.intel = 15 + random(-2,2);
		this.wis = 15 + random(-2,2);
		this.dex = 15 + random(-4, 1);
		this.con = 20 + random(-1,2);
	}
	else if(this.className == constants.CLASS_RANGER) {
		/** Ranger Stats **/
		if(race == HUMAN) {
			this.vnum = 18601;
		}
		else if(race == TROLLOC){
			this.vnum = 18605;
		}
		if(random(-2,1) > 0) {
			this.skill = random(3,5);
		}
		this.str = 17 + random(-1,3);
		this.intel = 15 + random(-1,4);
		this.wis = 17 + random(-2,3);
		this.dex = 17 + random(-2,4);
		this.con = 17 + random(0,4);
	}
	else if(this.className == constants.CLASS_THIEF) {
		/** Thief Stats **/
		if(race == HUMAN) {
			this.vnum = 18602;
		}
		else if(race == TROLLOC){
			this.vnum = 18606;
		}
		if(random(-2,1) > 0) {
			this.skill = random(1,2);
		}
		this.str = 15 + random(0,4);
		this.intel = 15 + random(0,3);
		this.wis = 13 + random(-2,2);
		this.dex = 18 + random(0,4);
		this.con = 17 + random(0,4);
	}
	if(this.skill == this.skillsList.length - 1) { //Skill: Channeling
		this.chan = true;
	}
	this.name = getMobName(this.vnum);
	this.hps = 16*this.con + 4*this.wis + 2*this.intel;
	this.mvs = Math.floor(3.5*this.con + 5.5*this.dex); //for trollocs add 100
	if(this.race == TROLLOC) {
		this.mvs += this.mvs;
	}
	if(this.chan) {
		this.maxSpell = 5*this.wis + 4*this.intel + this.con;
		this.sps = this.maxSpell;
	}
	this.skillBonus = Math.floor((5*this.wis + 3*this.intel + 2*this.dex)/10) + random(-4,4);
	this.price = function() {
		/* Calculate the merc's price based on multiple stats and bonsuses
		 * and number of historically purchased mercs of that class */
		var base = 0; //ADJUST TO AFFECT BASE PRICE OF ALL MERCS
		var price = base + 2*this.str + this.intel + this.wis + 2*this.dex + 3*this.con + 3*parseInt(fread("purchHistClass" + this.className))/50;
		if(this.chan) {
			/**** CHANGE THIS TO AFFECT PRICE OF CHANNELER MERCS ****/
			price *= 5.25;
		}
		if(this.skill > -1) {
			price *= 3;
		}
		return Math.floor(price);
	};
	
}
JSCharacter.prototype.viewMercList = function(cmd) {
	const MAX_MERCS_IN_GAME = 5;
	const MAX_MERCS_OWNED = 2;
	const TROLLOC_DEPOT_VNUM = 8081; //The vnum of the room for trollocs
	const HUMAN_DEPOT_VNUM = 388; //The vnum of the room for humans
	
	const HUMAN = constants.RACE_HUMAN;
	const TROLLOC = constants.RACE_TROLLOC;
	const cyn = this.cyan(constants.CL_OFF); 
	const ylw = this.yellow(constants.CL_OFF);
	const grn = this.green(constants.CL_OFF);
	const red = this.red(constants.CL_OFF);
	const mag = this.magenta(constants.CL_OFF);
	const blu = this.blue(constants.CL_OFF);
	const bld = this.bold(constants.CL_OFF);
	const nrm = this.normal(constants.CL_OFF);
	
	if(this.level > 99) {
		this.send(bld+cyn+"Trolloc: " + TROLLOC + ", Human: " + HUMAN + ", " + this.name + ": " + this.race+nrm);
		this.send(bld+cyn+"Trolloc Depot: " + TROLLOC_DEPOT_VNUM + ", Human Depot: " + HUMAN_DEPOT_VNUM + ", Here: " + this.room.vnum+nrm);
	}
	const THIEF = constants.CLASS_THIEF;
	const WARRIOR = constants.CLASS_WARRIOR;
	const RANGER = constants.CLASS_RANGER;
	const CHANNELER = constants.CLASS_CHANNELER;
	const DREADGUARD = constants.CLASS_DREADGUARD;
	
	/** Update the mercsOwned array **/
	/* This array contains the JSChar of all mercs purchased from this location */
	var mercsOwned = [];
	mercsOwned = getSval(this.room,-18600,"mercsOwned");
	if(!mercsOwned)
		mercsOwned = [];
	for(var i=0; i < mercsOwned.length; i++) {
		if(!mercsOwned[i].isValid) {
			//Merc is no longer valid, remove from the array
			mercsOwned.splice(i,1);
			i--;
		}
	}
	setSval(this.room,-18600,"mercsOwned",mercsOwned);
	
	/** Update the Actor's myMercs array **/
	/* This array contains the JSchar of all the mercs owned by "this"  */
	var myMercs = [];
	myMercs = getSval(this,-18600,"myMercs");
	if(!myMercs)
		myMercs = [];
	for(var i=0; i < myMercs.length; i++) {
		if(!myMercs[i].isValid) {
			//Merc is no longer valid, remove from the array
			myMercs.splice(i,1);
			i--;
		}
	}
	setSval(this,-18600,"myMercs",myMercs);
	
	/*Close List*/
	if ( strn_cmp(cmd[0],"close",1) ) {
		this.detach(18607);
		this.send("You exit the mercenary list.");
		this.room.echoaround(this, capFirstLetter(this.name) + " stops viewing the mercenary list.");
			return;
	}
	var mercArray = getSval(this.room,-18600,"mercArray");
	var currPage = getSval(this,-18600,"savedListPage");
	JSCharacter.prototype.createMercArray = function() {
		if((this.race == HUMAN && this.room.vnum != HUMAN_DEPOT_VNUM) || (this.race == TROLLOC && this.room.vnum != TROLLOC_DEPOT_VNUM)) {
			this.detach(18607);
			return null;
		}
		mudLog(3,100,"Mercenary array created in room " + this.room.vnum + " for " + raceNumToText(this.race) + " race.");
		if(this.level > 99) {
			this.send(red + "        ***************Setting room's mercArray***************" + nrm);
		}
		mercArray = [];
		var merc;
		for(var i = 0; i < 10; i++) {
			/* Pick a Class for Each Merc */
			var randClass = random(WARRIOR,CHANNELER-1); //remove "-1" when channies go in
			if(randClass == CHANNELER) {
				//Reduce number of CHANNELER loads
				if(random(WARRIOR,CHANNELER) != CHANNELER) {
					randClass = random(WARRIOR,RANGER);
				}
			}
			merc = new mercObj(randClass, this.race);
			mercArray.push(merc);
		}
		setSval(this.room,-18600,"mercArray",mercArray);
		return;		
	}
	if(!mercArray || mercArray == "") {
		if(this.createMercArray() == null) {
			return;
		}
	}
	var entryTotal = mercArray.length;
	var BUFFER = "--------------------------------------------------------------------------";
	if ( strn_cmp(cmd[0], "main menu",1) ) {
			var currPage = -1; //Page -1 is the Main Menu
			setSval(this,-18600,"savedListPage",currPage);
	}
	//Next page 
	else if ( strn_cmp(cmd[0],"next",1) && mercArray != "empty") {
		var currPage = getSval(this,-18600,"savedListPage");
		if(currPage + 1 < entryTotal) {
			currPage += 1;
			setSval(this,-18600,"savedListPage",currPage);
		}
		else { //go to main menu
			currPage = -1;
			setSval(this,-18600,"savedListPage",currPage);
			this.viewMercList("main menu");
			this.send(red + "End of the list." + nrm);
			return;
		}
	}
	//previous a page
	else if ( strn_cmp(cmd[0],"previous",1) && mercArray != "empty") {
		var currPage = getSval(this,-18600,"savedListPage") - 1;
		if(currPage > -2) {
			setSval(this,-18600,"savedListPage",currPage);
		}
		else {
			currPage = mercArray.length;
			setSval(this,-18600,"savedListPage",currPage);
			this.viewMercList(currPage + "");
		}
	}
	//Review the page
	else if ( strn_cmp(cmd[0], "review",1) && ( ( currPage >= 0 || mercArray != "empty" ) || currPage == -1 )) {
		var currPage = getSval(this,-18600,"savedListPage");
	}
	//Page number	-- Which merc
	else if ( isNumber( cmd[0] ) && mercArray != "empty" && cmd[0] < entryTotal + 1 && cmd[0] > 0) {
		var currPage = Number(cmd[0]) - 1;	
		setSval(this,-18600,"savedListPage",currPage);
	}
	//Reload mercArray for imm
	else if(strn_cmp(cmd[0],"load",1) && this.level > 99) {
		/** Reload the mercArray **/
		this.createMercArray();
	}
	else if(strn_cmp(cmd[0],"delete",6) && this.level > 99) {
		/** Delete the rooms mercArray **/
		setSval(this.room,-18600,"mercArray",0);
		this.send("Deleting mercArray...");
	}
	else if(strn_cmp(cmd[0],"history",4) && this.level > 99) {
		/** View the Purchase History for all Classes **/
		this.viewMercList("main menu");
		this.send("Purchase History:");
		this.send("\tWarrior: \t" + cyn + fread("purchHistClass" + WARRIOR) + nrm);
		this.send("\tRanger: \t" + cyn + fread("purchHistClass" + RANGER) + nrm);
		this.send("\tThief: \t\t" + cyn + fread("purchHistClass" + THIEF) + nrm);
		this.send("\tChanneler: \t" + cyn + fread("purchHistClass" + CHANNELER) + nrm);
		this.send("\tDreadguard: \t" + cyn + fread("purchHistClass" + DREADGUARD) + nrm);
		return;
	}
	else if(cmd[0] == "reset" && this.level > 99) {
		/** Reset the Purchase History to 0 for all Classes **/
		this.viewMercList("main menu");
		this.send(red + "Reseting Purchase History..." + nrm);
		fwrite("purchHistClass" + DREADGUARD,0);
		fwrite("purchHistClass" + WARRIOR,0);
		fwrite("purchHistClass" + RANGER,0);
		fwrite("purchHistClass" + THIEF,0);
		fwrite("purchHistClass" + CHANNELER,0);
		setSval(this.room,-18600,"mercsOwned",0);
		return;
	}
	/**** Hire the Merc ****/
	else if ( strn_cmp(cmd[0], "hire", 1) && currPage >= 0 && currPage < entryTotal + 1 ) {
		//Check to make sure we dont excede MAX_MERCS_IN_GAME
		if(getSval(this.room,-18600,"mercsOwned").length >= MAX_MERCS_IN_GAME) {
			this.viewMercList("main menu");
			this.send(red + "There is a hiring freeze, try again later." + nrm);
			return;
		}
		if(getSval(this,-18600,"myMercs").length >= MAX_MERCS_OWNED) {
			this.viewMercList("main menu");
			this.send(red + "You can only hire " + intToText(MAX_MERCS_OWNED) + " mercenaries at once."+nrm);
			return;
		}
		merc = mercArray[currPage];
		var currPage = getSval(this,-18600,"savedListPage");
		if(this.gold >= merc.price()) {
			this.gold -= merc.price();
			this.send(cyn + bld + "You hire " + merc.name + "." + nrm);
			echoaround(this, this.name + " hires " + merc.name + ".");
			this.room.loadMob(merc.vnum);
			var Merc = this.room.people[0]; //Should be merc
			
			/* Set the Purchase History File */
			fwrite("purchHistClass" + merc.className, parseInt(fread("purchHistClass" + merc.className)) + 1);
			if(this.level > 99) {
				this.send(bld + cyn + "Number purchased: " + fread("purchHistClass" + merc.className) +nrm);
			}
			
			/* Apply Merc's svals for scripts */
			setSval(Merc,18600,"owner",this);
			setSval(Merc,-18600,"myMercObj",merc);
			setSval(Merc,18600,"price",merc.price());
			setSval(Merc,18600,"mercNum",-1);
			setSval(Merc,18600,"homeRoom",Merc.room);
			var numMerc = getSval(this,18601,"numMerc");
			//this.send("numMerc: " + numMerc);
			if(!numMerc) {
				numMerc = 0;
			}
			//this.send("numMerc: " + numMerc);
			setSval(Merc,18600,"mercNum",numMerc+1);
			setSval(this,18601,"numMerc",numMerc+1);
			
			/** Update mercsOwned array **/
			mercsOwned.push(Merc);
			setSval(this.room,-18600,"mercsOwned",mercsOwned);
			
			var myMercs = getSval(this,-18600,"myMercs");
			/** Update myMercs array **/
			myMercs.push(Merc);
			setSval(this,-18600,"myMercs",myMercs);
			
			/* Save the mercs eq */
			Merc.mobSaveEq(this.id + ".mercEQ." + getSval(Merc,18600,"mercNum"));
			
			if(this.level > 99) {
				this.send(bld+cyn+"Mercs in game for " + raceNumToText(Merc.race) + " race: " + mercsOwned.length+nrm);
			}
			
			/* Attach merc scripts */
			Merc.attach(18606);
			Merc.attach(18605);
			Merc.attach(18604);
			Merc.attach(18603);
			
			/* Attach Owner Scripts */
			this.attach(18614);
			
			this.send(red + "If you rent or quit out you will lose this mercenary and " + Merc.his_her() +" equipment will not be saved."+ nrm);
			Merc.comm("follow " + this.name);
			
			/** Set the Merc's Stats **/
			Merc.strength = merc.str;
			Merc.intelligence = merc.intel;
			Merc.wisdom = merc.wis;
			Merc.dexterity = merc.dex;
			Merc.constitution = merc.con;
			Merc.maxHit = merc.hps;
			Merc.maxMove = merc.mvs;
			Merc.hps = merc.hps;
			Merc.mvs = merc.mvs;
			Merc.maxSpell = merc.maxSpell;
			Merc.sps = merc.maxSpell;
			
			/*** Remove the sold merc from list ***/
			mercArray.splice(currPage,1);
		}
		else {
			this.send(red + "You don't have enough coin!" + nrm);
		}
		if(mercArray.length > 0) {
			setSval(this.room,-18600,"mercArray",mercArray);
		}
		else {
			setSval(this.room,-18600,"mercArray","empty");
		}
		this.viewMercList("main menu");
		return;
	}
	else {         
		//Invalid command
		if(mercArray == "empty") {
			setSval(this,-18600,"savedListPage",-1);
			this.viewMercList("main menu");
		}
		else {
			this.viewMercList("review");
		}
		this.send(red + "Invalid command." + nrm);
		return;
	}
	mercArray = getSval(this.room,-18600,"mercArray");
	if(currPage == -1) {
		//Main Menu
		if((this.race == HUMAN && this.room.vnum != HUMAN_DEPOT_VNUM) || (this.race == TROLLOC && this.room.vnum != TROLLOC_DEPOT_VNUM)) {
			this.detach(18607);
			this.send("Sorry, but you cannot do that here!");
			return;
		}
		var clr = cyn;
		if(mercsOwned.length == MAX_MERCS_IN_GAME) {
			clr = bld + cyn;
		}
		this.send("Mercenaries -- Main Menu             Mercenaries Hired Out: " + clr + mercsOwned.length + nrm + "/" + clr + MAX_MERCS_IN_GAME + nrm);
		this.send(BUFFER);
		if(mercArray == "empty") {
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
		mercArray = getSval(this.room,-18600,"mercArray");
		var merc = mercArray[currPage];
		var buf1 = bld + "Mercenary " + (currPage+1) + ":" + nrm;
		var buf2 ="";
		for( var i = buf1.length - 1; i < 40; i++) {
			buf2 += " ";
		}
		this.send(buf1 + buf2 + "Your Gold: " + this.numCopperToText(this.gold,true) + nrm);
		this.send(BUFFER);
		buf1;
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
		if(merc.chan) {
			buf1 = "Spell Points: " + cyn + merc.maxSpell + nrm;
		}
		else {
			buf1 = "Channeler: " + cyn + capFirstLetter(yesNo(merc.chan)) + nrm;
		}
		//--READD WHEN CHANNIES GO IN-- this.send(buf1);
		buf2 = "";
		buf1 = "\n[" + grn + "H" + nrm + "] Hire";
		for( var i = buf1.length - 1; i < 46; i++) {
			buf2 += " ";
		}
		this.send(buf1 + buf2 + "Price: " + this.numCopperToText(merc.price(),true) + nrm);
	}
	setSval(this,-18600,"savedListPage",currPage);
	var firstEntryNumOnPage = (currPage*2)-1;
	var lastEntryNumOnPage = firstEntryNumOnPage+1;
	var pagingPrompt = "[" + grn + "M" + nrm + "] Main Menu\t\t[" + grn + "N" + nrm + "] Next Page\n[" + grn + "R" + nrm + "] Review the Page\t[" + grn + "P" + nrm + "] Previous Page\n[" + grn + "C" + nrm + "] Close";
	this.send("\n" + BUFFER + "\n" + pagingPrompt);
	return;
}
//Eventually come up with a channeling script for mercs, allow them to turn "passive channeling" on

