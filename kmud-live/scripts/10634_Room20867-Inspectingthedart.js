var script10634 = function(self, actor, here, args, extra)
{
	_block;
        actor.send("You begin inspecting a black-tipped dart...");
        here.echoaround(actor, capFirstLetter(actor.name) + " begins inspecting something in the wall.");
        actor.startTimer(3);
        var result = runTimer(actor);
        
        if( result == true )
        {//25% chance of failing
            if( random(1,100) <= 35 ) {
            //Success
            actor.send("Your skills quickly yield the composition of the black substance.");
            waitpulse 3;
            actor.send("A black-tipped dart is coated with powdered peach pit.");
            }
            else {
            //Fail
            actor.send("Your skills fail to inspect the dart.");
            waitpulse 3;
            actor.send("A black-tipped dart is coated with a black substance.");
            }
        }

};

