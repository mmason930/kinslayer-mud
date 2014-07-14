var script6401 = function(self, actor, here, args, extra)
{
	//Alder
//August 2010
//Quest: Bloody Gates
/** Command Trigger: paint (room 6481) **/
var qName = "Bloody Gates";
var hasJar = actor.checkQuestItem("a jar of thick blood",1,qName);
if ( actor.quest(qName) > 0 && hasJar) {
	var task;
	switch ( self.vnum ) {
		case 19301:
			task = 4;
			break;
		case 6481:
			task = 3;
			break;
		case 6401:
			task = 2;
			break;
		case 9398:
			task = 1;
			break;
	}
	if ( !actor.questTaskProgress(qName)[task+1].comp ) {
		_block;
		getCharCols(actor);
		actor.send(bld+cyn+"You dip your hand into a jar of thick blood and paint the gate with Trolloc chants."+nrm);
		waitpulse 7;
		actor.updateJournalTask(qName,task);
		waitpulse 5;
		if ( actor.questTaskProgress(qName)[0] == true ) {
			actor.remQuestItem("a jar of thick blood",-1,qName);
		}
	}
}

};

