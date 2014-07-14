var script9870 = function(self, actor, here, args, extra)
{
	//Alder
var qName = "Behind Closed Doors";
if ( actor.quest(qName) == 2 ) {
	self.moveToRoom(getRoom(501));
	getCharCols(actor);
	actor.send(bld+"The woman yields, knowing you have control of the situation."+nrm);
	waitpulse 10;
	actor.send(cyn+bld+"You demand she tell you who she is and what she was doing here."+nrm);
	waitpulse 14;
	actor.send(bld+"The woman gasps, 'My name is Nella Dierdic.'");
	actor.updateJournalTask(qName,0);
	self.moveToRoom(getRoom(10333));
	waitpulse 10;
	self.comm("say If you know what is good for you, you will not kill me. I can help you.");
	waitpulse 10;
	actor.send(bld+"The woman grins slightly, eyeing you closely."+nrm);
	waitpulse 14;
	actor.send(bld+cyn+"You have obtained adequate information for Zind. Either kill the woman, leave her, or inquire about her 'help'."+nrm);
	self.hps = 5;
	actor.qval(qName,3);
	return;
}

};

