var script22809 = function(self, actor, here, args, extra)
{
	_block;
wait 3;
here.echo("A tough gray wolf glances at you before turning back towards the village, stopping to paw at the trolloc corpse on his way.");
wait 5;
actor.send("Remember what must be remembered.");
wait 2;
actor.send("You begin pelting after the wolf.");
act("$n begins chasing after a rough gray wolf!", true, actor, null, null, constants.TO_ROOM);
wait 6;
actor.send("The wolf continues running, eluding your grasp.");
act("A tough gray wolf quickens his pace, eluding $n.", true, actor, null, null, constants.TO_ROOM);
wait 4;
actor.send("You slow to a halt, huffing to regain your breath.");
act("$n stops to catch her breath.", true, actor, null, null, constants.TO_ROOM);
wait 6;
here.echo("The startled yelp of a wolf rings out nearby!");
wait 3;
actor.send("You follow the source of the noise, coming upon the wounded wolf. It has fallen in to a deep trench.");
act("$n chases after the source of the noise, disappearing quickly from view.", true, actor, null, null, constants.TO_ROOM);
wait 6;
actor.send("The wolf whimpers to you from the pit, his unwounded forepaw scrabbling helplessly at the steep sides of the trap.");
wait 4;
actor.send("The continous wind blows gritty sand in to your face, forcing you to blink your eyes furiously to remove the intrusion.");
wait 7;
actor.send("When you open your eyes, a large star can be seen etched in to the dirt not far away.");
wait 5;
actor.send("You move towards the apparition, but the insistent whimpering of the wolf stops you in your tracks.");
wait 3;
actor.send("You look between the wolf and your goal uncertainly.");

};

