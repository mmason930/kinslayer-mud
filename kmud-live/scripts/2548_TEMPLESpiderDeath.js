var script2548 = function(self, actor, here, args, extra)
{
	if(here.getMobs(TempleManager.spiderMobVnum).length == 1 && here.getObjects(TempleManager.spiderLeverObjectVnum).length == 0)
{
	setTimeout(5,function() {
		here.echo("A panel on the wall falls to the ground, revealing a dusty lever.");
		here.loadObj(TempleManager.spiderLeverObjectVnum);
	});
}


};

