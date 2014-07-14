var script22824 = function(self, actor, here, args, extra)
{
	wait 6;
here.loadMob(5608);
here.loadMob(8002);
here.loadMob(8008);
here.loadMob(20963);
here.loadMob(18605);
here.loadMob(3001);
wait 6;
here.echo("Every inch of your body is in pain. Ropes bind you to a stake in the ground, preventing you from moving. Blinking a few times, you adjust your eyes to the pale moonlight, barely opening your eyes to assess your situation, and your breath catches at what you see. Trolloc forms move about the camp under the watchful eye of a myrddraal, preparing a cook pot.");
wait 8;
actor.comm("look");
wait 8;
here.echo("Another woman stirs besides you, breathing in sharply before she falls over again. You recognize her as an Aes Sedai of the White Tower, Masuri Sokawa.");
wait 10;
here.echo("The fade glides over, examining his prey. Ignoring you, he pushes a waterskin into Masuri's mouth, and she sputters softly as the liquid is forced down her throat. She grimaces, muttering one word. 'Forkroot.'");
wait 10;
here.echo("The fade points at Masuri, and a trolloc lumbers over, saliva dripping from its muzzle. It unties Masuri, and despite her screams of protest, it drags her over to the boiling cookpot. The fade follows, probably thinking you are still unconscious.");
wait 10;
here.echo("The way comes but once. Be steadfast.");
wait 6;
here.echo("Your head swirls at the thought. Where had that come from?");
wait 6;
here.echo("The thought pushes you into action. Ignoring the pain in your wrists, you contort, tug, and struggle until you finally manage to free yourself from the rope.");
wait 10;
here.echo("You tentatively stand, careful not to lose your balance. You calm yourself, and open your mind to saidar, already knowing which weaves you'll use.");
wait 2;
actor.comm("embrace");
wait 4;
here.echo("The way back...");
wait 3;
here.echo("Masuri struggles against her captors, and noticing you, begins to cry out. 'Please " + actor.name + ", help me!'");
wait 4;
here.echo("Something on the edge of your vision flashes.");
wait 4;
here.echo("..comes but once."); 
wait 4;
here.echo("A glowing portal shimmers into view.");
_block;
actor.comm("stagetwo");

};

