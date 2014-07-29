var script2896 = function(self, actor, here, args, extra) {
	_block;
	var itemArr = actor.inventory;
	// if (itemArr.length > 49) {
		// actor.send("You're holding too many items! Lose a few.");
		// return;
	// }
	for (i=0;i<itemArr.length;i++) {
		var nextItem = itemArr[i];
		if (nextItem.vnum < 2896 || nextItem.vnum > 2899)
			continue;
		var iRecType = nextItem.getPval("recType");
		var iRecVnum = nextItem.getPval("recVnum");
		var iExaView = nextItem.getPval("exaView");
		var newView = [];
		if (!iRecType || !iExaView || !iRecVnum) {
			//here.echo(nextItem.name + " failed");
			continue;
		}
		iExaView = iExaView.split("~");
		var views = iExaView.length;
		//here.echo("starting views = " +views);
		for (var _autoKey in itemArr) {
			var item = itemArr[_autoKey];
			var recType = item.getPval("recType");
			var recVnum = item.getPval("recVnum");
			var exaView = item.getPval("exaView");
			if (recType && recVnum && exaView) {
				exaView = exaView.split("~");
				if (recType == iRecType && recVnum == iRecVnum) {
					for (var _autoKey in exaView) {
						var corner = exaView[_autoKey];
						if (!arrContains(iExaView,corner) && arrCompare(iExaView,exaView).length < iExaView.length && arrCompare(iExaView,exaView).length < exaView.length ) {
							iExaView.push(corner); 
						}
					}
					//var newView = arrCompare(iExaView,exaView);
					//here.echo("ending views = "+views);
					if (iExaView.length > views) {
						if (iExaView.length == 2) {
							var newRecVnum = 2898;
							var aType = "fragment worn parchment";
						}
						else if (iExaView.length == 3) {
							var newRecVnum = 2897;
							var aType = "piece worn parchment";
						}
						else if (iExaView.length == 4) {
							var newRecVnum = 2896;
							var aType = "full sheet worn parchment";
						}
						iExaView = iExaView.sort();
						iExaView = iExaView.join("~");
						actor.send("You start folding the pieces together intricately...");
						act("$n starts fiddling with some pieces of parchment.", true, actor, null, null, constants.TO_ROOM);
						actor.startTimer(5);
						var success = runTimer(actor);
						if (!success) {
							actor.send("You give up the combination attempt.");
							return;
						}
						actor.loadObj(newRecVnum);
						var newObj = actor.inventory[0];
						if (recType == "M")
							var alias = "mixing";
						else if (recType == "S")
							var alias = "smithing";
						else if (recType == "T")
							var alias = "tailoring";
						//here.echo(iExaView);
						newObj.setPval("recType",iRecType,true);
						newObj.setPval("recVnum",iRecVnum,true);
						newObj.setPval("exaView",iExaView,true);
						var namelist = nextItem.namelist.split(" ");
						newObj.setRetoolName(namelist[0]+" "+namelist[1]+" "+namelist[2]+" "+aType+" "+alias);
						item.moveToRoom(getRoom(20899));
						//here.echo("combining "+item.getPval("exaView") + " - " + item.getPval("recVnum") +" with " +nextItem.getPval("exaView") + " - " + nextItem.getPval("recVnum")); 
						item.extract();
						nextItem.moveToRoom(getRoom(20899));
						nextItem.extract();
						actor.send("You finish combining the recipe.");
						act("The sound of crackling paper ceases as $n finishes $s work.", true, actor, null, null, constants.TO_ROOM);
						return;
					}
				}
			}
		}	
	}
	actor.send("You're not holding any parts that can be combined!");
	
	
	
}