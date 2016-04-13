var script10661 = function(self, actor, here, args, extra) {
	var message = new Array();
	message[0] = "say This one will have to do, I suppose.";
	message[1] = "say This recruit is acceptable, if just barely.";
	message[2] = "say Obedient and absolutey immoral. Excellent!"; 
	message[3] = "say Perfect. This one's rotten to the core!";
	var target = new Array();
	target[0] = "emote breathes 'Now go. Kill Elayne Trakand! Have all the fun you want with her before she dies."
	target[1] = "emote breathes 'Now go. Kill the Amyrlin Seat! And make sure she suffers."
	target[2] = "emote breathes 'Now go. Kill Lord Agelmar! And his meddling sidekick Uno."
	target[3] = "emote breathes 'Now go. Kill the Dragon! And strangle a few peasants from his backwater village."
	var recruit = actor.getCharRoomVis("QUEST_PLAIN");
	var recruit2 = actor.getCharRoomVis("2.QUEST_PLAIN");
	var recruit3 = actor.getCharRoomVis("3.QUEST_PLAIN");
	var recruit4 = actor.getCharRoomVis("4.QUEST_PLAIN");
	var recruit5 = actor.getCharRoomVis("5.QUEST_PLAIN");
	var qName = 'Eternal Servitude';
	if ( Quest.getByName(qName).hasBegun(actor) && recruit != undefined && recruit2 != undefined && recruit3 != undefined && recruit4 != undefined && recruit5 != undefined && actor == 
	recruit.leader && actor.questTaskProgress(qName)[2].comp && !actor.questTaskProgress(qName)[3].comp ) {
	   waitpulse 10;
	   self.say("So you've brought me five recruits...good job, " + actor.name + "!" );
	   self.say("Let me inspect each one before I give him a mission." );
	   waitpulse 14;
	   self.comm("emote inspects a plain looking man...");
	   waitpulse 14;
	   self.comm(message[random(0, message.length - 1)]);
	   waitpulse 14;
	   self.comm(target[random(0, message.length - 1)]);
	   waitpulse 14;
	   recruit.comm("bow greyman");
	   waitpulse 14;
	   recruit.comm("fol self");
	   recruit.comm("e");
	   recruit.moveToRoom(getRoom(501));
	   recruit.extract();
	   waitpulse 21;
	   self.comm("emote inspects a plain looking man...");
	   waitpulse 14;
	   self.comm(message[random(0, message.length - 1)]);
	   waitpulse 10;
	   self.comm(target[random(0, message.length - 1)]);
	   waitpulse 10;
	   recruit2.comm("bow greyman");
	   waitpulse 10;
	   recruit2.comm("fol self");
	   recruit2.comm("e");
	   recruit2.moveToRoom(getRoom(501));
	   recruit2.extract();
	   waitpulse 21;
	   self.comm("emote inspects a plain looking man...");
	   waitpulse 14;
	   self.comm(message[random(0, message.length - 1)]);
	   waitpulse 10;
	   self.comm(target[random(0, message.length - 1)]);
	   waitpulse 10;
	   recruit3.comm("bow greyman");
	   waitpulse 10;
	   recruit3.comm("fol self");
	   recruit3.comm("e");
	   recruit3.moveToRoom(getRoom(501));
	   recruit3.extract();
	   waitpulse 21;
	   self.comm("emote inspects a plain looking man...");
	   waitpulse 14;
	   self.comm(message[random(0, message.length - 1)]);
	   waitpulse 10;
	   self.comm(target[random(0, message.length - 1)]);
	   waitpulse 10;
	   recruit4.comm("bow greyman");
	   waitpulse 10;
	   recruit4.comm("fol self");
	   recruit4.comm("e");
	   recruit4.moveToRoom(getRoom(501));
	   recruit4.extract();
	   waitpulse 21;
	   self.comm("emote inspects a plain looking man...");
	   waitpulse 14;
	   self.comm(message[random(0, message.length - 1)]);
	   waitpulse 10;
	   self.comm(target[random(0, message.length - 1)]);
	   waitpulse 10;
	   recruit5.comm("bow greyman");
	   waitpulse 10;
	   recruit5.comm("fol self");
	   recruit5.comm("e");
	   recruit5.moveToRoom(getRoom(501));
	   recruit5.extract();
	   waitpulse 21;
	   self.say("Let the chaos begin.");
	   waitpulse 5;
	   self.comm("chuckle");
	   waitpulse 5;
	   self.loadObj(10636);
	   waitpulse 5
	   self.comm("give token " + actor.name);
	   waitpulse 14
	   self.comm("emote breathes 'Bring this back to your base as a token of your services.'");
	}
}