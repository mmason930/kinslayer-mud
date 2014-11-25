function questCommandUsage(self,actor)
{
	var cyn = actor.cyan(constants.CL_OFF);
	var bld = actor.bold(constants.CL_OFF);
	var nrm = actor.normal(constants.CL_OFF);
	var ylw = actor.yellow(constants.CL_OFF);
	actor.send(" ");
	actor.send("You are currently interacting with a "+bld+"quest master"+nrm+": "+ylw+bld+self.name+nrm);
	actor.send("____________________________________________________________________________");
    actor.send(" ");
	actor.send("For a "+bld+"list"+nrm+" of available quests............Type: "+cyn+"quest list"+nrm);
    actor.send(" ");
	actor.send("For "+bld+"information"+nrm+" on a quest................Type: "+cyn+"quest info <QuestNumber>"+nrm);
	actor.send(" ");
	actor.send("To "+bld+"begin"+nrm+" a quest..........................Type: "+cyn+"quest begin <QuestNumber>"+nrm);
    actor.send(" ");
	actor.send("To "+bld+"complete"+nrm+" a quest.......................Type: "+cyn+"quest complete <QuestNumber>"+nrm);
	actor.send(" ");
	actor.send("To "+bld+"abandon"+nrm+" a quest........................Type: "+cyn+"quest abandon <QuestNumber>"+nrm);
	actor.send(" ");
	actor.send("To toggle quest "+bld+"greetings"+nrm+" on and off......Type: "+cyn+"quest greetings"+nrm);
	if(actor.room.zoneVnum == 132)
	{
		actor.send(" ");
		actor.send("To receive a "+bld+"map"+nrm+" of the area..............Type: "+cyn+"quest map please"+nrm);
	}
	actor.send("____________________________________________________________________________");
}

