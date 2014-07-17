var script205 = function(self, actor, here, args, extra) {
	/**************************************************************************
	 *                                                                        *
	 * Standardized Horse Stabling                                            *
	 *                                                                        *
	 * ~~~ By: Galnor 06/06/2010                                              *
	 *                                                                        *
	 **************************************************************************/
	if( !str_cmp(args,"stable my horse") || !str_cmp(args,"stable my horse.") )
	{
		var ticketMapper = global.stableTicketMapper;
		var horseMapper = global.stableHorseMapper;
		var mount = actor.mount;//Cache it in case they dismount.

		if( !ticketMapper[ here.vnum ] ) {
			self.say("Sorry, but we're in maintenance mode right now.");
			return;
		}
		if( !mount ) {
			self.say("You aren't even riding anything.");
			return;
		}
		var iCost = horseMapper[ mount.vnum ];
		var iTicketVnum = ticketMapper[ here.vnum ];
		var iMountVnum = mount.vnum;

		if( actor.gold <= iCost ) {
			self.say("You do not have enough money for that.");
			return;
		}
		if( !iTicketVnum ) {
			self.say("I do not stable that kind of mount.");
			return;
		}
		actor.gold -= iCost;
		waitpulse 25;
		self.say("Stand still for a moment.");
		waitpulse 25;
		actor.send("A stable boy leads your horse away to the stable.");
		act("$n leads $N's horse away to the stable.", true, self, null, actor, constants.TO_NOTVICT);
		mount.extract();
		waitpulse 25;
		var ticket = self.loadObj( iTicketVnum );
		self.comm("give ticket " + actor.name);
		ticket.setPval("mount", iMountVnum);

		if(ticket.carriedBy != actor)
		{
			self.say("You can't seem to carry this ticket, so I'll just leave it here.");
			wait 1;
			self.comm("drop ticket");
		}
	}
};

var script206 = function(self, actor, here, args, extra) {
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