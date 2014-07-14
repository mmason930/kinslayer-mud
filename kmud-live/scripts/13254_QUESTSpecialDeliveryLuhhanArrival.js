var script13254 = function(self, actor, here, args, extra)
{
	waitpulse 2;
//Check to see if the char is even on the quest.
if( actor.quest( "Special Delivery" ) != 1 )//1 == crate dragging stage.
	return;
//User is on crate dagging stage. Check to see if there is a crate being dragged by this user.
for each(var item in here.items) {
	if( item.vnum == 21407 && getSval(item, 13252, "draggedBy") == actor )
	{//Yes!
		actor.comm("release crate");//Force user to stop dragging this thing.
		wait 1;
		self.tell(actor, "Ah, yes! I've been waiting for this crate to get here!");
		wait 3;
		self.tell(actor, "Nevermind what took so long. Let's make sure everything is in there!");
		wait 3;
		self.comm("emote craftily removes the nails which seal the crate shut.");
		wait 3;
		self.comm("emote removes the lid from the top of the crate.");
		wait 3;
		self.comm("emote sifts through the straw-packed contents of the crate.");
		wait 3;
		self.tell(actor, "Burn me, Bran must have forgotten to include my new hammer!");
		wait 3;
		self.tell(actor, "I'll need you to go fetch one for me. I doubt Bran will have it, either.");
		wait 3;
		self.tell(actor, "It must be made of iron. Bring me an iron hammer and I'll compensate you for your trouble.");
		wait 3;
		actor.questSend("Be on the lookout for an iron hammer. You should be able to find one nearby.");
		wait 3;
		actor.questSend("Once you find it, return to Master Luhhan and give it to him.");
		actor.qval( "Special Delivery", 2 );
		break;
	}
}


};

