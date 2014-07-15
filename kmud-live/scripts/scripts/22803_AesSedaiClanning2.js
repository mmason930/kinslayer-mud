var script22803 = function(self, actor, here, args, extra) {
	if( !actor.isAesSedai() )
		return;
	var vArgs = getArgList(args.toLowerCase());
	if( vArgs[0] == "accepted" && vArgs[2] == "has" && vArgs[3] == "arrived" && vArgs[4] == "for" && vArgs[5] == "her" && vArgs[6] == "test" && (vArgs[7] == "sister" || vArgs[7] == "sister.") )
	{
		wait 1;
		var accepted = getCharInListVis(self,vArgs[1],here.people);
		if( !accepted ) {
			self.say("Really? Because I do not see " + capFirstLetter(vArgs[1]) + " here.");
			return;
		}
		var mobIndex = new Object();
		mobIndex[ 22 ] = 22311;
		mobIndex[ 19 ] = 22308;
		mobIndex[ 23 ] = 22309;
		mobIndex[ 20 ] = 22312;
		mobIndex[ 17 ] = 22314;
		mobIndex[ 18 ] = 22307;
		mobIndex[ 21 ] = 22313;
		var targetRoom = getRoom(10574);
		var portalHere;
		var portalThere;
		
		for(var clanNum in mobIndex) {
			if( !actor.inClan( clanNum ) )
				targetRoom.loadMob( mobIndex[ clanNum ] );
		}
		wait 2;
		self.say("Now that you are here " + accepted.name + ", let us go to the testing chamber where the others are waiting.");
		wait 3;
		self.comm("embrace");
		wait 2;
		self.comm("emote weaves her hands before her, creating a shimmering portal.");
		portalHere = here.loadObj(10600);
		portalThere = targetRoom.loadObj(10600);
		targetRoom.echo("A shimmering portal appears.");
		wait 4;
		self.comm("emote motions for " + accepted.name + " to lead the way.");
		wait 4;
		accepted.send("You step through the portal.");
		act("$n steps into the portal.",false,accepted,null,null,constants.TO_ROOM);
		accepted.moveToRoom(targetRoom);
		act("$n steps through the portal.", false, accepted, null, null, constants.TO_ROOM);
		accepted.comm("look");
		wait 3;
		actor.send("You follow " + accepted.name + " through the portal.");
		actor.moveToRoom(targetRoom);
		actor.comm("look");
		act("$n steps through the portal.", false, actor, null, null, constants.TO_ROOM);
		wait 2;
		portalHere.extract();
		self.moveToRoom(targetRoom);
		act("$n steps through the portal.", false, self, null, null, constants.TO_ROOM);
		wait 3;
		portalThere.extract();
		targetRoom.echo("The portal closes in on itself.");
		wait 4;
		act("A circle is formed around $n and Sheriam Sedai by the Aes Sedai present.", true, accepted, null, null, constants.TO_ROOM);
		actor.send("You join the circle of women surrounding " + accepted.name + ".");
		accepted.send("The Aes Sedai draw in to form a circle around you and Sheriam Sedai.");
		wait 7;
		self.say(accepted.name + ", you are here today to test your preparation for the shawl. I hope that you are ready for this test.");
		wait 3;
		self.say("Women have died through that arch.");
		wait 2;
		self.comm("emote gestures to the oval-shaped ring hanging in the air.");
		wait 6;
		act("$N looks sternly at you.", false, accepted, null, self, constants.TO_CHAR);
		act("$n looks sternly at $N.", false, self, null, accepted, constants.TO_NOTVICT);
		wait 2;
		self.say("You have completed all of the necessary steps up to this point, " + accepted.name + ". Please remove your clothing and hand your ring to me.");
	}
}