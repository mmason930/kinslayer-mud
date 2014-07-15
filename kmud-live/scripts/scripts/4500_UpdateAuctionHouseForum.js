var script4500 = function(self, actor, here, args, extra) {
	here.echo(".-=*AUCTION FORUM UPDATING*=-.");
	var HUMAN_AUCTIONEER_ID = 33487;
	var TROLLOC_AUCTIONEER_ID = 33513;
	var AUCTION_THREAD_ID = 4446;
	var guardian = getMobAtRoom(500, 500);
	var sQuery = "";
	var qResult;
	sQuery = "SELECT * FROM auctions";
	qResult = sqlQuery(sQuery);
	while(qResult.hasNextRow) {
		var row = qResult.getRow;
		
		var aucVnum = parseInt( row.get("vnum") );
		var aucName = row.get("name");
		var aucRace = parseInt( row.get("aRaces") );
		
		/* Default Auctioneer is Rhollor */
		var auctioneerID = 760;
		if(isBitSet(aucRace, constants.RACE_TROLLOC) ) {
			auctioneerID = TROLLOC_AUCTIONEER_ID;
		}
		else if(isBitSet(aucRace, constants.RACE_HUMAN) ) {
			auctioneerID = HUMAN_AUCTIONEER_ID;
		}
		
		var oldPostID = fread(auctioneerID + "_last_post");
		if(oldPostID) {
	//		here.echo("Removing post: " + oldPostID);
			removeForumPost(oldPostID);
		}
		
		var sPost = "<font size = 5><b>Current Items Available at " + aucName + ":</b></font>\n\n";
		
		sQuery = "SELECT * FROM auction_items WHERE active = 1 AND auction_id = " + sqlEsc(aucVnum) + " ORDER BY starting_price DESC";
		var qResult1 = sqlQuery(sQuery);
		//sPost += "[list]";
		if(!qResult1.hasNextRow)
			sPost += "No items are currently at this auction.";
		else
			var count = 1;
		while(qResult1.hasNextRow) {
			var row1 = qResult1.getRow;
			var ownerID = parseInt( row1.get("owner_id") ); 
			var objectID = row1.get("object_id");
			var start = parseInt(row1.get("starting_price"));
			var buyout = parseInt(row1.get("buyout_price"));
			var current = "none";
			var temp = sqlQuery("SELECT * FROM auction_bids WHERE object_id LIKE '" + sqlEsc(objectID) + "'");
			var bidTime = 0;
			if(temp.hasNextRow) {
				temp = temp.getRow;
				current = guardian.numCopperToText(parseInt( temp.get("bid_amount") ), false);
				bidTime = temp.get("timestamp");
			}
			var now = new Date();
			temp = (parseInt(row1.get("end_time") + parseInt(row1.get("timestamp"))) - now.getTime());
			var hour = (temp/(1000*60*60))%24;
			var timeLeft = parseInt(hour) + " hours and " + parseInt(((temp - hour)/(1000*60))%60) + " minutes remaining";
			
			
			var owner = sqlQuery("SELECT * FROM users WHERE id = " + sqlEsc(ownerID)).getRow;
			var ownerName = owner.get("name");
			var ownerID = owner.get("id");
			var obj = getObjProto(parseInt( sqlQuery("SELECT vnum FROM objects WHERE id LIKE '" + sqlEsc( objectID ) + "'").getRow.get("vnum") ) );
			
			//This will fail for any item with no vnum(Scalp, corpse)
			if( obj ) {
				sPost += "<font size=2>" + count + ". " + ownerName + " has <i><b>" + obj.name + "</b> </i>listed </font>" + 
					"<UL><LI>Remaining Time: " + timeLeft +
					"<LI>Starting Price: " + guardian.numCopperToText(start, false) + 
					"<LI>Buyout Price: " + guardian.numCopperToText(buyout, false) +
					"<LI>Current Bid: " + current + "</UL>\n";
					if(current != "none" && bidTime != 0 && (time() - bidTime) < 1800) {
						sendPM(auctioneerID, ownerID, "You have a bid on a " + obj.name + ".", "There has been a recent bid on " + obj.name + " that you listed.\n\nThe current bid is " + current + ".\n\n<i>Please do not respond to this message.</i>");
					}
			
				count++;
			}
		}
		//sPost += "[/list]";
		
		var postID = createForumPost( auctioneerID, AUCTION_THREAD_ID, sPost);
	//	here.echo("Post ID: " + postID);
		fwrite(auctioneerID + "_last_post", postID);
	}
}