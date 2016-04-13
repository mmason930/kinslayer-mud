var script22827 = function(self, actor, here, args, extra) {
	_block;
	wait 4;
	var novice = actor;
	novice.send("Cold water is poured over your head, washing away the tears before they are replaced by more.");
	act("$N steps out of the arch and is doused in water again. Her tears start up immediately.", false, novice, null, novice, constants.TO_ROOM);
	wait 4;
	here.echo("'You are washed clean of false pride.' the Red pronounces, 'You are washed clean of false ambition. You come to us washed clean, in heart and soul.'");
	wait 6;
	novice.send("As the Red turns away, Sheriam guides you by your shoulders to the third arch. 'One more child.' She encourages. 'One more, and it is done. The third time is for what will be. The way back will come but once. Be steadfast.'");
	act("Sheriam guides $N to the final arch, and after a soft whisper of encouragement, she says, 'The third time is for what will be. The way back will come but once. Be steadfast.'", false, novice, null, novice, constants.TO_ROOM);
	wait 4;
}