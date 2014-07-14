var script212 = function(self, actor, here, args, extra)
{
	//	June 2010 - Fogel
//	Script for purchasing horses from a horse vendor
_block;
var argv = args.split(" ");
var price = 0;
var mount;
if( argv[1] == "warhorse" )
{
	if( getSval( actor, 207, "horse" ) )
	{
		waitpulse 1;
		self.say("You've already issued a horse!");
		return;
	}
	price += 1500;
	mount = 201;
}
else if( argv[1] == "razor" )
{
	price += 1200;
	mount = 212;
}
else
{
	return;
}
if( actor.gold < price )
{
	wait 1;
	self.say("You don't have enough money!");
	return;
}
wait 2;
self.say( (( mount == 201 ) ? "A black warhorse" : "A Domani razor") + " it is, " + actor.name + "!");
wait 2;
mount = here.loadMob( mount );
self.comm("emote leads out " + mount.name + ".");
wait 1;
mount.comm("follow " + actor.name);
actor.gold -= price;

};

