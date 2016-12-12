var script18601 = function(self, actor, here, args, extra) {

	var price = MercUtil.getPrice(self);
	var owner = MercUtil.getOwner(self);
	var paid = extra.amount;

	if(actor == owner)
	{
		wait 1
		self.say("Thanks for the cash ya moron.");
	}
	else
	{
		if(paid >= price)
		{//Player can afford.

			//But they already have maximum mercenaries.
			if(MercUtil.getMercenaryNumberArray().length >= MercUtil.getMaximumMercenaries()) {
				waitpulse 5;
				self.say("Wait, you're the one amassing an army! I'll have no part in that!");
				self.comm("give " + paid + " copper " + actor.name);
				return;
			}

			wait 1;
			self.say("Ok I'll join you for this bit of coin.");

			if(owner != 0)
			{//Mercenary had previous owner.

				if(owner.isValid)
				{
					wait 1
					self.say("But first, here's the stuff from " + owner.name);
					self.comm("rem all");
					self.comm("give all " + actor.name);
					self.comm("rem all");
					self.comm("drop all");

					//Save all eq slots as empty when they are removed and dropped
					MercUtil.saveMercenaryEquipment(self);
				}
			}

			MercUtil.addMercenaryToOwner(self, actor, price + 20);
			owner = actor;

			//TODO: Still valid message? - Galnor 2014-12-23
			owner.send(owner.red(constants.CL_OFF) + "If you rent or quit out you will lose this mercenary and " + self.hisHer() +" equipment will not be saved."+ owner.normal(constants.CL_OFF));
			self.comm("follow " + owner.name);
		}
		else
		{
			wait 1.5
			self.say("I'm no cheap mercenary!");

			wait 1
			self.say("My going price is " + price + " copper!");

			if(owner != 0 && owner.isValid)
			{
				wait 2
				self.say("Only for that price will I ditch " + owner.name + "!");
			}

			self.comm("give " + paid + " copper " + actor.name);
		}
	}
};
