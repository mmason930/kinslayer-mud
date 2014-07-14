var script10636 = function(self, actor, here, args, extra)
{
	var qName = 'The Shienaran Expedition';
var hasPackage = actor.checkQuestItem("a large package of provisions",1,qName);
if ( actor.questInProgress(qName) && !actor.questTaskProgress(qName)[2].comp && hasPackage ) {
     _block;
     waitpulse 5;
     getCharCols(actor);
     actor.send(bld+cyn+"You untie a large package of provisions and lower it from your back."+nrm);
     waitpulse 14;
     actor.send("A Shienaran Blight scout turns and appraises you silently."+nrm);
     waitpulse 14;
     actor.send("A Shienaran Blight scout inclines his head to you in greeting.");
     waitpulse 21;
     actor.send("A Shienaran Blight scout claps his hands and says, 'Tracker! Collect provisions!'");
     waitpulse 21;
     actor.send("A grizzled tracker rushes over and pushes the package against the wall.");
     waitpulse 21;
     actor.send("A Shienaran Blight scout says, 'About flaming time! A whole company to feed'");
     waitpulse 21;
     actor.send("A Shienaran Blight scout says, 'in bloody no man's land! Open it up!'");
     waitpulse 10;
     actor.updateJournalTask(qName, 1);
     actor.remQuestItem("a large package of provisions",-1,qName);
     
}

};

