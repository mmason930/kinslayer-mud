var script10637 = function(self, actor, here, args, extra)
{
	//Jenkin 
//September 2010
//Quest: Shienaran Expedition
var qName = 'The Shienaran Expedition';
if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[3].comp ) {
     waitpulse 5;
     self.say("Intelligence? But I sent that a week past by tracker.");
     waitpulse 14;
     self.say("You'll have to find the soldier to get your hands on that information.");
     waitpulse 21;
     self.say("I sent him through Stedding Qichen. Last I heard he was resting in the village there.");
     waitpulse 21; 
     self.say("If he didn't reach Fal Dara he's either dead or injured.");
     waitpulse 21;
     self.say("Search for wise women or herbalists. He may yet still live.");
}

};

