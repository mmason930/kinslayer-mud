var script22817 = function(self, actor, here, args, extra)
{
	var dress = extra.obj;
if( dress.vnum == 22305 ) {
        wait 2;
        here.echo("Sheriam nods and folds up the dress, placing it on top of the chest with the other belongings.");
        wait 6;
        self.say("I'll keep this safe with me until you return.");
        wait 8;
        act("An Aes Sedai of the Red Ajah steps forward and says, 'Whom do you bring with you, Sister?'", false, actor, null, null, constants.TO_ROOM);
	act("An Aes Sedai of the Red Ajah steps forward and says, 'Whom do you bring with you, Sister?'", false, actor, null, null, constants.TO_CHAR);
        wait 4;
        here.echo("Sheriam replies, 'One who comes as a candidate for Acceptance, Sister.'");
        wait 8;
        here.echo("The Red Ajah Sedai says, 'Is she ready?'");
        wait 6;
        here.echo("Sheriam Sedai says, 'She is ready to leave behind what she was, and, passing through her fears, gain Acceptance'");
                wait 6;
		act("The Aes Sedai casts a glance at you. 'Does she know her fears?'", false, actor, null, null, constants.TO_CHAR);
		act("The Aes Sedai casts a glance at " + actor.name + ". 'Does she know her fears?'", false, actor, null, null, constants.TO_ROOM);
                wait 6;
                here.echo("Sheriam Sedai replies firmly, 'She has never faced them, but now is willing.'");
                wait 8;
                act("An Aes Sedai of the Red Ajah turns and walks toward the Arches. 'Then let her face what she fears.'", false, actor, null, null, constants.TO_ROOM);
                act("An Aes Sedai of the Red Ajah turns and walks toward the Arches. 'Then let her face what she fears.'", false, actor, null, null, constants.TO_CHAR);
                wait 6;
		act("While Sheriam Sedai gestures for you to enter the first arch she says, 'The first time is for what was. The way back will come but once. Be steadfast.'", false, actor, null, null, constants.TO_CHAR);
		act("While Sheriam Sedai gestures for " + actor.name + " to enter the first arch she says, 'The first time is for what was. The way back will come but once. Be steadfast.'", false, actor, null, null, constants.TO_ROOM);
wait 2;
    }

};

