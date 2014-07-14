var script13223 = function(self, actor, here, args, extra)
{
	//Alder
//May 2010
	var vArgs = getArgList(args);
	if ( vArgs.length >= 2 ) {
		var cmd = vArgs[1];	// updates or greetings
		var qval;	//1=off 0=on
		if ( strn_cmp(cmd,'greetings',5) == true ) {
			_block;
			qval = actor.quest('QUEST_GREETINGS');
			actor.qval('QUEST_GREETINGS',!qval);
			actor.send("Quest greetings toggled "+(actor.quest('QUEST_GREETINGS') == 0 ? 'ON' : 'OFF' )+".");
		}
		else if ( strn_cmp(cmd,'updates',3) == true ) {
			_block;
			qval = actor.quest('QUEST_UPDATES');
			actor.qval('QUEST_UPDATES',!qval);
			actor.send("Quest updates toggled "+(actor.quest('QUEST_UPDATES') == 0 ? 'ON' : 'OFF' )+".");
		}
	}
	else {
		_block;
		actor.send("Quest Greetings: "+(actor.quest('QUEST_GREETINGS') == 0 ? 'ON' : 'OFF' ));
		actor.send("Quest Updates: "+(actor.quest('QUEST_UPDATES') == 0 ? 'ON' : 'OFF' ));
	}
		

};

