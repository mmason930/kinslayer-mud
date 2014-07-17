//Clanning script for OFFLINE users
//Rhollor May 2010
/**
Returns true if success, false if failed.
**/
var clanOffline = function(userName, clanNum) {
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
	
	var clanName = clanNumToText(clanNum);
	
	//hash table kinda thing for clan num -> grp ID where index is clan num and value is grp id
	var clanGrp = [-1,QG_GRP_ID,BD_GRP_ID,MC_GRP_ID,-1,VL_GRP_ID,KO_GRP_ID,-1,DJ_GRP_ID,-1,GB_GRP_ID,-1,BM_GRP_ID,BT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,WT_GRP_ID,SL_GRP_ID,-1,DG_GRP_ID,MY_GRP_ID,-1]; //All for now	
	
	//Check to see if "actor" is already in clan clanNum
	var id = null;
	clanNum = sqlEsc(clanNum);
	userName = sqlEsc(capFirstLetter(userName));
	var qBuffer = "SELECT userClan.* FROM userClan LEFT JOIN users ON userClan.user_id=users.user_id WHERE users.username LIKE '" + userName + "' " + " AND clan_id = " + sqlEsc(clanNum) + ";";
	var query = sqlQuery(qBuffer);
	if(query.numRows) {
		return false;
	}
	else {
		//If no rows are returned that means they aren't clanned in actor clan
		/* Add "actor" to the clan in the DB */
		if(clanNum > clanGrp.length || clanNum < 1) {
			//Unused clan or clan with no forum
			mudLog(1,100,"JSError in -69: Attempting to add " + userName + " to a non-existant clan.");
			return 0;
		}
		qBuffer = "SELECT user_id FROM users WHERE username LIKE '" + userName + "';";
		query = sqlQuery(qBuffer);
		if(query.numRows) {
			//The actor does exist
			id = query.getRow.get("user_id");
			qBuffer = "INSERT INTO userClan (user_id, clanned_datetime, last_ranked_datetime, clan_id, quest_points, rank, is_council) VALUES ('" + id + "', " + sqlEncodeQuoteDate(new Date()) + ", " + sqlEncodeQuoteDate(new Date()) + ", '" + sqlEsc(clanNum) + "', '0', '1', '0');";
			sqlQuery(qBuffer);
			mudLog(1,100, "JS-73: Added " + userName + " to " + clanName + " (Offline).");
		}
		
		if(clanGrp[clanNum - 1] != -1) {
			/* Add to clan forum group */
			qBuffer = "SELECT user_id FROM users WHERE username LIKE '" + userName + "';";
			if(sqlQuery("SELECT * FROM phpbb_user_group WHERE user_id = " + sqlEsc(id) + " AND group_id = " + sqlEsc(clanGrp[clanNum -1]) + ";").numRows != 0) {
				//Already in this user group
			}
			else {
				qBuffer = "INSERT INTO phpbb_user_group (group_id, user_id, user_pending) VALUES ('" + sqlEsc(clanGrp[clanNum - 1]) + "','" + sqlEsc(id) + "','" + sqlEsc(0) + "');";
				sqlQuery(qBuffer);
				mudLog(1,100,"JS-73: Added " + userName + " to the " + clanName + " clan forum (Offline)."); 
			}
		}
		return true;
	}	
}



