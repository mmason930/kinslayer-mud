var script85 = function(self, actor, here, args, extra) {
	//Rhollor January 2010
	args = getArgList(args);
	function clan (charName, clanNum) {
		if(actor.level < 103) {
			return 0;
		}
		/** Set up forum group IDs corresponding to clan name **/
		const KO_GRP_ID = 6;	//Ko'bal
		const BD_GRP_ID = 7;	//Bordguard
		const QG_GRP_ID = 8;	//Queen Guard
		const VL_GRP_ID = 9;	//Valon
		const GB_GRP_ID = 10;	//Ghob'hlin
		const DJ_GRP_ID = 11;	//Dhjin'nen
		const MC_GRP_ID = 23;	//Murandian Cavalry
		const WT_GRP_ID = 26;	//White Tower
		const BT_GRP_ID = 27;	//Black Tower
		const DG_GRP_ID = 28;	//Dreadguard
		const SL_GRP_ID = 69;	//Soulless
		const MY_GRP_ID = 52;	//Myrddraal
		const BM_GRP_ID = 65;	//Blademaster
		
		//hash table kinda thing for clan num -> grp ID where index is clan num and value is grp id
		var clanGrp = [-1,QG_GRP_ID,BD_GRP_ID,MC_GRP_ID,-1,VL_GRP_ID,KO_GRP_ID,-1,DJ_GRP_ID,-1,GB_GRP_ID,-1,-1,BM_GRP_ID,BT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,SL_GRP_ID,-1,DG_GRP_ID,MY_GRP_ID,-1]; //All for now	
		
		/** Attempt to determine clan from clanNum **/
		if(isNumber(clanNum)) {
			if(clanNum > clanGrp.length || clanNum < 1) {
				//Unused clan or clan with no forum
				mudLog(1,100,"JSError in 85: Attempting to add " + capFirstLetter(charName) + " to a non-existant clan.");
				return 0;
			}
		}
		else {
			//Is it a name of a clan???
			actor.send("might be a name");
			return 0;
		}
			//Check to see if "actor" is already in clan clanNum
		clanNum = sqlEsc(clanNum);
		var qBuffer = "SELECT * FROM userClan WHERE name LIKE '" + sqlEsc(charName) + "' " + " AND clan = " + sqlEsc(clanNum) + ";";
		var query = sqlQuery(qBuffer);
		if(query.numRows) {
			actor.send( capFirstLetter(charName) + " is already clanned in " + clanNumToText(clanNum) + ".");
			return 0;
		}
		else {
			//If no rows are returned that means they aren't clanned in actor clan
			/* Add "actor" to the clan in the DB */
			qBuffer = "INSERT INTO userClan (name, clan_time, rank_time, clan, qps, rank, council) VALUES ('" + sqlEsc(capFirstLetter(charName)) + "','" + sqlEsc(time()) + "','" + sqlEsc(time()) + "','" + clanNum + "','" + sqlEsc(0) + "','" + sqlEsc(1) + "','" + sqlEsc(0) + "');"
			sqlQuery(qBuffer);
			
			if(clanGrp[clanNum - 1] != -1) {
				/* Add to clan forum group */
				qBuffer = "SELECT user_id FROM users WHERE username LIKE '" + sqlEsc(charName) + "';";
				var id = sqlQuery(qBuffer).getRow.get("id");
				if(sqlQuery("SELECT * FROM forums_user_group WHERE id = " + sqlEsc(id) + " AND group_id = " + sqlEsc(clanGrp[clanNum -1]) + ";").numRows != 0) {
					//Already in this user group
				}
				else {
					qBuffer = "INSERT INTO forums_user_group (group_id, id, user_pending) VALUES ('" + sqlEsc(clanGrp[clanNum - 1]) + "','" + sqlEsc(id) + "','" + sqlEsc(0) + "');";
					sqlQuery(qBuffer);
					mudLog(1,actor.level,capFirstLetter(actor.name) + " added " + capFirstLetter(charName) + " to the " + clanNumToText(clanNum) + " clan forum."); 
				}
			}
			mudLog(1,actor.level,capFirstLetter(actor.name) + " clanned " + capFirstLetter(charName) + " into clan " + clanNumToText(clanNum) + "."); 
		}
	}
	clan(args[1],args[2]);
	return 0;
	
}