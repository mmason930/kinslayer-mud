var script20869 = function(self, actor, here, args, extra) {
	for each (var person in actor.room.people) {
		for each (var pName in person.namelist.split(" ")) {
			if (pName == "dude") {
				var apprentice = person;
				break;
			}
		}
	}
	if (!apprentice) {
		actor.detach(20869);
		actor.send("The transaction is canceled because the apprentice has disappeared!");
		return;
	}
	var pack = apprentice.eq(constants.WEAR_BACK);
	if (!pack || pack.contents.length == 0) {
		actor.send("The transaction is canceled because the apprentice has given back all the items!");
		actor.detach(20869);
		return;
	}
	var itemList = pack.contents;
	_block;
	getCharCols(actor);
	var vArgs = getArgList(args);
	if (strn_cmp(vArgs[0],"cancel",1)) {
		actor.send("You cancel the transaction.");
		actor.detach(20869);
		return;
	}
	for (i=0; i< itemList.length; i++) {
		if (vArgs[0] == (i+1)) {
			if (actor.gold < Math.floor(itemList[i].cost*getCostMultiplier(itemList[i].name))) {
				actor.send(red+capFirstLetter(apprentice.name) + " tells you 'You can't afford that!'"+nrm);
				actor.send(" ");
				actor.detach(20869);
				return;
			}
			else {
				actor.gold -= Math.floor(itemList[i].cost*getCostMultiplier(itemList[i].name));
				apprentice.say("Excellent choice, "+actor.name+"!");
				apprentice.comm("get "+itemList[i].namelist.split(" ")[0]+" pack");
				while (apprentice.inventory[0] != itemList[i])
					apprentice.comm("get "+itemList[i].namelist.split(" ")[0]+" pack");
				apprentice.inventory[0].moveToChar(actor);
				actor.send(capFirstLetter(apprentice.name)+ " gives you "+itemList[i].name+".");
				act("$n gives $N "+itemList[i].name+".",true,apprentice,null,actor,constants.TO_NOTVICT);
				apprentice.comm("put all pack");
				actor.detach(20869);
				return;
			}
		}
	}
	actor.send("That is not a valid choice.");
	actor.send("Choose another option, or type "+cyn+"C"+nrm+" to cancel the transaction.");
	return;
	
}