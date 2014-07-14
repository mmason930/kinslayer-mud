var script22819 = function(self, actor, here, args, extra)
{
	wait 6;
here.echo("As you near your home, the street becomes more crowded, filling with people transfixed by some commotion happening in the center of the large group.");
here.loadMob(10219);
here.loadMob(10219);
here.loadMob(10219);
here.loadMob(10219);
here.loadMob(8192);
here.loadMob(22110);
wait 4;
here.echo("You push your way past people, eager to see what all the fuss is about, but upon seeing your mother and father pulled out of your home, your breath catches in your throat.");
wait 6;
actor.comm("look");
here.echo("Three Children of the Light stand guard over your parents, while another one reads from a scroll produced from his belt pouch.");
wait 5;
here.echo("The Child of the Light says, 'These two pieces of filth have been declared Darkfriends by the nations of the Light, and for their crimes of treason, they shall be put to death immediately!");
wait 6;
here.echo("One of the whitecloaks sneers as he stabs his sword deep into your fathers gut.");
wait 5;
here.echo("The way comes but once. Be steadfast.");
wait 6;
here.echo("You hold your head, wondering where that voice came from. Focusing back on the moment, you wonder what you should do. Your fathers life is slowly draining away, and your mother is still in danger as well.");
wait 4;
here.echo("The way back...");
wait 4;
here.echo("Something on the edge of your vision flashes.");
wait 4;
here.echo("..comes but once."); 
wait 4;
here.echo("A glowing portal shimmers into view.");
_block;
actor.comm("phasetwo");

};

