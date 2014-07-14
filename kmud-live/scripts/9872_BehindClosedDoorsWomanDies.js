var script9872 = function(self, actor, here, args, extra)
{
	//Alder
//Behind Closed Doors: Woman Dies
	var qName = "Behind Closed Doors";
	if ( actor.quest(qName) > 0 ) {
		function f(arg) {
			var actor = arg[0];
			if ( actor.quest(qName) == 2 ) {
				actor.updateJournalTask(qName,0);
			}
			actor.qval(qName,4);
			actor.qval("NELLA_DEAD",1);
		}
		setTimeout(1,f,[actor]);
	}

};

