var script1111 = function(self, actor, here, args, extra) {
	_block;
	var arg_array = args.split("'");
	if( arg_array.length != 3 )
	{
		actor.send("Syntax is: cast 'spell name' victim");
		return;
	}
	var spellName = arg_array[1].toLowerCase();
	var target = getCharInListVis(actor, arg_array[2].trim(), here.people);
	if( !target && spellName != "lolcat" )
	{
		actor.send("Cast at who?");
		return;
	}
	//Ouysa says 'Expelliarmus - Disarms your opponent.  Locomotor Mortis - Locks opponents legs. Stupefy - Knocks out opponent. Tarantallegra - Forces opponent to dance. Wingardium Leviosa - Makes on object fly.'
	if( spellName == "expecto patronum" )
	{
		actor.comm("say EXPECTO PATRONUM");
		waitpulse 2;
		//do expecto patronum messages
		//do expecto patronum stuff
	}
	else if( spellName == "stupefy" )
	{
		actor.comm("say STUPEFY");
		waitpulse 2;
		//do stupefy messages
		target.position = constants.POS_STUNNED;
		waitpulse 15;
		target.position = constants.POS_STANDING;
	}
	else if( spellName == "expelliarmus" )
	{
		actor.comm("say EXPELLIARMUS");
		waitpulse 2;
		//do expelliarmus messages
		//do expelliarmus stuff
		target.eq(constants.WEAR_WIELD).moveToChar(target);
	}
	else if( spellName == "locomotor mortis" )
	{
		actor.comm("say LOCOMOTOR MORTIS");
		waitpulse 2;
		//do locomotor mortis messages
		//do locomotor mortis stuff
	}
	else if( spellName == "tarantallegra" )
	{
		actor.comm("say TARANTALLEGRA");
		//do tarantallegra messages
		//do tarantallegra stuff
		waitpulse 2;
		target.comm("k ciarra");
	}
	else if( spellName == "wingardium leviosa" )
	{
		actor.comm("say WINGARDIUM LEVIOSA");
		waitpulse 2;
		//do wingardium leviosa messages
		//do wingardium leviosa stuff
	}
	else if( spellName == "avada kedavra" )
	{
		actor.comm("say AVADA KEDAVRA");
		waitpulse 2;
		act("A bright green flame frgoto com $n's wand hits you squarely in the chest! OWWIES!!", false, actor, null, target, constants.TO_VICT);
		act("A bright green flame shoots out from your wand, hitting $N squarely in the chest! WTFPWNED!", false, actor, null, target, constants.TO_CHAR);
		act("A bright green flame shoots out from $n's wand, hitting $N squarely in the chest!", false, actor, null, target, constants.TO_NOTVICT);
		if(target.name.toLowerCase() == "fogel" || target.name.toLowerCase() == "lolcat"){
			target.hps = 0;
	}
	}
	else if( spellName == "lolcat" )
	{
		actor.comm("say LOLCAT");
		waitpulse 2;
		if( actor.followers.length < 1 ) {
			act("A flash of white light emits from your wand, producing a furry kitten!", false, actor, null, target, constants.TO_CHAR);
			act("A flash of white light emits from $n's wand, producing a furry kitten!", false, actor, null, target, constants.TO_NOTVICT);
			here.loadMob(10201);
			waitpulse 2;
			here.people[0].comm("follow " + actor.name);
		}
	}
	else
	{
		_block;
		actor.send("Not a real spell!");
		return;
	}
}