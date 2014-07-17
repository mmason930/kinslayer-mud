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
	
}