var script18603 = function(self, actor, here, args, extra) {
	//Determines actions taken when a PC engages a mercenary
	//Rhollor - December 2009

	var owner = MercUtil.getOwner(self);

	if(actor.vnum != -1)
		return;//Mob.
	if(!owner)
		return;

	waitpulse 5;
	if(owner != actor)
	{//Attacker is not the owner.
		self.tell(owner, "I didn't sign on to mess with someone like " + actor.name + " , I'll leave your stuff here!");

		var rendRoom = MercUtil.getRendezvousRoom(self);

		if(rendRoom && rendRoom != here)
		{
			self.tell(owner, "I'll rendezvous with you at " + rendRoom.name + ".");

			if(owner.room.vnum == self.room.vnum) {
				owner.send(capFirstLetter(self.name) + " heads for the rendezvous.");
			}

			actor.send(capFirstLetter(self.name) + " flees from you, leaving " + owner.name + " behind!");
			echoaroundmany([owner,actor],capFirstLetter(self.name) + " flees from " + actor.name + ", leaving " + owner.name + " behind.");
			self.moveToRoom(rendRoom);
		}
		else
		{
			if(owner.room && owner.room.vnum == self.room.vnum) {
				owner.send(capFirstLetter(self.name) + " flees from " + actor.name + " and leaves you behind!");
			}
			actor.send(owner,capFirstLetter(self.name) + " flees before you leaving " + owner.name + " behind!");
			echoaroundmany([owner,actor],owner,capFirstLetter(self.name) + " flees from " + actor.name + ", leaving " + owner.name + " behind.");

			MercUtil.clearSavedMercenaryEquipment(self);
			MercUtil.removeMercenaryFromOwner(self, owner);
			MercUtil.extractMercenary(self);
		}
	}
	else
	{
		self.say("How could you!");
		owner.send(capFirstLetter(self.name) + " flees before you, leaving his things behind.");
		echoaround(owner,capFirstLetter(self.name) + " flees before " + owner.name + ".");

		MercUtil.clearSavedMercenaryEquipment(self);
		MercUtil.removeMercenaryFromOwner(self, owner);
		MercUtil.extractMercenary(self);
	}
};
