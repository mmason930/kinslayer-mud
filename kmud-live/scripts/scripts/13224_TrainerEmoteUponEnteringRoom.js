var script13224 = function(self, actor, here, args, extra) {
	waitpulse 8;
	var className = classNumToText(self.class);
	var heShe = self.sex == constants.SEX_MALE ? "he" : "she";
	var practices;
	var exampleSkillName = "RIDE";
	if(self.class == constants.CLASS_RANGER) {
		exampleSkillName = "TRACK";
		practices = actor.skillPractices;
	}
	else if(self.class == constants.CLASS_THIEF) {
		exampleSkillName = "SHORT BLADES";
		practices = actor.skillPractices;
	}
	else if(self.class == constants.CLASS_CHANNELER) {
		exampleSkillName = "EARTH";
		practices = actor.spellPractices;
	}
	getCharCols(actor);
	if(self.class != constants.CLASS_CHANNELER || (actor.class == constants.CLASS_CHANNELER && actor.sex == self.sex)) {
		actor.send( "The " + bld + red + className + nrm + " trainer is in this room. You can use the " + cyn + "PRACTICE" + nrm + " command to see a list of skills\n"
				+	"that " + heShe + " can teach you. To practice a skill, simply type \"practice\" followed by the skill name.\n"
				+	"For example: " + cyn + "PRACTICE " + exampleSkillName + nrm + "\n");
	}

	if(actor.level <= 15) {
		wait 2;

		if (actor.room.vnum != self.room.vnum || !self.canSee(actor))
			return;

		if (practices == 0) {
			self.say("You are fully trained here at this time, " + actor.name + ".");
			return;
		}

		getCharCols(actor);

		if (self.class == constants.CLASS_THIEF && actor.class == constants.CLASS_THIEF && actor.getSkill(getSkillVnum("Short Blades")) == 0) {

			self.say("You are a thief, " + actor.name + ", one of my kind. I recommend you " + bld + cyn + "PRACTICE SHORT BLADES" + nrm);
		}
		else if(self.class == constants.CLASS_WARRIOR)
		{
			if(actor.class == constants.CLASS_THIEF)
				self.say("You are a thief, " + actor.name + ". While you may want to learn some of my skills, you should really go see the thief trainer west of here. They will know how to train your kind!");
			else
			{

				var weaponTypeToData = {};
				weaponTypeToData[constants.WEAPON_AXE] = {name: "axe", skillName: "axes"};
				weaponTypeToData[constants.WEAPON_LONG_BLADE] = {name: "long blade", skillName: "long blades"};
				weaponTypeToData[constants.WEAPON_SHORT_BLADE] = {name: "short blade", skillName: "short blades"};
				weaponTypeToData[constants.WEAPON_CLUB] = {name: "club", skillName: "clubs"};
				weaponTypeToData[constants.WEAPON_STAFF] = {name: "staff", skillName: "staves"};
				weaponTypeToData[constants.WEAPON_SPEAR] = {name: "spear", skillName: "spears"};
				weaponTypeToData[constants.WEAPON_LANCE] = {name: "lance", skillName: "lances"};
				weaponTypeToData[constants.WEAPON_CHAIN] = {name: "chain", skillName: "chains"};
				weaponTypeToData[constants.WEAPON_LANCE] = {name: "lance", skillName: "lances"};

				//Figure out which weapon they are most likely to use.
				var weaponWielded = actor.eq(constants.WEAR_WIELD);
				var weapons = actor.inventory.filter(function(item) {
					return item.type == constants.ITEM_WEAPON;
				});

				if(weaponWielded)
				{
					if(weaponWielded.value(0) == constants.WEAPON_SHORT_BLADE) {
						self.say("I see that you are wielding a short blade, " + actor.name + ". Short blades are a thief's tool. You should use another weapon instead!");
						return;
					}
					else
					{
						var weaponData = weaponTypeToData[ weaponWielded.value(0) ];

						if(!weaponData)
						{
							mudLog(constants.BRF, 100, "Unknown weapon type `" + weaponWielded.value(0) + "` for item #" + weaponWielded.value(0) + ". script13224.");
							return;
						}

						if(actor.getSkill(getSkillVnum(weaponData.skillName)) > 0)
							return;

						self.say(actor.name + ", I see you are wielding a " + weaponData.name + ". You can " + bld + cyn + "PRACTICE " + weaponData.skillName.toUpperCase() + nrm + " here and I will teach you how to use it.");
					}
				}
			}
		}
		self.say("")
	}
};