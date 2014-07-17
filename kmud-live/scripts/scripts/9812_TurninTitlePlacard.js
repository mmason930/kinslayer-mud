var script9812 = function(self, actor, here, args, extra) {
	// Alder
	// February 2011
	/** Turn in items for feat point shop (title placards and retools) **/
	waitpulse 1;
	var item = extra.obj;
	var type = "";
	if ( item.vnum == 9809 ) {
		type = "prefix";
	}
	else if ( item.vnum == 9810 ) {
		type = "normal";
	}
	else {
		if ( actor.featRetool ) {
			if ( item.canWear(actor.featRetool.type) ) {
				if ( actor.featPoints < actor.featRetool.cost ) {
					self.tell(actor, "Looks like you don't have enough feat points to retool this item. Come back when you have "+intToText(actor.featRetool.cost-actor.featPoints)+" more.");
				}
				else if ( !actor.featRetool.complete ) {
					self.tell(actor, "I'm still waiting for you to tell me the remaining information for your item retool.");
				}
				else if ( item.isRetooled() || item.extraFlags(constants.ITEM_RARE) ) {
					self.tell(actor, "This item cannot be retooled.");
				}
				else {
					actor.addFeatPoints(-actor.featRetool.cost);
					// Name
					item.setRetoolSDesc(actor.featRetool.name);
					item.setPval("featRetool "+wearPosToString(actor.featRetool.type)+" name", actor.featRetool.name, true);
					// Room description
					item.setRetoolDesc(actor.featRetool.roomDesc);
					item.setPval("featRetool "+wearPosToString(actor.featRetool.type)+" roomDesc", actor.featRetool.roomDesc, true);
					// Alias list
					item.setRetoolName(actor.featRetool.aliases);
					item.setPval("featRetool "+wearPosToString(actor.featRetool.type)+" aliases", actor.featRetool.aliases, true);
					mudLog(3,103,"A feat points retool has been made: -check retools- at room 9800.");
					self.comm("give "+getArgList(item.namelist)[0]+" "+actor.name);
					self.tell(actor, "Pleasure doing business with you.");
					delete actor.featRetool;
					return;
				}
			}
		}
		self.comm("give "+getArgList(item.namelist)[0]+" "+actor.name);
		return;
	}
	var title = item.name.split(":");
	if ( title.length == 1 ) {
		self.tell(actor,"You might wanna put some writing on there before you hand that in to me.");
		self.comm("give TITLE_PLACARD "+actor.name);
		return;
	}
	title.shift();
	item.extract();
	self.tell(actor,"Alright, I'll see what I can do.");
	title = title.join(" ");
	title = title.substr(1);
	actor.unlockTitle(title,type,"silly");
	return;
}