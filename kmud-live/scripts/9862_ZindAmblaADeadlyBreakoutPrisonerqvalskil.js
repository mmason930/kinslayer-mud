var script9862 = function(self, actor, here, args, extra)
{
		if ( actor.quest("A Deadly Breakout") == 1 ) {
		function afterDeath( arg ) {
			var qName = "A Deadly Breakout";
			var actor = arg[0];
			var items = actor.room.items;
			for ( var i = 0; i < items.length; i++ ) {
				var item = items[i];
				if ( !str_cmp(item.name,"the corpse of an imprisoned thief") ) {
					var corpse = item;
				}
			}
			actor.updateJournalTask("A Deadly Breakout",0);
			if ( actor.checkQuestItem("the corpse of an imprisoned thief",1,qName) == false ) {
				actor.send("You stoop down and pull the corpse of an imprisoned thief over your shoulder.");
				actor.room.echoaround(actor,capFirstLetter(actor.name) + " stoops down and pulls the corpse of an imprisoned thief over "+actor.his_her()+" shoulder.");
				actor.addQuestItem(corpse.name,1,qName);
				corpse.extract();
			}
		}
		setTimeout(1,afterDeath,[actor]);
	}

};

