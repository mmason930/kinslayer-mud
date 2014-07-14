var script207 = function(self, actor, here, args, extra)
{
	//	June 2010 - Fogel
//	Allows a clanned human, fade or dreadlord to issue a horse
if( getSval( self, 207, "issuing" ) == 1 )
	return;
setSval( self, 207, "issuing", 1 );
var argv = args.split(" ");
var horse = getSval( actor, 207, "horse" );
var horseVnum;
if( horse )
{
	waitpulse 2;
	self.comm("say You already have one issued!");
}
else if( actor.race == constants.RACE_TROLLOC && !( actor.class == constants.CLASS_FADE || actor.class == constants.CLASS_DREADLORD) )
{
	waitpulse 2;
	self.comm("say What would you need a horse for?");
}
else if( actor.race == constants.RACE_HUMAN && !actor.isClanned() )
{
	waitpulse 2;
	self.comm("say You need to be in a clan to issue a horse!");
}
else if( actor.affectedBy(constants.AFF_NOQUIT) )
{
	waitpulse 2;
	self.comm("say Calm down first, you'll spook the horses!");
}
else if( argv[1] == "horse" && !horse )
{
	if( actor.name == "Otar" ) {
		horseVnum = 9996;
	}
	else if( actor.class == constants.CLASS_OGIER ) {
		horseVnum = 205;
	}
	else {
		horseVnum = ( actor.race == constants.RACE_TROLLOC ) ? 9998 : 201;
	}
	waitpulse 4;
	self.comm("say Right away, " + actor.name + "!");
	waitpulse 4;
	if( actor.room != self.room )
	{
		self.say("I can't issue a horse to someone who isn't here...");
	}
	else
	{
		horse = here.loadMob(horseVnum);
		setSval( actor, 207, "horse", horse );
	//	horse = here.people[0];
		echoaround( actor, self.name + " leads " + horse.name + " to " + actor.name + ".");
		actor.send( self.name + " leads " + horse.name + " up to you." );
		horse.comm("follow " + actor.name);
	}
}
setSval( self, 207, "issuing", 0 );

};

