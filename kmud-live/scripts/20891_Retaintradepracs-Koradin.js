var script20891 = function(self, actor, here, args, extra)
{
	function afterWait(args) {
	var self = args[0];
	var actor = args[1];
	for each (var person in self.room.people) {
		var rank = person.getSpecTitle();
		if (rank) {
			var title = rank.title.split(" ");
			var type = rank.category;
			if (title[1] && type == "trades") {
				var rankName = title[0];
				var tradeName = title[1];
				if (rankName == "Apprentice")
					var level = 10;
				else if (rankName == "Journeyman")
					var level = 40;
				else if (rankName == "Master")
					var level = 75;
				else if (rankName == "Artisan")
					var level = 99;
				if (tradeName == "Miner")
					var trade = "Mining";
				else if (tradeName == "Herbalist")
					var trade = "Herbalism";
				else if (tradeName == "Angler")
					var trade = "Fishing";
				else if (tradeName == "Carver")
					var trade = "Skinning";
				else if (tradeName == "Chef")
					var trade = "Cooking";
				else if (tradeName == "Blacksmith")
					var trade = "Smithing";
				else if (tradeName == "Tailor")
					var trade = "Tailoring";
				else if (tradeName == "Alchemist")
					var trade = "Mixing";
				var currentSkill = person.getSkill(getSkillVnum(trade));
				if (currentSkill < level && currentSkill > 0) {
					self.say("Why, "+person.name+"! A tradesman of your quality deserves better!");
					getCharCols(person);
					person.send(bld+capFirstLetter(self.name)+" awards you some trade experience in '"+nrm+cyn+trade+nrm+bld+"'!"+nrm);
					person.setSkill(getSkillVnum(trade),level);
				}
			}
		}
	}
}
setTimeout(1,afterWait,[self,actor]);

};

