//Attempt 1 at a declanning script that integrates forum user groups into the process
//Rhollor April 2010
/**
Returns true if success, false if failed.
**/
JSCharacter.prototype.declan = function(clanNum) {
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
	clanNum = sqlEsc(clanNum);
	var qBuffer = "SELECT userClan.* FROM userClan LEFT JOIN users ON userClan.user_id=users.user_id WHERE users.username LIKE '" + sqlEsc(this.name) + "' " + " AND userClan.clan_id = " + sqlEsc(clanNum) + ";";
	var query = sqlQuery(qBuffer);
	if(query.numRows) {
		/* For DECLAN this means they ARE a member so are ready for declan */
		var mob = getMobAtRoom(84,84);
		mob.comm("declan " + this.name + " " + clanNum);
		this.send("You have been removed from the " + bld + clanName + nrm + ".");
		
		if(clanGrp[clanNum - 1] != -1) {
			/* Delete from clan forum group */
			qBuffer = "SELECT user_id FROM users WHERE username LIKE '" + sqlEsc(this.name) + "';";
			if ( sqlQuery("SELECT * FROM phpbb_user_group WHERE user_id = " + sqlEsc(this.id) + " AND group_id = " + sqlEsc(clanGrp[clanNum -1]) + ";").numRows != 0 ) {
				//Already in this user group so DELETE
				qBuffer = "DELETE FROM phpbb_user_group WHERE user_id = " + sqlEsc(this.id) + " AND group_id = " + sqlEsc(clanGrp[clanNum -1]) + ";";
				sqlQuery(qBuffer);
				mudLog(1,this.level,"JS-64: Removed " + capFirstLetter(this.name) + " from the " + clanName + " clan forum."); 
				this.send("You have been removed from the " + bld + clanName + nrm + " phpbb.");
			}
			else {
				//Wasnt in the user group
			}
		}
		return true;
	}
	else {
		/* For DECLAN this means they AREN'T a member anyway so they don't need to be declanned... */
		return false;
	}	
}


