var MercUtil = (function(MercUtil) {

	MercUtil.performReport = function(actor, target, vArgs, here, myMercObj)
	{
		actor.send("\n" + capFirstLetter(target.name) + "'s Status Report:");
		if(myMercObj.chan) {
			actor.send("I have " + cyn + target.hps + nrm + "(" + cyn + target.maxHit + nrm + ") hit, " + cyn + target.sps + nrm + "(" + cyn + target.maxSpell + nrm + ") mana and " + cyn + target.mvs + nrm + "(" + cyn + target.maxMove + nrm + ") movement points.");
		}
		else {
			actor.send("I have " + cyn + target.hps + nrm + "(" + cyn + target.maxHit + nrm + ") hit and " + cyn + target.mvs + nrm + "(" + cyn + target.maxMove + nrm + ") movement points.");
		}
		actor.send("Offensive Bonus: " + cyn + target.offense + nrm + " Dodge Bonus: " + cyn + target.dodge + nrm + ", Parry Bonus: " + cyn + target.parry + nrm);
		actor.send("My abilities are: Str: " + cyn + target.strength + nrm + ", Int: " + cyn + target.intelligence + nrm + ", Wis: " + cyn + target.wisdom + nrm + ", Dex: " + cyn + target.dexterity + nrm + ", Con: " + cyn + target.constitution + nrm);
		actor.send("My armor absorbs " + cyn + target.absorb + nrm + "%% damage on average.");
		var rendRoom = getSval(target,18605,"mercRendezvousRoom");
		if(rendRoom && rendRoom != "") {
			actor.send("My rendezvous point: " +  cyn + rendRoom.name + nrm + ".");
		}
		if(myMercObj.skill > -1) {
			actor.send("My skill: " + cyn + myMercObj.skillsList[myMercObj.skill] + nrm + ".");
		}
	};

	return MercUtil;
})(MercUtil || {});
