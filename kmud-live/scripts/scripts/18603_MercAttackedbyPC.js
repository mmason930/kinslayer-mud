var script18603 = function(self, actor, here, args, extra) {
	//Determines actions taken when a PC engages a mercenary
	//Rhollor - December 2009
	var ITEM_DUMP = 25; //A room vnum for dumping eq from mobs
	var homeRoom = getSval(self,18600,"homeRoom");
	var owner = getSval(self,18600,"owner");
	if (actor.vnum == -1) {
		waitpulse 5;
		if(owner) {
			if(owner != actor) {
				self.comm("tell " + owner.name + " I didn't sign on to mess with someone like " + actor.name + " , I'll leave your stuff here!");//, "you can " + owner.green(constants.CL_OFF) + "retreive "  + owner.red(constants.CL_OFF) + "your equipment later!");	
				var rendRoom = getSval(self,18605,"mercRendezvousRoom");
				if(rendRoom && rendRoom != here) {
					self.comm("tell " + owner.name + " I'll rendezvous with you at " + rendRoom.name + ".");
					if(owner.room.vnum == self.room.vnum) {
						owner.send(capFirstLetter(self.name) + " heads for the rendezvous.");
					}
					actor.send(owner,capFirstLetter(self.name) + " flees before you leaving " + owner.name + " behind!");
					echoaroundmany([owner,actor],owner,capFirstLetter(self.name) + " flees from " + actor.name + ", leaving " + owner.name + " behind."); 
					self.moveToRoom(rendRoom);
				}
				else {
					if(owner.room && owner.room.vnum == self.room.vnum) {
						owner.send(capFirstLetter(self.name) + " flees from " + actor.name + " and leaves you behind!");
					}
					actor.send(owner,capFirstLetter(self.name) + " flees before you leaving " + owner.name + " behind!");
					echoaroundmany([owner,actor],owner,capFirstLetter(self.name) + " flees from " + actor.name + ", leaving " + owner.name + " behind."); 
					fwrite(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"), "");
					self.extract();			
					// self.moveToRoom(getRoom(ITEM_DUMP));
					// self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
					// self.extract();
				}
			}
			else {
				self.say("How could you!");
				owner.send(capFirstLetter(self.name) + " flees before you, leaving his things behind.");
				echoaround(owner,capFirstLetter(self.name) + " flees before " + owner.name + ".");
				fwrite(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"), "");
				self.extract();
			}
		}
		/* Decrement the mercsInGame sval */
		setSval(getSval(self,18600,"homeRoom"),-18600,"mercsInGame",parseInt(getSval(getSval(self,18600,"homeRoom"),-18600,"mercsInGame")) -1);
	}
}