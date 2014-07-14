var script2547 = function(self, actor, here, args, extra)
{
	if(!TempleManager.isSpiderSlabStuck())
	return;//This trigger does nothing if the slab is not stuck.

//But if so, check to see if the actor is trying to open the slab. If so, prevent it from going through.
var vArgs = getArgList(args);

if(getArg(vArgs, 1).toLowerCase() == "slab")
{
	if(here.vnum == 2544) //Antechamber
	{
		actor.send("You try to open the slab, but it won't budge.");
		act("$n tries to open the slab, but it won't budge.", false, actor, null, null, constants.TO_ROOM);
	}
	else
	{
		actor.send("You try to reach the slab, but a massive web is in your way.");
		act("$n tries to open the slab, but a massive web is in $s way.", false, actor, null, null, constants.TO_ROOM);
	}
	
	_block;
}

};

