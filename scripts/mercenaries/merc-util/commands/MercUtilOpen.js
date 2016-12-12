var MercUtil = (function(MercUtil) {

	MercUtil.performOpen = function(actor, target, vArgs, here, myMercObj)
	{
		if(vArgs.length < 2) {
			target.say("What am I trying to open?");
			return;
		}
		var found = 0; //haven't determined if it is hidden yet
		if(here.doorName_cmp(vArgs[1]) != -1){
			//there is a door by that name
			found = here.doorName_cmp(vArgs[1]); //found it as a door
			if(here.doorHidden(found) > 0) {

				found = 1;
			}
		}
		else {
			//maybe its a container???
			var posCont = getObjInListVis(target, vArgs[1], here.items);
			if(posCont != null && posCont.type == constants.ITEM_CONTAINER) {
				found = 10; //found it as a container
			}
		}
		if(found) {
			if(actor.gold < 40) {
				//Not enough cash!!!
				target.say("I'll need a bit more incentive to use my skillz...");
			}
			else {
				if(found != 10) {
					here.echo(capFirstLetter(target.name) + " tries to open " + vArgs[1] + ".");
				}
				if(found == 10) {
					here.echo(capFirstLetter(target.name) + " tries to open " + posCont.name + ".");
				}
				target.comm("open " + vArgs[1]);
				actor.send(capFirstLetter(target.name) + " takes " + ylw + "40" + nrm + " copper for " + target.hisHer() + " services.");
				actor.gold -= 40;
			}
		}
		else
			target.say("I dont see that...");
	};

	return MercUtil;
})(MercUtil || {});