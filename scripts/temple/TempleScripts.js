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
};

var script2546 = function(self, actor, here, args, extra) {
	if(TempleManager.isSpiderSlabStuck() || actor.level >= 100)
		return;

	var numberOfSpiders = here.getMobs(TempleManager.spiderMobVnum);
	var numberOfNonSpiders = here.people.length - numberOfSpiders.length;

	if(random(1, numberOfNonSpiders * 5) <= numberOfNonSpiders)
	{
		here.echo("A deep rumbling resonates from the east as a slab closes the chamber shut!");
		here.echo("A large clump of web drops from the ceiling, blocking the slab!");

		var roomVnumToFirstStepMap = {};

		var echoMessageFunction = function(targetCharacter)
		{
			var firstStep = roomVnumToFirstStepMap[ targetCharacter.room.vnum ];

			if(firstStep === undefined)
			{
				firstStep = targetCharacter.room.firstStep(here);
				roomVnumToFirstStepMap[ targetCharacter.room.vnum ] = firstStep;
			}

			return "The ground shakes violently as a deep rumbling noise resonates from the " + dirToText(firstStep) + ".";
		}

		here.echoToNeighbors(echoMessageFunction, 5);

		TempleManager.markSpiderSlabAsStuck();
		here.closeDoor(constants.EAST);
		here.direction(constants.EAST).closeDoor(constants.WEST);

		setTimeout(60, function() {

			here.echo("The spider web blocking the slab disintigrates.");
			TempleManager.unmarkSpiderSlabAsStuck();
		});
	}
};

var script2547 = function(self, actor, here, args, extra) {
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

var script2548 = function(self, actor, here, args, extra) {
	if(here.getMobs(TempleManager.spiderMobVnum).length == 1 && here.getObjects(TempleManager.spiderLeverObjectVnum).length == 0)
	{
		setTimeout(5,function() {
			here.echo("A panel on the wall falls to the ground, revealing a dusty lever.");
			here.loadObj(TempleManager.spiderLeverObjectVnum);
		});
	}

};

var script2549 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);

	_block;

	if(vArgs.length == 1)
	{
		actor.send("Pull what?");
		return;
	}

	var targetObject = getObjInListVis(actor, vArgs[1], here.items);

	if(targetObject == null)
	{
		actor.send("You don't see that here.");
		return;
	}

	if(targetObject != self)
	{
		actor.send("You can't pull that!");
		return;
	}

	act("You pull $P.", false, actor, null, targetObject, constants.TO_CHAR);
	act("$n pull $P.", false, actor, null, targetObject, constants.TO_ROOM);

	setTimeout(3, function() {

		here.echo("You hear a loud thud from the east.");
		var room1 = getRoom(2543);
		var room2 = getRoom(2546);

		if(self.spiderSlabOpen === undefined || self.spiderSlabOpen === false)
		{
			room1.unlockDoor(constants.EAST);
			room1.openDoor(constants.EAST);

			room2.unlockDoor(constants.WEST);
			room2.openDoor(constants.WEST);

			room1.echo("The " + room1.doorName(constants.EAST) + " opens to the east.");
			room2.echo("The " + room2.doorName(constants.WEST) + " opens to the west.");

			self.spiderSlabOpen = true;
		}
		else
		{
			room1.closeDoor(constants.EAST);
			room1.lockDoor(constants.EAST);

			room2.closeDoor(constants.WEST);
			room2.lockDoor(constants.WEST);

			room1.echo("The " + room1.doorName(constants.EAST) + " closes to the east.");
			room2.echo("The " + room2.doorName(constants.WEST) + " closes to the west.");

			self.spiderSlabOpen = false;
		}
	});
};

var script2550 = function(self, actor, here, args, extra) {
	here.getObjects(TempleManager.spiderLeverObjectVnum).forEach(function(item) {
		item.extract();
	});
};

var script2551 = function(self, actor, here, args, extra) {
	if(self.vnum == 2403)
	{//Skeleton
		var emoteFunction = null;

		if(self.fighting)
			emoteFunction = TempleManager.getRandomSkeletonFightingEmote();
		else
			emoteFunction = TempleManager.getRandomSkeletonStandingEmote();

		if(emoteFunction !== null && emoteFunction !== undefined)
			emoteFunction(self, self.fighting);
	}
	else if(self.vnum == 2404)
	{//Skeleton lord

		var roll = random(1, 2);

		if(roll == 1)
		{
			var numberOfSkeletons = here.getMobs([2403, 2404]).length;
			var numberOfNonSkeletons = here.people.length - numberOfSkeletons;

			if(numberOfNonSkeletons > numberOfSkeletons)
			{
				setTimeout(1, function() {
					act("$n strikes his scepter to the ground, unleashing a violent wave that rocks the floor!", false, self, null, null, constants.TO_ROOM);
					act("You strike your scepter to the ground, unleashing a violent wave that rocks the floor!", false, self, null, null, constants.TO_CHAR);

					setTimeout(5, function() {

						var newSkeleton = here.loadMob(2403);

						here.echo("Skeletal arms rip through the earthly floor, slowly emerging to reveal a reanimated skeleton!");

						newSkeleton.comm("follow skeletonlord");
						self.comm("group all");
						newSkeleton.comm("assist skeletonlord");
					});
				});
			}
		}

	}
};

var script2552 = function(self, actor, here, args, extra) {
	act("$n shatters into a heap on the ground!", false, self, null, null, constants.TO_ROOM);
	var scepter = self.eq(constants.WEAR_HOLD);

	if(scepter != null)
		scepter.moveToRoom(here);

	setTimeout(5, function() {

		if(scepter.isValid)
		{
			act("$p lands on the ground with thud.", false, null, scepter, null, constants.TO_ROOM);
			scepter.attach(TempleManager.skeletonScepterShatterScriptVnum);
		}
	});
};

var script2553 = function(self, actor, here, args, extra) {
	getCharCols(actor);

	actor.sendRaw(red);
	act("The nerves in your hand explode as your fingers touch $p.", false, actor, self, null, constants.TO_CHAR);
	actor.sendRaw(nrm);
	act("$p disintigrates!", false, actor, self, null, constants.TO_CHAR);
	act("$p flashes brightly as $n touches it before shattering against the ground.", false, actor, self, null, constants.TO_ROOM);

	actor.damage(20);

	setTimeout(5, function() {

		here.echo("A panel on the wall falls to the floor, revealing three levers.");
	});

	self.extract();
};
