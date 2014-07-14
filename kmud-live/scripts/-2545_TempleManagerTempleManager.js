var TempleManager = new Object();

TempleManager.setup = function()
{
	//Spiders
	this.spiderRoomVnum = 2545;
	this.spiderMobVnum = 2402;
	this.spiderLeverObjectVnum = 2545;
	this.spiderInitialSize = 4; // Number of initial spiders spawned.
	this.spiderUserIdArray = []; //This array stores all user IDs that we have spawned additional spiders for.
	this.spiderLastSawPlayerEnterDatetime = new Date();
	this.spiderMillisecondsToReduce = (1000) * (60) * 5; //Every five minutes, reduce load size.
	this.spiderSlabIsStuck = false;
	
	//Skeletons
	this.skeletonStandingEmotes = this.generateSkeletonStandingEmotes();
	this.skeletonFightingEmotes = this.generateSkeletonFightingEmotes();
	this.skeletonScepterShatterScriptVnum = 2553;
};

TempleManager.getSpiderRoom = function()
{
	return getRoom(this.spiderRoomVnum);
};

TempleManager.canSpawnSpiderFor = function(user)
{
	return this.spiderUserIdArray.indexOf(user.id) == -1 && this.getSpiderRoom().getMobs(this.spiderMobVnum).length > 0;
};

TempleManager.spawnSpiderFor = function(user)
{
	var spider = this.getSpiderRoom().loadMob(this.spiderMobVnum);
	
	this.spiderUserIdArray.push(user.id);
	
	return spider;
};

TempleManager.updateSpidersLastSawPlayerEnterDatetime = function()
{
	this.spiderLastSawPlayerEnterDatetime = new Date();
}

TempleManager.timeForSpidersToReduce = function()
{
	return new Date().getTime() - this.spiderLastSawPlayerEnterDatetime.getTime() >= this.spiderMillisecondsToReduce;
};

TempleManager.reduceSpidersToInitialSpawnSize = function()
{
	var spiderRoom = this.getSpiderRoom();
	var spiderMobs = spiderRoom.getMobs(this.spiderMobVnum);
	
	//Extract down to the initial spawn size.
	while(spiderMobs.length > this.spiderInitialSize)
	{
		var spider = spiderMobs.pop();
		spider.extract();
	}
	
	this.spiderUserIdArray.length = 0;
};

TempleManager.markSpiderSlabAsStuck = function()
{
	this.spiderSlabIsStuck = true;
};

TempleManager.unmarkSpiderSlabAsStuck = function()
{
	this.spiderSlabIsStuck = false;
};

TempleManager.isSpiderSlabStuck = function()
{
	return this.spiderSlabIsStuck;
};

TempleManager.generateSkeletonStandingEmotes = function()
{
	return [
	
		function(skeleton)
		{
			act("$n's jaw drops open as $e lets out a low pitched moan.", false, skeleton, null, null, constants.TO_ROOM);
			act("Your jaw drops open as you let out a low pitched moan.", false, skeleton, null, null, constants.TO_CHAR);
		},
		
		function(skeleton)
		{
			act("The gentle rustling of $n's bones echoes through the room as $e hobbles around.", false, skeleton, null, null, constants.TO_ROOM);
			act("The gentle rustling of your bones echoes through the room as you hobble around.", false, skeleton, null, null, constants.TO_CHAR);
		}
	];
};

TempleManager.generateSkeletonFightingEmotes = function()
{
	return [
		function(skeleton, fighting)
		{
			fighting.sendRaw(fighting.red(constants.CL_NORMAL));
			act("$n hurls a bone right at you. Ouch!", false, skeleton, null, fighting, constants.TO_VICT);
			fighting.sendRaw(fighting.normal(constants.CL_NORMAL));
			act("$n hurls a bone right at $N. Ouch!", false, skeleton, null, fighting, constants.TO_NOTVICT);
			act("You hurl a bone right at $N. Ouch!", false, skeleton, null, fighting, constants.TO_CHAR);
			
			fighting.damage(random(15, 25), skeleton);
		},
		
		function(skeleton, fighting)
		{
			fighting.sendRaw(fighting.red(constants.CL_NORMAL));
			act("$n approaches you hastily, headbutting you!", false, skeleton, null, fighting, constants.TO_VICT);
			fighting.sendRaw(fighting.normal(constants.CL_NORMAL));
			act("$n approaches $N hastily, headbutting $M!", false, skeleton, null, fighting, constants.TO_NOTVICT);
			act("You suddenly approach $N hastily, headbutting $M!", false, skeleton, null, fighting, constants.TO_CHAR);
			
			fighting.damage(random(7, 13), skeleton);
			
			if(!fighting.affectedBy(constants.AFF_DISORIENT))
				fighting.affect(constants.AFF_DISORIENT, 3);
				
			fighting.lag(3);
			
			act("$n looks disoriented!", false, fighting, null, null, constants.TO_ROOM);
			act("You feel disoriented!", false, fighting, null, null, constants.TO_CHAR);
		},
		
		function(skeleton, fighting)
		{
			act("$n suddenly bites $N with $s gnarly teeth!", false, skeleton, null, fighting, constants.TO_NOTVICT);
			fighting.sendRaw(fighting.red(constants.CL_NORMAL));
			act("$n suddenly bites you with $s gnarly teeth!", false, skeleton, null, fighting, constants.TO_VICT);
			fighting.sendRaw(fighting.normal(constants.CL_NORMAL));
			act("You bite $N with your gnarly teeth! Take that!", false, skeleton, null, fighting, constants.TO_CHAR);
			
			fighting.damage(random(3, 7), skeleton);
		},
		
		function(skeleton, fighting)
		{
			act("$n moans, '$N, I am going to put your bones in the ground!'", false, skeleton, null, fighting, constants.TO_ROOM);
			act("You moan, '$n, I am going to put your bones in the ground!'", false, skeleton, null, fighting, constants.TO_CHAR);
		},
		
		function(skeleton, fighting)
		{
			act("$n moans, 'Fresh flesh... C'mere, $N!", false, skeleton, null, fighting, constants.TO_ROOM);
			act("You moan, 'Fresh flesh... C'mere, $N!'", false, skeleton, null, fighting, constants.TO_CHAR);
		},
		
		function(skeleton, fighting)
		{
			act("$n moans, 'I have a bone to pick with you, $N.'", false, skeleton, null, fighting, constants.TO_ROOM);
			act("You moan, 'I have a bone to pick with you, $N.'", false, skeleton, null, fighting, constants.TO_CHAR);
		}
	];
};

TempleManager.getRandomSkeletonFightingEmote = function()
{
	return this.skeletonFightingEmotes[ random(0, this.skeletonFightingEmotes.length - 1) ];
}

TempleManager.getRandomSkeletonStandingEmote = function()
{
	return this.skeletonStandingEmotes[ random(0, this.skeletonStandingEmotes.length - 1) ];
}

TempleManager.setup();

