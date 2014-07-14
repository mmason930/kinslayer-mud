var script385 = function(self, actor, here, args, extra)
{
	//Alder
//August 2010
//Quest: Deliverance
/** Command: 'deliver' **/
var qName = 'Deliverance';
if ( actor.quest(qName) && actor.questTaskProgress(qName)[1].comp && actor.checkQuestItem("Murash's order of attack on Caemlyn",1,qName) ) {
        _block;
        waitpulse 7;
        self.say("Finally we can carry out the plans we've been devising.");
        waitpulse 14;
        self.say("In order to take the city of Caemlyn we are going to have to dispatch the leader of its defenses: the Queen's Guard captain.");
        waitpulse 21;
        self.say(actor.name+". You'll be at the head of the group, scouting for the captain. Once you've found him we'll join you in the fray to take him out.");
        waitpulse 28;
        self.say("Now move out! We'll come to your aid once you've found the captain "+actor.name+".");
        actor.updateJournalTask(qName,1);
        here.loadMob(2002).comm("follow fade");
        here.loadMob(2002).comm("follow fade");
        here.loadMob(2002).comm("follow fade");
        setSval(actor,385,"DELIVERANCE_FADE",self);
        // self.attach(386);
        actor.remQuestItem("Murash's order of attack on Caemlyn",-1,qName)
}

};

