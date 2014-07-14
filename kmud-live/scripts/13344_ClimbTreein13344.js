var script13344 = function(self, actor, here, args, extra)
{
	_block;
var vArgs = getArgList( args );
if ( vArgs.length != 2 ) {
	actor.send("Yes, climb, but what?");
}
else if ( vArgs[1] != "tree" && vArgs[1] != "log" ) {
	actor.send("What is it you are trying to climb exactly?");
}
else {
	if ( actor.quest('Sunny Side Up') > 1 ) {
		if ( actor.mount == null ) {
			if ( actor.dexterity >= 18 ) {
				actor.send("You clamber on to the log and start climbing up the tree.");
				actor.startTimer(7);
				var success = runTimer(actor);
				if ( success ) {
					actor.send("\nYou reach a branch large enough to support your full weight and stand up.");
					actor.moveToRoom( getRoom(13394) );
					actor.comm("look");
				}
				else {
					waitpulse 1;
					actor.send("\nYou lose your grip on the tree and fall back to the forest floor!");
					actor.moveToRoom( getRoom(13344) );
					actor.comm("look");
					actor.damage(5);
					actor.send("\nOuch! That hurt!");
				}
			}
			else {
				actor.send("You aren't coordinated enough to climb a tree!");
			}
		}
		else {
			actor.send("You aren't climbing anything from the back of a horse!");
		}
	}
	else actor.send("Why do you need to get up there?");
}

};

