var script10642 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
var achv = vArgs[1];
var vict = vArgs[2];
_block;
//Adding Child of Light Titles
if(achv == "col")
{
        vict = getCharInListVis(actor, vArgs[2].trim(), here.people);
        vict.unlockAchievement( 10601 );
        actor.send("SUCCESS");
        return;
}
else
{
        actor.send("FAIL.");
        return;
}

};

