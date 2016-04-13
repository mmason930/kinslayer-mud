var script10643 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var achv = vArgs[1];
	var vict = vArgs[2];
	_block;
	//Child of Light Achievement
	if(achv == "col")
	{
	        vict = getCharInListVis(actor, vArgs[2].trim(), here.people);
	        vict.lockAchievement( 10601 );
	        vict.lockTitle("Child of Light", "normal", "clans");
	        actor.send("SUCCESS");
	        return;
	}
	else
	{
	        actor.send("FAIL.");
	        return;
	}
}