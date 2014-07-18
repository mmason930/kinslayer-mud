JSCharacter.prototype.__defineGetter__("hisher", function () {
	return (this.sex == constants.SEX_MALE) ? "his" : "her";
});

JSCharacter.prototype.isGroupedWith = function (groupedCharacter) {
	if (this.leader == groupedCharacter)
		return true;
	var followers = this.followers;
	for (var followerIndex = 0; followerIndex < followers.length; ++followerIndex) {
		if (followers[ followerIndex ] == groupedCharacter)
			return true;
	}
	return false;
};

JSCharacter.prototype.faceoff = function (vict) {
	var target;
	if (!vict.fighting || vict.affectedBy(constants.AFF_GROUP)) {
		return vict;
	}
	for (var i = 0; i < vict.room.people.length; i++) {
		target = vict.room.people[i];
		if ((!target.leader && !vict.leader) || !target.affectedBy(constants.AFF_GROUP)) {
			continue;
		}
		if (this.canSee(target) && (target.leader == vict.leader || target == vict.leader || target.leader == vict)) {
			if (target.numFighting() < vict.numFighting() && !target.mobFlagged(constants.MOB_MOUNT) && !target.mobFlagged(constants.MOB_SHADOW_MOUNT)) {
				return target;
			}
		}
	}
	return vict;
};

JSCharacter.prototype.numFighting = function () {
	var count = 0;
	var victim;
	for (var i = 0; i < this.room.people.length; i++) {
		victim = this.room.people[i];
		if (victim.fighting == this) {
			count++;
		}
	}
	return count;
};

// Alder
// June 2011
/***************************************************************
 * Parameter 1: String or Array of Strings representing the    *
 * names of scalps to look for                                 *
 * NOTE: use the mob's name, not the name of the scalp. EX:    *
 * If you want to check for Easar's scalp, you need to pass    *
 * "King Easar", NOT "a bloody scalp of King Easar"            *
 * Parameter 2: desired race constant of returned scalps       *
 * Returns: Array of scalp objects that match the parameter    *
 ***************************************************************/
JSCharacter.prototype.findScalpsByNameAndRace = function ( names, scalpRace ) {
	if ( !Array.isArray(names) ) {
		names = [names];
	}
	if ( isString(scalpRace) ) {
		scalpRace = parseRace(scalpRace);
	}
	var scalpNames = [];
	for each ( var name in names ) {
		scalpNames.push("a bloody scalp of "+name);
	}
	var matchScalps = [];
	var items = this.getAllItems();
	for each ( var item in items ) {
		if ( arrContains(scalpNames,item.name) && item.scalpRace == scalpRace ) {
			matchScalps.push(item);
		}
	}
	return matchScalps;
};

//Searches a characters list of worn items to find an item matching the objName provided
//Rhollor - December 2009
JSCharacter.prototype.getObjWorn = function(objName) {
	for(var pos = 0; pos < constants.NUM_WEARS; pos++) {
		var currentItem = this.eq(pos);
		if(currentItem) {
			if(isName(objName,currentItem.namelist)){
				return currentItem;
			}
		}
	}
	return null;
};

//This is a HIGHLY specialized script meant to be used with mercenaries ONLY, use else where without alteration
//Do NOT alter in anyway without knowing what you're doing!
//Rhollor Dec 09
JSCharacter.prototype.mobSaveEq = function(fileName) {
	if(this.vnum == -1) {
		mudLog(3,101,"JSCRIPT ERR: Script -34 attempted to execute a Mobile only script on a PC!");
		return 1;
	}
	//The max amount of items to be saved in their inventory & containers
	var maxInInvAndCont = 100;
	var invAndCont = [maxInInvAndCont];
	//Index for invAndCont array;
	var invAndContCount = 0;
	var invAndContBuffer = "";
	var BUFFER = ""; //Master buffer
	for(var pos = 0; pos < constants.NUM_WEARS; pos++) {
		var currentItem = this.eq(pos);
		if(currentItem) {
			//array of worn items delimit by " "
			BUFFER += (currentItem.vnum + " ");
			//Read worn conainers
			if(currentItem.type == constants.ITEM_CONTAINER || currentItem.extraFlags(constants.ITEM_DAGGER_SHEATH) || currentItem.extraFlags(constants.ITEM_SWORD_SHEATH) ) {
				var container = currentItem.contents;
				if(container.length != 0) {
					for(var j = 0; j < container.length && invAndContCount <= maxInInvAndCont; j++) {
						//adding to array of vnums for invAndCont delimit by " "
						invAndContBuffer += (container[j].vnum + " ");
						invAndContCount++;
					}
				}
			}
		}
	}
	//Read inventory
	if(this.inventory.length != 0) {
		for(j = 0; j < this.inventory.length && invAndContCount <= maxInInvAndCont; j++) {
			currentItem = this.inventory[j];
			//adding to array of vnums for invAndCont delimit by " "
			invAndContBuffer += (currentItem.vnum + " ");
			invAndContCount++;
			//Reading containers in inventory
			if(currentItem.type == constants.ITEM_CONTAINER || currentItem.extraFlags(constants.ITEM_DAGGER_SHEATH) || currentItem.extraFlags(constants.ITEM_SWORD_SHEATH) ) {
				container = currentItem.contents;
				if(container.length != 0) {
					for(var k = 0; k < container.length && invAndContCount <= maxInInvAndCont; k++) {
						//adding to array of vnums for invAndCont delimit by " "
						invAndContBuffer += (container[k].vnum + " ");
						invAndContCount++;
					}
				}
			}
		}
	}
	//this file contains <= maxInInvAndCont vnums that appear in inventory and containers
	fwrite(fileName, BUFFER + invAndContBuffer);
};

/*******************************************************************
 *                                                                 *
 * JSCharacter.getItems()                                          *
 * Arguments: itemList - an array of items being searched.         *
 *                        element is either vnum of object sdesc   *
 *		   checkInv - If true, checks char's inventory             *
 *         checkEQ  - If true, checks char's equipment             *
 *		   recursive- If true, checks CONTENTS of eq/inv           *
 *                                                                 *
 * Returns: An array of matching items                             *
 * Purpose: Searches a character for certain items, returning all  *
 *          items that match the specified criteria                *
 *                                                                 *
 * ~~~ By: Galnor 01/13/2010                                       *
 *                                                                 *
 *******************************************************************/
JSCharacter.prototype.getItems = function(itemList, checkInv, checkEQ, recursive)
{
	if( getObjectClass(itemList) != "Array" )
		itemList = [itemList];
	var objArr = [];
	var cInv = this.inventory;

	if( checkInv )
	{
		for(var i in cInv)
		{
			for each(var tObj in itemList)
			{
				if( (getObjectClass(tObj) == "Number" && cInv[i].vnum == tObj) )
					objArr.push( cInv[i] );
				else if( cInv[i].name == tObj )
					objArr.push( cInv[i] );
			}
			if( recursive && cInv[i].type == constants.ITEM_CONTAINER)
				objArr = objArr.concat( cInv[i].getItems(itemList, true) );
		}
	}
	if( checkEQ )
	{
		for(var i = 0;i < 22;++i)
		{
			var eObj = this.eq(i);
			if( !eObj )
				continue;
			for each(var tObj in itemList)
			{
				if( (getObjectClass(tObj) == "Number" && eObj.vnum == tObj) )
					objArr.push( eObj );
				else if( eObj.name == tObj )
					objArr.push( eObj );
			}
			if( recursive && eObj.type == constants.ITEM_CONTAINER )
				objArr = objArr.concat( eObj.getItems(itemList, true) );
		}
	}
	return objArr;
};

// Rhollor January 2010
// This will take an int numCopper and convert it to text
// Sample: numCopper = 12500 -> "1 gold, 25 silver"
// dispFull is an optional boolean specifying whether or not the output will display placeholders:
// Ex. "0 gold, 32 silver, 0 copper"
JSCharacter.prototype.numCopperToText = function(numCopper, color, dispFull) {
	var nrm = this.normal(constants.CL_OFF);
	if(color) {
		var ylw = this.yellow(constants.CL_OFF);
		var cyn = this.cyan(constants.CL_OFF);
		var bld = this.bold(constants.CL_OFF);
	}
	else {
		var ylw = "";
		var cyn = "";
		var bld = "";
		var nrm = "";
	}
	var str = "";
	var gold, silver, copper;
	gold = Math.floor(numCopper/10000);
	numCopper = numCopper%10000;
	silver = Math.floor(numCopper/100);
	numCopper = numCopper%100;
	copper = numCopper;
	if(gold > 0 || dispFull) {
		str += " " + gold + bld + ylw + " gold" + nrm;
	}
	if(silver > 0 || dispFull) {
		if(gold > 0 || dispFull) {
			str += ",";
		}
		str += " " + silver + bld + cyn + " silver" + nrm;
	}
	if(copper >= 0 || dispFull) {
		if(gold > 0 || silver > 0 || dispFull) {
			if(copper <= 0 && !dispFull) {
				return str;
			}
			str += ",";
		}
		str += " " + copper + ylw + " copper" + nrm;
	}
	return str;
};

//JSCharacter.heal(int hps) heals the JSCharacter hps amount
//Rhollor - January 2010
JSCharacter.prototype.heal = function(hps) {
	this.hps += hps;
	if(this.hps > this.maxHit) {
		this.hps = this.maxHit;
	}
	return;
};

JSCharacter.prototype.isMount = function()
{
	return this.mobFlagged(constants.MOB_MOUNT) || this.mobFlagged(constants.MOB_SHADOW_MOUNT) || this.mobFlagged(constants.MOB_OGIER_MOUNT);
};

JSCharacter.prototype.isHorse = JSCharacter.prototype.isMount;

//Alder January 2010
/**************************************************
 * actor.findClans()                              *
 * Args: if returnArray is true, the function     *
 * will always return an array.                   *
 * Returns: if actor is in one clan, returns that *
 * clan number. If they are in multiple, returns  *
 * array of clan numbers.                         *
 * If actor is unclanned, returns -1              *
 *                                                *
 **************************************************/
JSCharacter.prototype.findClans = function(returnArray) {
	var clanNumArray = [];
	for ( var i = 1; i <= 30; i++ ) {
		if ( this.inClan(i) == true ) {
			clanNumArray.push(i);
		}
	}
	if ( !clanNumArray.length ) {
		return -1;
	}
	if( returnArray ) {
		return clanNumArray;
	}
	return ( clanNumArray.length > 1 && this.vnum == -1 ? clanNumArray : clanNumArray[0] );
};

//Alder January 2010
/***************************************************
 * Returns true if actor belongs to any clans in   *
 * arrClanNums (-1 is unclanned). False otherwise. *
 ***************************************************/
JSCharacter.prototype.clanCheck = function( arrClanNums ) {
	if ( getObjectClass(arrClanNums) != "Array" ) {
		arrClanNums = [arrClanNums];
	}
	if ( !arrClanNums.length || arrClanNums[0] == -1 ) {
		return true;
	}
	var clanCheck = false
	var clans = this.findClans(true);
	for ( var i = 0; i < clans.length; i++ ) {
		var num = clans[i];
		if ( arrContains(arrClanNums,num) ) {
			clanCheck = true;
			break;
		}
	}
	return ( clanCheck );
};

JSCharacter.prototype.channelingAbility = function() {
	if( this.class == constants.CLASS_DREADLORD || this.class == constants.CLASS_CHANNELER || this.class == constants.CLASS_DREADGUARD )
		return true;
	return false;
};

//	June 2010 - Fogel
//	Returns if a character is clanned
JSCharacter.prototype.isClanned = function()
{
	for( var i = 0; i < constants.CLANS_MAX; i++ )
		if( this.inClan(i) )
			return true;
	return false;
};

//	June 2010 - Fogel
//	Returns the level of the character's skill
JSCharacter.prototype.getSkillLevel = function( skillName )
{
	return Math.floor( this.getSkill( getSkillVnum(skillName) ) / 12 );
};

JSCharacter.prototype.hasItem = function(item_vnum, check_inv, check_eq, recursive)
{
	if( check_inv ) {
		for(var i in this.inventory) {
			if( (getObjectClass(item_vnum) == "Number" && this.inventory[i].vnum == item_vnum) )
				return this.inventory[i];
			else if( this.inventory[i].name == item_vnum )
				return this.inventory[i];
			if( recursive ) {
				var obj = this.inventory[i].hasItem(item_vnum, true);
				if( obj )
					return obj;
			}
		}
	}
	if( check_eq ) {
		for(var i = 0;i < 22;++i) {
			if( !this.eq(i) )
				continue;
			if( (getObjectClass(item_vnum) == "Number" && this.eq(i).vnum == item_vnum) )
				return this.eq(i);
			else if( this.eq(i).name == item_vnum )
				return this.eq(i);
			if( recursive ) {
				var obj = this.eq(i).hasItem(item_vnum, true);
				if( obj )
					return obj;
			}
		}
	}
	return null;
};

JSCharacter.prototype.heShe = function()
{
	var he_she = ["it", "he", "she"];
	if ( this.sex < 0 || this.sex >= he_she.length )
	{
		return undefined;
	}
	return he_she[this.sex];
};

JSCharacter.prototype.himHer = function()
{
	var him_her = ["it", "him", "her"];
	if ( this.sex < 0 || this.sex >= him_her.length )
	{
		return undefined;
	}
	return him_her[this.sex];
};

JSCharacter.prototype.hisHer = function()
{
	var hArr = ["its", "his", "her"];
	if ( this.sex < 0 || this.sex >= hArr.length )
	{
		return undefined;
	}
	return hArr[this.sex];
};

JSCharacter.prototype.__defineGetter__("eqList", function()
	{
		var eq = [];
		for(var i = 0;i < constants.NUM_WEARS;++i) {
			if( !this.eq( i ) ) continue;
			eq.push( this.eq( i ) );
		}
		return eq;
	}
);

JSCharacter.prototype.__defineGetter__("totalQuestPoints",
	function()
	{
		var vClans = this.findClans(true);
		if( vClans.length == 0 ) return 0;

		var totalQP = 0;
		for(var i = 0;i < vClans.length;++i) {
			totalQP += this.qp( vClans[ i ] );
		}
		return totalQP;
	}
);

//Alder
//November 2010
/**********************************************
 * Temporary killcount getter for JSCharacter *
 **********************************************/
JSCharacter.prototype.killCount = function() {
	if ( this.vnum == -1 ) {
		var rs = sqlQuery("SELECT pks FROM users WHERE user_id="+sqlEsc(this.id)+";");
		return parseInt(rs.getRow.get("pks"));
	}

	return 0;
};

//Alder
//April 2010
/*****************************************
 * Awards actor intExp experience points *
 *****************************************/
JSCharacter.prototype.exp = function ( intExp ) {
	this.experience += parseInt(intExp);
};

//Alder
//February 2011
/**************************************************************
 * JSCharacter.findPlayerScalps ( scalpRace, minLevel )       *
 * scalpRace: integer or string representing race of scalp    *
 * minLevel: integer representing minimum level of scalp      *
 * Returns: array of scalp objects matching given criteria    *
 **************************************************************/

JSCharacter.prototype.findPlayerScalps = function( scalpRace, minLevel ) {
	// If scalpRace is given as String, convert to number
	if ( getObjectClass(scalpRace) == "String" ) {
		scalpRace = parseRace(scalpRace,true);
	}
	// If minLevel isNaN, display error
	if ( !isNumber(minLevel ) ) {
		mudLog(constants.CMP,102,"Value passed as minLevel isNaN: JSCharacter.findPlayerScalps() :: JS -9800");
		return null;
	}
	minLevel = parseInt(minLevel);

	var matchingScalps = [];				// Holds scalps that match parameters
	var possessions = this.getAllItems();	// Every item on player

	for each ( var item in possessions ) {
		if ( item.isPlayerScalp && item.scalpRace == scalpRace && item.scalpLevel >= minLevel ) {
			matchingScalps.push(item);		// This item is a scalp matching specified criteria
		}
	}
	return matchingScalps;
};

//Alder
//May 2010
/**************************************************************
 * Returns: true if actor is in void, false if not            *
 **************************************************************/
JSCharacter.prototype.isIdle = function() {
	if ( this.idle >= 5 )
		return true;
	return false;
};

//Alder
//May 2010
/**************************************************************
 * strTitle: the title string to look for (Case sensitive)    *
 * Returns: true if player has a title which matches strTitle *
 *          exactly                                           *
 **************************************************************/
JSCharacter.prototype.checkTitle = function ( strTitle ) {
	var strTitle = strTitle+"";
	var titles = this.titles;
	for each ( var title in titles ) {
		if ( title.title == strTitle ) {
			return true;
		}
	}
	return false;
};
