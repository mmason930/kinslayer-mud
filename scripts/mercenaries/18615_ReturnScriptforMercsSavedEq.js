var script18615 = function(self, actor, here, args, extra) {
	//This script allows a player to retrieve their saved eq from their mercs
	//Rhollor - December 2009
	//The number of files to check for data

	var maxMercs = 30;
	var fileName = (actor.id + ".mercEQ.");
	var vnums;
	var j, empty = 0;

	//TODO: This check valid?
	if(MercUtil.getMercenaryNumberArray(actor).length > 0) {
		//Still has mercs -> no possible way it crashed etc...
		actor.send("You cannot retrieve any belongings at this time.");
		return 0;
	}

	//TODO: We need a better way to determine which mercenaries the player had items for.
	var totalNotLoaded = 0;
	for(var i = 1; i <= maxMercs; i++) {

		var items =MercUtil.get

		//Check for non empty file
		if(vnums != "") {
			vnums = vnums.split(" ");
			var numNotLoaded = 0;
			for(j = 0; j < vnums.length - 1; j++) {
				var vNum = parseInt(vnums[j]);
				if (vNum > 2599 && vNum < 2650)
					numNotLoaded += 1;
				else if (vNum > 2895 && vNum < 2902)
					numNotLoaded += 1;
				else if (vNum > 11299 && vNum < 13100)
					numNotLoaded += 1;
				else if (vNum > 29999 && vNum < 31000)
					numNotLoaded += 1;
				else
					actor.loadObj(vNum);
			}
		}
		else {
			empty++;
		}
		totalNotLoaded += numNotLoaded;
	}
	if (totalNotLoaded == 1)
		actor.send("The mercenary lost 1 item in his travels.");
	else if (totalNotLoaded > 0)
		actor.send("The mercenary lost "+totalNotLoaded+" items in his travels.");
	if(empty != maxMercs) {
		echoaround(actor,actor.name + " receives his belongings.");
		actor.send("You receive all your belongings.");
	}
	else {
		actor.send("You have no belongings stored.");
	}
	return 0;
}