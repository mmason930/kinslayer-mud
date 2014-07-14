var script13515 = function(self, actor, here, args, extra)
{
	if( extra.direction == constants.UP ) {
        _block;
        actor.send("You begin climbing your way up...");
        here.echoaround(actor, capFirstLetter(actor.name) + " begins climbing " + his_her(actor.sex) + " way up.");
        actor.startTimer(3);
        var result = runTimer(actor);
        
        if( result == true )
        {//25% chance of slipping back in
            if( random(1,100) <= 25 ) {
                    //Fail.
                        actor.send("You fall back down into the rocky bottom.");
                        here.echoaround(actor, capFirstLetter(actor.name) + " falls back down onto the rocky bottom.");
                        actor.moveToRoom(here);
                        actor.comm("look");
                }
                else {
                    //Success.
                        actor.send("You manage to pull yourself up.");
                        here.echoaround(actor, capFirstLetter(actor.name) + " manages to pull " + him_her(actor.sex) + "self up.");
                        actor.moveToRoom( here.direction( constants.UP ) );
                        actor.comm("look");
                        actor.room.echoaround(actor, capFirstLetter(actor.name) + " climbs up and out of the dry rocky bottom.");
                }
        }
}

};

