var script13224 = function(self, actor, here, args, extra) {
	waitpulse 8;
	var className = classNumToText(self.class);
	var heShe = self.sex == constants.SEX_MALE ? "he" : "she";
	var exampleSkillName = "RIDE";
	if(self.class == constants.CLASS_RANGER)
		exampleSkillName = "TRACK";
	else if(self.class == constants.CLASS_THIEF)
		exampleSkillName = "SHORT BLADES";
	else if(self.class == constants.CLASS_CHANNELER)
		exampleSkillName = "EARTH";
	getCharCols(actor);
	if(self.class != constants.CLASS_CHANNELER || (actor.class == constants.CLASS_CHANNELER && actor.sex == self.sex)) {
		actor.send( "The " + bld + red + className + nrm + " trainer is in this room. You can use the " + cyn + "PRACTICE" + nrm + " command to see a list of skills\n"
				+	"that " + heShe + " can teach you. To practice a skill, simply type \"practice\" followed by the skill name.\n"
				+	"For example: " + cyn + "PRACTICE " + exampleSkillName + nrm + "\n");
	}
}