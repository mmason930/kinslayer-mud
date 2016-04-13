var script303 = function(self, actor, here, args, extra) {
	var KIT_KEY="NEWBIE_KIT";
	var vArgs = getArgList(args.toLowerCase());
	var type = null;
	if(vArgs.length >= 1)
	{
		if(vArgs[0] == "kit")
			type = "default";
		else if(vArgs[0] == "abs")
			type = "abs";
		else if(vArgs[0] == "dodge")
			type = "dodge";
		else if(vArgs[0] == "combo")
			type = "combo";
	}
	if(type == null)
	{
		//If we get here then they didn't enter in proper input.
		actor.gsend("If you would like a kit, tell me one of the following: `kit`, `abs`, `dodge`, or `combo`");
		return;
	}
	if (type != "default" && actor.level < 20) {
		actor.gsend("Attain level 20 before asking for a custom kit.");
		return;
	}
	var hasKit = actor.getPval(KIT_KEY);
	if( hasKit ) {
		actor.gsend("If you die, ask me again and I will grant you a kit.");
		return;
	}
	if( type == "dodge" ) {
		actor.loadObj(1010);
		actor.loadObj(1010);
		actor.loadObj(913);
		actor.loadObj(913);
		actor.loadObj(42);
		actor.loadObj(2006);
		actor.loadObj(304);
		actor.loadObj(296);
		actor.loadObj(102);
		actor.loadObj(601);
		actor.loadObj(707);
		actor.loadObj(707);
		actor.loadObj(1410);
		actor.loadObj(1901);
		actor.loadObj(200);
		actor.loadObj(807);
		actor.loadObj(807);
		actor.loadObj(400);
		actor.loadObj(503);
	}
	else if( type == "abs" ) {
		actor.loadObj(1011);
		actor.loadObj(1011);
		actor.loadObj(23);
		actor.loadObj(2089);
		actor.loadObj(322);
		actor.loadObj(20112);
		actor.loadObj(112);
		actor.loadObj(623);
		actor.loadObj(705);
		actor.loadObj(705);
		actor.loadObj(211);
		actor.loadObj(804);
		actor.loadObj(804);
		actor.loadObj(415);
		actor.loadObj(511);
		actor.loadObj(1312);
	}
	else if( type == "combo" ) {
		actor.loadObj(1010);
		actor.loadObj(1010);
		actor.loadObj(913);
		actor.loadObj(913);
		actor.loadObj(36);
		actor.loadObj(2009);
		actor.loadObj(326);
		actor.loadObj(20105);
		actor.loadObj(104);
		actor.loadObj(618);
		actor.loadObj(704);
		actor.loadObj(704);
		actor.loadObj(216);
		actor.loadObj(808);
		actor.loadObj(808);
		actor.loadObj(419);
		actor.loadObj(519);
		actor.loadObj(1318);
		actor.loadObj(1901);
	}
	else if(type == "default") {
		if( actor.race == constants.RACE_HUMAN )
		{
			if( actor.class == constants.CLASS_CHANNELER || actor.class == constants.CLASS_THIEF )
			{
				actor.loadObj(107);
				actor.loadObj(309);
				actor.loadObj(1406);
				actor.loadObj(408);
				actor.loadObj(507);
				actor.loadObj(1901);
			}
			else if( actor.class == constants.CLASS_WARRIOR || actor.class == constants.CLASS_RANGER )
			{
				actor.loadObj(107);
				actor.loadObj(309);
				actor.loadObj(408);
				actor.loadObj(507);
				actor.loadObj(1406);
				actor.loadObj(1901);
			}
		}
		else if( actor.race == constants.RACE_TROLLOC )
		{
			if( actor.class == constants.CLASS_RANGER || actor.class == constants.CLASS_WARRIOR )
			{
				actor.loadObj(107);
				actor.loadObj(309);
				actor.loadObj(1901);
				actor.loadObj(408);
				actor.loadObj(507);
				actor.loadObj(1406);
			}
			else if( actor.class == constants.CLASS_THIEF )
			{
				actor.loadObj(107);
				actor.loadObj(309);
				actor.loadObj(1406);
				actor.loadObj(408);
				actor.loadObj(507);
				actor.loadObj(1901);
			}
			else if( actor.class == constants.CLASS_DREADGUARD )
			{
				actor.loadObj(107);
				actor.loadObj(309);
				actor.loadObj(1406);
				actor.loadObj(408);
				actor.loadObj(507);
				actor.loadObj(1901);
			}
		}
	}
	
	if(actor.race == constants.RACE_HUMAN) {
		actor.loadObj(2040);//Wooden torch
		actor.loadObj(2040);//Wooden torch
		actor.loadObj(2021);//Light leather flask
	}
	else {
		actor.loadObj(2026);//Flask of human skin
	}
	actor.loadObj(2060);//Bag
	actor.loadObj(2102);//Meat
	actor.loadObj(2102);//Meat
	actor.loadObj(2102);//Meat
	actor.gold += 75;
	
	actor.gsend("Use this kit wisely, " + actor.name + ".");
	act("$N gives you a kit.", false, actor, null, self, constants.TO_CHAR);
	act("$N gives $n a kit.", false, actor, null, self, constants.TO_ROOM);
	actor.setPval(KIT_KEY, time());
	
	
	
	
	
	
	
}