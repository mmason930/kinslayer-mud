//Attempt 1 at a clanning script that integrates forum user groups into the process
//Rhollor January 2010
/**
Returns true if success, false if failed.
**/
JSCharacter.prototype.clan = function(clanNum) {
	
	var clanName = clanNumToText(clanNum);
	//Check to see if "actor" is already in clan clanNum
	clanNum = parseInt(sqlEsc(clanNum));
	var qBuffer = "SELECT * FROM userClan LEFT JOIN users ON userClan.user_id=users.user_id WHERE users.username LIKE '" + sqlEsc(this.name) + "' " + " AND userClan.clan_id = " + sqlEsc(clanNum) + ";";
	// this.send("qBuffer: " + qBuffer);
	var query = sqlQuery(qBuffer);
	if(query.numRows) {
		return false;
	}
	else {
		/*** REMOVE BECAUSE IT DOESNT DO IT LIVE ***/
		var mob = getMobAtRoom(84,84);
		mob.comm("clan " + this.name + " " + clanNum);
		this.send("You have been clanned into " + bld + clanName + nrm + ".");
		
		/* Add to clan forum group */
		
		var clanGroupNumber = getForumGroupIDFromClanVnum( clanNum );
//Galnor 2010-11-16 - Removing the forum access until we finish it up.
//		if( clanGroupNumber != undefined ) {
//			addUserToForumGroup( this.id,  clanGroupNumber);
//		}
		// qBuffer = "SELECT user_id FROM users WHERE username LIKE '" + sqlEsc(this.name) + "';";
		// if(sqlQuery("SELECT * FROM phpbb_user_group WHERE user_id = " + sqlEsc(this.id) + " AND group_id = " + sqlEsc(clanGrp[clanNum -1]) + ";").numRows != 0) {
			// //Already in this user group
		// }
		// else {
			// qBuffer = "INSERT INTO phpbb_user_group (group_id, user_id, user_pending) VALUES ('" + sqlEsc(clanGrp[clanNum - 1]) + "','" + sqlEsc(this.id) + "','" + sqlEsc(0) + "');";
			// sqlQuery(qBuffer);
			// mudLog(1,this.level,"JS-69: Added " + capFirstLetter(this.name) + " to the " + clanName + " clan forum."); 
			// this.send("You have been added to the " + bld + clanName + nrm + " phpbb.");
		// }
		return true;
	}	
}


