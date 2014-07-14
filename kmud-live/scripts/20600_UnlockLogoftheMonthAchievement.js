var script20600 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
var achv = vArgs[1];
var vict = vArgs[2];
_block;
//Log of the month Achievement
if(achv == "lotm")
{
	vict = getCharInListVis(actor, vArgs[2].trim(), here.people);
	vict.unlockAchievement( 20600 );
	actor.send("SUCCESS");
	return;
}
else
{
	actor.send("FAIL.");
	return;
}

};

