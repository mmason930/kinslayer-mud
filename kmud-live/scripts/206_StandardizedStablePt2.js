var script206 = function(self, actor, here, args, extra)
{
	/**************************************************************************
 *                                                                        *
 * Standardized Horse Stabling                                            *
 *                                                                        *
 * ~~~ By: Galnor 06/06/2010                                              *
 *                                                                        *
 **************************************************************************/
var objectReceived = extra.obj;
var ticketMapper = global.stableTicketMapper;
var horseMapper = global.stableHorseMapper;
var iHorseVnum = objectReceived.getPval("mount");
if( iHorseVnum )
	iHorseVnum = parseInt(objectReceived.getPval("mount"));
else
	iHorseVnum = 201;//Default, for old tickets.
if( !horseMapper[ iHorseVnum ] || ticketMapper[ here.vnum ] != objectReceived.vnum ) {
	wait 1;
	self.say("Sorry, but we don't accept that here.");
	waitpulse 5;
	self.comm("emote returns the item given to " + self.him_her() + ".");
	objectReceived.moveToChar( actor );
	return;
}
if( iHorseVnum == 201 && getSval( actor, 207, "horse" ) )
{
	wait 1;
	self.say("You already have a horse issued!");
	waitpulse 5;
	self.comm("emote returns the item given to " + self.him_her() + ".");
	objectReceived.moveToChar( actor );
	return;
}
waitpulse 1;
objectReceived.extract();
wait 2;
self.say("Thanks! I'll be right out with your mount!");
wait 2;
var horse = here.loadMob( iHorseVnum );
self.comm("emote walks back out alongside " + horse.name + ".");
wait 1;
horse.comm("follow " + actor.name);

};

