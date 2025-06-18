var script22807 = function(self, actor, here, args, extra) {
	wait 3;
	here.echo("The portal closes in on itself.");
	let items = here.items;
	let portal = 0;
	for(let i = 0;i < items.length;++i) {
		portal = items[ i ];
		if( portal.vnum === 10600 )
			break;
	}
	if( portal ) {
		portal.extract();
	}
	wait 3;
	here.echo("The wind begins to pick up.");
	wait 4;
	var wolf = here.loadMob(19303);
	here.echo("An enormous wold steps out of an empty building.");
	wait 5;
	wolf.comm("emote tosses his head into the air, snarling fiercely.");
	wait 2;
	actor.send("A tough grey wolf starts running towards you.");
	act("A tough grey wolf starts running towards $n.", false, actor, null, null, constants.TO_ROOM);
	wait 4;
	actor.send("A gaunt trolloc swings at you with his enormous paws, catching your shoulder under his blow.");
	var trolloc = here.lodMob(22806);
	wait 1;
	actor.send("Ouch!");
	wait 2;
	trolloc.comm("kill " + actor.name);
	wait 4;
	actor.send("A tough grey wolf assists you!");
	act("A tough grey wolf assists $n.", false, actor, null, null, constants.TO_ROOM);
	wait 2;
	wolf.comm("kill trolloc");
}