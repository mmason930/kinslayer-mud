var script2545 = function(self, actor, here, args, extra) {
	if(TempleManager.canSpawnSpiderFor(actor))
	{
		setTimeout(random(7, 14), function() {
		
			if(actor.room.vnum != 2545)
				return; //Character has moved away. Do not spawn.
				
			var spider = TempleManager.spawnSpiderFor(actor);
			TempleManager.updateSpidersLastSawPlayerEnterDatetime();
			
			act("You crawl out of a gaping hole in the corner of the room and charge at $N!", false, spider, null, actor, constants.TO_CHAR);
			act("$n crawls out of a gaping hole in the corner of the room and charges at you!", false, spider, null, actor, constants.TO_VICT);
			act("$n crawls out of a gaping hole in the corner of the room and charges at $N!", false, spider, null, actor, constants.TO_NOTVICT);
			
			spider.setFighting(actor);
		});
	}
}