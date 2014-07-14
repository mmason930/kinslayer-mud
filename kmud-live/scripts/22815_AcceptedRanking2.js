var script22815 = function(self, actor, here, args, extra)
{
	if( !actor.isAesSedai() )
        return;
var vArgs = getArgList(args.toLowerCase());
if( vArgs[0] == "novice" && vArgs[2] == "is" && vArgs [3] == "ready" && vArgs[4] == "for" && vArgs[5] == "her" && vArgs [6] == "accepted" && vArgs[7] == "test" && (vArgs[8] == "sister" || vArgs[8] == "sister.") )
{
        wait 1;
        var novice = getCharInListVis(self,vArgs[1],here.people);
        if( !novice ) {
                self.say("Really? Because I do not see " + capFirstLetter(vArgs[1]) + " here.");
                return;
        }
self.say("Very well then, I will alert Sheriam immediately.");
wait 8;
var sheriam = here.loadMob(22317);
setSval(sheriam,22815,"novice",novice);
here.echo("Sheriam enters the room, her blue shawl draped carefully over her shoulders.");
for each(var person in here.people){if(person.name == "Sheriam Sedai"){person.comm("phaseone")}};
}

};

