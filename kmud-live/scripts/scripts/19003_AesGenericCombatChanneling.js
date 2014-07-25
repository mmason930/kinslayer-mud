var script19003 = function(self, actor, here, args, extra) {
	/*Script for Channeling during Combat*/
	/*Rhollor Dec 09*/
	var rand = random(0,5);
	var dmg, msgToActor, msgToRoom, length;
	switch(rand) {
		case 0:
			/*Air Scythe*/
			dmg = random(15,19);
			length = 1.5;
			msgToActor = "You suddenly feel intense pain as deep cuts appear on your body.";
			msgToRoom = actor.name + "'s flesh is sliced apart!";
			break;
		case 1:
			/*Fireball*/
			dmg = random(40,60);
			length = 4;
			msgToActor = "A large fireball blooms into existence and envelops you!";
			msgToRoom = "A large fireball blooms into existence and envelops " + actor.name;
			break;
		case 2:
			/*Ice Shards*/
			dmg = random(26,36);
			length = 3.25;
			msgToActor = "Razor sharp ice shards seem to form from thin air before impaling you.";
			msgToRoom = actor.name + " is struck by razor sharp ice shards!";
			break;
		case 3:
			/*Impact*/
			dmg = random(27,39);
			length = 3.25;
			msgToActor = "A huge boulder rises from the ground and then crashes into you, causing a lot of pain.";
			msgToRoom = "A boulder floats up into the air and then races towards " + actor.name + ", crashing right into " + him_her(actor.sex) + ".";
			break;
		case 4:
			/*Create Fire*/
			dmg = random(15,25);
			length = 1.5;
			msgToActor = "There is sudden burst of light as sparks ignite your clothing.";
			msgToRoom = actor.name + " howls in pain as flames suddenly appear on " + him_her(actor.sex) + ".";
			break;
		case 5:
			/*Frostbite*/
			dmg = random(14,22);
			length = 1.5;
			msgToActor = "Your limbs start to feel numb as frost accumulates over them.";
			msgToRoom = "Frost begins to grow on " + actor.name + ".";
			break;
	}
	if(!self.isBashed) {
		self.startTimer(length);
		var success = runTimer(self);
		if(success && actor.room == self.room) {
			var neck1 = self.eq(constants.WEAR_NECK_1);
			if(neck1){ neck1 = neck1.vnum; }
			var neck2 = self.eq(constants.WEAR_NECK_2);
			if(neck2){ neck2 = neck2.vnum; }
			if(self.affectedBy(constants.AFF_SHIELD) || neck1 == 1120 || neck2 == 1120){
				here.echo(capFirstLetter(self.name) + " looks suprised as " + self.heShe() + " fails to reach the source.");
				return;
			}
			if(actor.class == constants.CLASS_FADE) {
				if(random(0,5) == 2) {
					actor.send("You avoid " + self.name + "'s attempts at channeling!");
					here.echoaround(actor,capFirstLetter(actor.name) + " avoid's " + self.name + "'s attempt at channeling!");
				}
			}
			actor.send(msgToActor);
			var list = [self,actor];
			echoaroundmany(list,msgToRoom);
			actor.damage(dmg, self);
		}
	}
	
}