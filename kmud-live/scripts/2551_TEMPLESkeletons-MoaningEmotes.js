var script2551 = function(self, actor, here, args, extra)
{
	if(self.vnum == 2403)
{//Skeleton
	var emoteFunction = null;
	
	if(self.fighting)
		emoteFunction = TempleManager.getRandomSkeletonFightingEmote();
	else
		emoteFunction = TempleManager.getRandomSkeletonStandingEmote();
	
	here.echo(" *** Emote Function *** : " + (emoteFunction == null ? "<NULL>" : "Set"));
	
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

