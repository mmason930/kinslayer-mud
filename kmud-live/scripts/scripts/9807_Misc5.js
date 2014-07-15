var script9807 = function(self, actor, here, args, extra) {
	var weapon = actor.eq( constants.WEAR_WIELD );
	if( !weapon || weapon.name != "a 12 gauge double barreled remington" )
		return;
	_block;
	actor.comm("emote blows the smoke from the tip of the gun barrel.");
	wait 2;
	actor.comm("emote twirls " + weapon.name + " around his finger.");
	wait 2;
	actor.comm("override sheath");
}