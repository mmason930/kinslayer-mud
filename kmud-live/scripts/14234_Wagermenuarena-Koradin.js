var script14234 = function(self, actor, here, args, extra)
{
	getCharCols(actor);
var vArgs = getArgList(args);
_block;
var menu = getSval(actor,14209,"wagerMenu");
if (global.arenaFreeForAll == true)
	var name = "Someone";
else
	var name = "A member of the "+getTeamName(actor.arenaTeam)+" Team";
if (actor == global.arenaHost)
	name = "The host";
if (strn_cmp(vArgs[0],"cancel",1)) {
	actor.send("You stop using the wager menu.");
	setSval(actor,14209,"wagerMenu",0);
	actor.detach(14234);
	return;
}
var existingWager = false;
//Check to see if actor has a wager already
for each (var bet in global.arenaCurrentWagers) {
	if (bet[0] == actor)
		existingWager = true;
}
if (menu == 0) { // main betting menu
	if (global.arenaMixedBets == false && global.arenaHost != actor) {
		var hostWager = getWager(global.arenaCurrentWagers[0][0]);
		var hostWagerType = global.arenaCurrentWagers[0][2];
		var hostWagerAmount = global.arenaCurrentWagers[0][3];
		if (strn_cmp(vArgs[0],"yes",1)) {
			global.arenaCurrentWagers.push([actor,actor.arenaTeam,hostWagerType,hostWagerAmount]);
			arenaEcho(name+" has matched the host's wager of "+hostWager+"!");
			setSval(actor,14209,"wagerMenu",0);
			actor.detach(14234);
			return;
		}
		else if (strn_cmp(vArgs[0],"no",1)) {
			actor.comm("cancel");
			return;
		}
		else {
			actor.send("Valid choices are "+mag+"Y"+nrm+"es or "+mag+"N"+nrm+"o. Please try again.");
			return;
		}
	}
	if (strn_cmp(vArgs[0],"remove",1)) {
		for (var i=0; i < global.arenaCurrentWagers.length; i++) {
			if (global.arenaCurrentWagers[i][0] == actor) {
				global.arenaCurrentWagers.splice(i,1);
				arenaEcho(name+" has removed a wager!");
				if (global.arenaMixedBets == false) {
					global.arenaCurrentWagers = [];
					arenaEcho("All wagers have been erased.");
				}
				for each (var pers in getWaitingPlayers("pc"))
					pers.detach(14234);
				setSval(actor,14209,"wagerMenu",0);
				actor.detach(14234);
				return;
			}
		}
		actor.send(red+"You have no current wager to remove."+nrm);
		getWagerMenu(actor);
		return;
	}
	if (global.arenaFreeForAll == true)
		var top = 4;
	else
		var top = 3;
	if (vArgs[0] < 1 || vArgs[0] > top || isNaN(vArgs[0])) {
		actor.send(red+"That is not a valid command."+nrm);
		getWagerMenu(actor);
		return;
	}
	setSval(actor,14209,"wagerMenu",vArgs[0]);
	getWagerMenu(actor);
	return;
}
else if (isNaN(vArgs[0])) {
	actor.send(red+"That is not a valid command."+nrm);
	getWagerMenu(actor);
	return;
}
else if (existingWager == true) {
	actor.send("You've already wagered "+getWager(actor)+"!");
	actor.send("Remove this wager before wagering something else.");
	setSval(actor,14209,"wagerMenu",0);
	getWagerMenu(actor);
	return;
}
else if (menu == 1) { //wagering wps
	var wps = vArgs[0];
	/** WEAVE POINTS ARE TEMPORARILY OUT OF ORDER **/
	actor.send(red+"Weave points cannot be wagered at this time. Check back later!"+nrm);
	setSval(actor,14209,"wagerMenu",0);
	getWagerMenu(actor);
	return;
	/****/
	if (wps > actor.weavePoints) {
		actor.send(red+"You don't have that many Weave Points!"+nrm);
		setSval(actor,14209,"wagerMenu",0);
		getWagerMenu(actor);
		return;
	}
	arenaEcho(name+" has wagered "+vArgs[0]+" Weave Points!");
	global.arenaCurrentWagers.push([actor,actor.arenaTeam,1,vArgs[0]]);
	setSval(actor,14209,"wagerMenu",0);
	actor.detach(14234);
	return;
}
else if (menu == 2) { //wagering fps
	var fps = vArgs[0];
	if (fps > actor.featPoints) {
		actor.send(red+"You don't have that many Feat Points!"+nrm);
		setSval(actor,14209,"wagerMenu",0);
		getWagerMenu(actor);
		return;
	}
	arenaEcho(name+" has wagered "+vArgs[0]+" Feat Points!");
	global.arenaCurrentWagers.push([actor,actor.arenaTeam,2,vArgs[0]]);
	setSval(actor,14209,"wagerMenu",0);
	actor.detach(14234);
	return;
}
else if (menu == 3) { //wagering gold
	var gold = vArgs[0];
	if (gold > actor.gold) {
		actor.send(red+"You don't have that much Gold!"+nrm);
		setSval(actor,14209,"wagerMenu",0);
		getWagerMenu(actor);
		return;
	}
	arenaEcho(name+" has wagered "+nrm+actor.numCopperToText(vArgs[0],false)+mag+"!"+nrm);
	global.arenaCurrentWagers.push([actor,actor.arenaTeam,3,vArgs[0]]);
	setSval(actor,14209,"wagerMenu",0);
	actor.detach(14234);
	return;
}
else if (menu == 4) { //wagering item
	if (actor.inventory.length == 0) {
		actor.send(red+"You don't have any items in your inventory to bet."+nrm);
		setSval(actor,14209,"wagerMenu",0);
		getWagerMenu(actor);
		return;
	}
	for (var i=0; i<actor.inventory.length; i++) {
		if (vArgs[0] == (i+1)) {
			arenaEcho(name+" has wagered "+nrm+actor.inventory[i].name+mag+"!"+nrm);
			global.arenaCurrentWagers.push([actor,actor.arenaTeam,4,actor.inventory[i]]);
			setSval(actor,14209,"wagerMenu",0);
			actor.detach(14234);
			return;
		}
	}
	actor.send(red+"That is not a valid command."+nrm);
	getWagerMenu(actor);
	return;
}




};

