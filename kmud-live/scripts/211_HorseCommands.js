var script211 = function(self, actor, here, args, extra)
{
	//	June 2010 - Fogel
//	Commands meant for horses are to be placed in this script.
var vArgs = getArgList(args);
//	No target, so let's get out now to avoid script errors
if( !vArgs[1] )
	return;
	
var owner = getSval( self, 207, "owner" );
var specific = getCharInListVis(actor,vArgs[1],here.people);
if(specific)
{
	if( specific != self )
	{
		_noblock;
		return;//Let other mob try.
	}
}
if( owner && owner.isValid && actor != owner )
{
	if( strn_cmp(vArgs[0].toLowerCase(), "lead", 2)  && actor.getSkillLevel("Ride") < 7 )
	{
		_block;
		actor.send( "You aren't skilled enough to lead a horse you don't own!" );
	}
	else if( strn_cmp(vArgs[0].toLowerCase(), "ride", 2) )
	{
		_block;
		actor.send( "You try to mount " + self.name + ", but it won't let you on!" );
	}
	return;
}
else if( !isName(vArgs[1], self.namelist) || !vArgs[2] )
{
	_noblock;
	return;
}
else if( strn_cmp(vArgs[2].toLowerCase(), "flee", 2) && actor.getSkillLevel("Ride") >= 4 )
{
	_block;
	actor.send("You order your mount to flee.");
	echoaround(actor, actor.name + " gives " + self.name.toLowerCase() + " some orders.");
	self.comm("flee");
	return;
}
else if( strn_cmp(vArgs[2].toLowerCase(), "unfollow", 3) )
{
	_block;
	actor.send("You order your mount to stop following.");
	echoaround(actor, actor.name + " gives " + self.name.toLowerCase() + " some orders.");
	self.comm("follow self");
	return;
}
else if( strn_cmp(vArgs[2].toLowerCase(), "dance", 2) )
{
	_block;
	actor.send("You order your mount to dance.");
	echoaround(actor, actor.name + " gives " + self.name.toLowerCase() + " some orders.");
	self.comm("dance");
	return;
}
else if( actor == owner && strn_cmp(vArgs[2].toLowerCase(), "return", 3) && strn_cmp(vArgs[3].toLowerCase(), "home", 2) )
{
	_block;
	actor.send("You order your mount to return home.");
	echoaround(actor, actor.name + " gives " + self.name.toLowerCase() + " some orders.");
	waitpulse 1;
	echoaround(self, self.name + " gallops back to " + self.his_her() + " stable.");
	self.extract();
	return;
}

};

