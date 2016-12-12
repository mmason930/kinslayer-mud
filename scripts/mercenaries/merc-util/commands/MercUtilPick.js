var MercUtil = (function(MercUtil) {

	MercUtil.performPick = function(actor, target, vArgs, here, myMercObj)
	{
		if(myMercObj.className != constants.CLASS_THIEF && myMercObj.skillsList[myMercObj.skill] != "Pick") {
			target.say("I don't know how to do that...");
			return;
		}
		if(!arg2) {
			target.say("What am I trying to pick?");
			return;
		}
		var found = 0; //haven't found anything to pick yet...
		if(here.doorName_cmp(arg2) != -1){
			//there is a door by that name
			found = 1; //found it as a door
		}
		else {
			//maybe its a container???
			var posCont = getObjInListVis(target, arg2, here.items);
			if(posCont != null && posCont.type == constants.ITEM_CONTAINER) {
				found = 2; //found it as a container
			}
		}
		if(found)
		{
			if(actor.gold < 60) //Not enough cash!!!
				target.say("I'll need a bit more incentive to use my skills...");
			else
			{
				if(found == 1)
					here.echo(capFirstLetter(target.name) + " tries to pick " + arg2 + ".");
				if(found == 2)
					here.echo(capFirstLetter(target.name) + " tries to pick " + posCont.name + ".");
				target.comm("pick " + arg2);
				actor.send(capFirstLetter(target.name) + " takes " + ylw + "60" + nrm + " copper for " + target.hisHer() + " services.");
				actor.gold -= 60;
			}
		}
		else
			target.say("I don't see that...");
	};

	return MercUtil;
})(MercUtil || {});
