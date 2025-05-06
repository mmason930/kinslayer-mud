var script22829 = function(self, actor, here, args, extra) {
	_block;
	wait 6;

	here.echo("The White Tower shakes from another blast, causing you to brace yourself against a wall for support.");
	wait 4;

	here.echo("Soldiers and Aes Sedai run through the halls, firing arrows and weaves alike. Fire and rubble litter the hallways, as well as the acrid smell of burning flesh. Triumphant howls fill the air as another blast hits the Tower.");
	wait 2;

	actor.comm("embrace");
	wait 8;

	actor.comm("look");
	wait 6;

	here.echo("Glancing outside a nearby window, you barely believe your eyes. Tens of thousands of large forms run through the streets of Tar Valon, flooding into the Tower itself.");
	wait 6;

	here.echo("Suddenly, a woman wearing a green shawl turns the corner, almost arching a fireball in your direction before she lets the weave drop. 'Oh, " + actor.name + ".' she says. 'It is only you. The main force is fighting on the fourteenth floor, but they're barely holding on. I fear the Tower is lost. We tried to evacuate but Traveling doesn't work. I think - ' Her words are cut off as a bolt of lightning strikes her in the back.");
	wait 12;

	here.echo("Surprised, you spare a brief moment to see a dreadguard sneer before fleeing into the hallway to the east.");
	wait 4;

	here.echo("You spare a look behind your shoulder and the end of the hallway, watching for your pursuer. You push your way into a room as he turns the corner.");
	wait 8;

	here.echo("'" + actor.name + " Sedai!' A novice cries out. 'Please help!' A group of twelve novices huddle together on the far side of the room. Between them and you are four trollocs brandishing bloodied weapons. They advance towards the novices, who scream in terror.");
	wait 8;

	here.echo("The way comes but once. Be steadfast.");
	wait 6;

	here.echo("You stop preparing your Immolate weave to look around, wondering where the voice came from.");
	wait 6;

	here.echo("You shake your head and focus on the task at hand. With these novices a link could be formed. Surely you would be able to flee to safety with that much Power...");
	wait 8;

	here.echo("The way back...");
	wait 3;

	here.echo("A trolloc reaches out and grabs a novice by the hair, dragging her away from the others, making sickening guttural sounds, as if laughing.");
	wait 8;

	here.echo("Something on the edge of your vision flashes.");
	wait 8;

	here.echo("..comes but once.");
	wait 4;

	here.echo("A glowing portal shimmers into view.");
	actor.comm("parttwo");
}