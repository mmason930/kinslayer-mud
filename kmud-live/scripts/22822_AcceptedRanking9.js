var script22822 = function(self, actor, here, args, extra)
{
	_block;
wait 4;
var novice = actor;
novice.send("You step out of the arch, trembling and sobbing, memory of your betrayal cascading back as Sheriam's face confronts you. Cold clear water washes away your tears as the Red sister slowly empties a silver chalice over your head.");
act("$N steps out of the arch, trembling and sobbing, as Sheriam faces her. The Red sister empties a chalice full of cold clear water over $N's head.", false, novice, null, novice, constants.TO_ROOM);
wait 4;
here.echo("'You are washed clean,' the Red pronounces, 'of what sin you may have done, and of those done against you. You are washed clean of what crime you may have committed, and of those committed against you. You come to us washed clean and pure, in heart and soul.'");
wait 6;
novice.send("Sheriam steps back up to you, her face of Aes Sedai serenity. 'Enter the second arch if you are ready. The second time is for what is. The way back will come but once. Be steadfast.'");
act("Sheriam stands back in front of $N and says, 'Enter the second arch if you are ready. The second time is for what is. The way back will come but once. Be steadfast.'", false, novice, null, novice, constants.TO_ROOM);
wait 4;

};

