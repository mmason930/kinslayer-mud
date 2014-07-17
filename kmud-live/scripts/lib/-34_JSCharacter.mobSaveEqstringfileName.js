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
	fwrite(fileName ,BUFFER + invAndContBuffer); 
	//this.say("fileName: " + fileName + " invAndCont: " + fread(fileName));
}
