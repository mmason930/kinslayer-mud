var script504 = function(self, actor, here, args, extra) {
	var KIT_KEY="NEWBIE_KIT";
	var vArgs = getArgList(args);
	if( str_cmp(vArgs[0],"kit") ) {
		if( vArgs.length < 3 || str_cmp(vArgs[1],"guardian") || str_cmp(vArgs[2],"kit") )
			return;
	}
	else
		_block;
	if( actor.level > 5 ) {
		actor.gsend("Only those level five and below may receive kits.");
		return;
	}
	if( actor.getPval(KIT_KEY) != null ) {
		actor.gsend("You've already received a kit.");
		return;
	}
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
			actor.loadObj(2040);
			actor.loadObj(2040);
			actor.loadObj(2060);
			actor.loadObj(2102);
			actor.loadObj(2102);
			actor.loadObj(2102);
			actor.loadObj(2021);
		}
		else if( actor.class == constants.CLASS_WARRIOR || actor.class == constants.CLASS_RANGER )
		{
			actor.loadObj(107);
			actor.loadObj(309);
			actor.loadObj(1318);
			actor.loadObj(408);
			actor.loadObj(507);
			actor.loadObj(1406);
			actor.loadObj(2040);
			actor.loadObj(2040);
			actor.loadObj(2060);
			actor.loadObj(2102);
			actor.loadObj(2102);
			actor.loadObj(2102);
			actor.loadObj(2021);
			actor.loadObj(1901);
		}
	}
	else if( actor.race == constants.RACE_TROLLOC )
	{
		if( actor.class == constants.CLASS_RANGER || actor.class == constants.CLASS_WARRIOR )
		{
			actor.loadObj(107);
			actor.loadObj(309);
			actor.loadObj(1318);
			actor.loadObj(1901);
			actor.loadObj(408);
			actor.loadObj(507);
			actor.loadObj(1406);
			actor.loadObj(2060);
			actor.loadObj(2104);
			actor.loadObj(2104);
			actor.loadObj(2104);
			actor.loadObj(2104);
			actor.loadObj(2026);
		}
		else if( actor.class == constants.CLASS_THIEF )
		{
			actor.loadObj(107);
			actor.loadObj(309);
			actor.loadObj(1406);
			actor.loadObj(408);
			actor.loadObj(507);
			actor.loadObj(2060);
			actor.loadObj(2104);
			actor.loadObj(2104);
			actor.loadObj(2104);
			actor.loadObj(2104);
			actor.loadObj(2026);
			actor.loadObj(1901);
		}
		else if( actor.class == constants.CLASS_DREADGUARD )
		{
			actor.loadObj(107);
			actor.loadObj(309);
			actor.loadObj(1406);
			actor.loadObj(408);
			actor.loadObj(507);
			actor.loadObj(2060);
			actor.loadObj(2104);
			actor.loadObj(2104);
			actor.loadObj(2104);
			actor.loadObj(2104);
			actor.loadObj(1901);
			actor.loadObj(2021);
		}
	}
	actor.gold += 75;
	actor.send( self.name + " gives you a starting kit." );
	echoaround( actor, actor.name + " is granted a kit." );
	actor.setPval(KIT_KEY, "1", true);
	
}