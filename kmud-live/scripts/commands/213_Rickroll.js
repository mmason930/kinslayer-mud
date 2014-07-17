var script213 = function(self, actor, here, args, extra) {
	if( actor.level < 100 ) {
		return;
	}
	_block;
	var vArgs = getArgList( args );
	if( vArgs.length < 2 ) {
		actor.send("Rickroll whom?");
		return;
	}
	var targetStr = vArgs[1];
	var rickAstleyMobVnum = 9;
	var target = actor.getCharVis(targetStr);
	if( !actor ) {
		actor.send("Nobody is around by that name!");
		return;
	}
	if( target.level > actor.level ) {
		actor.send("How dare you try to rickroll your superiors!");
		return;
	}
	function fakeEmote(actor, message) {
		act("$n " + message, false, actor, null, null, constants.TO_ROOM);
		actor.send("You " + message);
	}
	var rickAstley = target.room.loadMob(rickAstleyMobVnum);
	rickAstley.room.echo("A catchy beat begins playing.");
	mudLog(constants.CMP, 102, target.name + " just got rickroll'd by " + actor.name + "!");
	wait 2;
	if( rickAstley.isValid )
		rickAstley.room.echo("Rick Astley appears from above!");
	wait 2;
	if( rickAstley.isValid )
		fakeEmote(rickAstley, "dances a little strut.");
	wait 3;
	var lyrics = [];
	lyrics.push("We're no strangers to love.");
	lyrics.push("You know the rules and so do I.");
	lyrics.push("A full commitment's what I'm thinking of.");
	lyrics.push("You wouldn't get this from any other guy.");
	lyrics.push("I just wanna tell you how I'm feeling.");
	lyrics.push("Gotta make you understand.");
	lyrics.push("Never gonna give you up,");
	lyrics.push("Never gonna let you down,");
	lyrics.push("Never gonna run around and desert you,");
	lyrics.push("Never gonna make you cry,");
	lyrics.push("Never gonna say goodbye,");
	lyrics.push("Never gonna tell a lie and hurt you.");
	lyrics.push("We've known each other for so long.");
	lyrics.push("Your heart's been aching but you're too shy to say it.");
	lyrics.push("Inside we both know what's been going on.");
	lyrics.push("We know the game and we're gonna play it.");
	lyrics.push("And if you ask me how I'm feeling.");
	lyrics.push("Don't tell me you're too blind to see.");
	lyrics.push("We've known each other for so long.");
	lyrics.push("Your heart's been aching but you're too shy to say it.");
	lyrics.push("Inside we both know what's been going on.");
	lyrics.push("We know the game and we're gonna play it.");
	lyrics.push("I just wanna tell you how I'm feeling.");
	lyrics.push("Gotta make you understand.");
	for(var index = 0;index < lyrics.length;++index) {
		if( rickAstley.isValid == false )
			break;
		var line = lyrics[ index ];
		fakeEmote(rickAstley, "sings, '" + line + "'");
		
		wait 3;
	}
	if( rickAstley.isValid == true ) {
		
		fakeEmote(rickAstley, "takes a bow.");
		
		wait 3;
		
		fakeEmote(rickAstley, "ascends into the sky.");
		
		rickAstley.moveToRoom(getRoom(1));
		
		rickAstley.extract();
	}
}