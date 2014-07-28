var script23230 = function(self, actor, here, args, extra) {

	var vArgs = getArgList(args);
	var targetString = vArgs[1];

	if(targetString === undefined)
	{
		return;
	}

	var targetObject = getObjInListVis(actor, targetString, here.items);

	_block;
	if(!targetObject)
	{
		actor.send("You don't see a `" + targetString + "` here.");
		return;
	}

	if(targetObject.vnum != global.global2014Util.pedestalObjectId)
	{
		actor.send("You can't repair " + targetObject.name + "!");
		return;
	}

	actor.send("You are repairing " + targetObject.name);
};