var script18601 = function(self, actor, here, args, extra)
{
	var price = getSval(self,18600,"price");
var owner = getSval(self,18600,"owner");
var paid = extra.amount;
var maxMercs = 30;
if(actor == owner) {
	wait 1
	self.say("Thanks for the cash ya moron.");
}
else {
	if(paid >= price) {
		if(actor.level > 99) {
			wait 1;
			self.say("Oh great; enslaved to an immortal!");
			//Add any additions to the merc for immortals here
		}
		else {
			//If in the future we want to change stats/levels etc based on the new owners level add here
		}
//Galnor 12/28/2009 - Removed because bribe scripts already remove the gold. This is charging double.
//		actor.gold -= paid;
		wait 1;
		self.say("Ok I'll join you for this bit of coin.");
		if(owner != 0) {
			if(owner.isValid) {
				wait 1
				self.say("But first, here's the stuff from " + owner.name);
				self.comm("rem all");
				self.comm("give all " + owner.name);
				self.comm("rem all");
				self.comm("drop all");
				//Save all eq slots as empty when they are removed and dropped
				self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
			}
			else {
				/* Decrement the mercsInGame sval */
				setSval(getSval(self,18600,"homeRoom"),-18600,"mercsInGame",parseInt(getSval(getSval(self,18600,"homeRoom"),-18600,"mercsInGame")) -1);
				fwrite(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"), "");
			}
		}
		setSval(self,18600,"owner",actor);
		owner = getSval(self,18600,"owner");
		setSval(self,18600,"price",price + 20);
		//Determine which merc new hire is, and re calculate the number of mercs actor owns
		var numMerc = getSval(actor,18601,"numMerc");
		if(numMerc >= maxMercs) {
			waitpulse 5;
			self.say("Wait, you're the one amassing an army! I'll have no part in that!");
			self.comm("give " + paid + " copper " + actor.name);
			self.extract();
			return;
		}
		setSval(self,18600,"mercNum",numMerc+1);
		setSval(actor,18601,"numMerc",numMerc+1);
		//Files will take this format: OWNER_ID.mercEQ.MERC_NUM
		self.mobSaveEq(owner.id + ".mercEQ." + getSval(self,18600,"mercNum"));
		owner.attach(18614);
		owner.send(owner.red(constants.CL_OFF) + "If you rent or quit out you will lose this mercenary and " + self.his_her() +" equipment will not be saved."+ owner.normal(constants.CL_OFF));
		self.comm("follow " + owner.name);
	}
	else {
		wait 1.5
		self.say("I'm no cheap mercenary!");
		wait 1
		self.say("My going price is " + price + " copper!");
		if(owner != 0 && owner.isValid) {
			wait 2
			self.say("Only for that price will I ditch " + owner.name + "!");
			self.comm("give " + paid + " copper " + actor.name);
		}
	}
}

};

