var script13251 = function(self, actor, here, args, extra)
{
	if( actor.quest( 'Special Delivery' ) < 1 )
	return;
wait 1;
self.say("Excellent! Let me fetch the crate!");
wait 2;
self.comm("emote hurries off to the back of the room.");
wait 2;
self.comm("emote returns a moment later dragging a bulky wooden crate.");
here.loadObj( 21407 );
wait 3;
self.say("This crate needs to be delivered to Master Luhhan. He resides in the north part of town.");
wait 3;
self.say("I'm afraid it's too heavy to lift. You will need to drag it up the road.");
wait 3;
actor.questSend("To begin dragging the crate, type 'drag crate'");

};

