function bootProcs()
{
	mudLog(constants.BRF, 102, "bootProcs() is running...");
	if( !global.questsHaveBeenBooted ) {
		mudLog(constants.BRF, 102, "Start of quests...");
		loadAllQuests();
		mudLog(constants.BRF, 102, "End of quests...");
		global.questsHaveBeenBooted = true;
	}
	if( !global.achievementsHaveBeenBooted ) {
		mudLog(constants.BRF, 102, "Start of achievements...");
		loadAllAchievements();
		mudLog(constants.BRF, 102, "End of achievements...");
		global.achievementsHaveBeenBooted = true;
	}
	if( !global.recipesHaveBeenBooted ) {
		mudLog(constants.BRF, 102, "Start of recipes...");
		loadAllRecipes();
		mudLog(constants.BRF, 102, "End of recipes...");
		global.recipesHaveBeenBooted = true;
	}
	if( !global.gateKeepersHaveBeenBooted ) {
		mudLog(constants.BRF, 102, "Start of gatekeepers...");
		loadAllGateKeepers();
		mudLog(constants.BRF, 102, "End of gatekeepers...");
		global.gateKeepersHaveBeenBooted = true;
	}
	if( !global.helpManager ) {
		mudLog(constants.BRF, 102, "Start of help manager...");
		global.helpManager = new HelpManager();
		mudLog(constants.BRF, 102, "End of help manager...");
	}
	
	if( !global.webSocketCommandProcessor ) {
		global.webSocketCommandProcessor = new WebSocketCommandProcessor();
	}

	if( !global.hasAddedUsersToRegisteredUsersUsergroup ) {
		mudLog(constants.BRF, 102, "Start of adding users to registered usergroup...");
	//Newly registered users need to be added to the "Registered Users" usergroup in order to have basic forum permissions.
	//We will do this here every time the MUD boots up.
		var sql = "";
		
		sql +=  "SELECT users.user_id";
		sql	+= " FROM users";
		sql += " LEFT JOIN phpbb_user_group ON ( users.user_id = phpbb_user_group.user_id";
		sql += " AND phpbb_user_group.group_id =2 )";
		sql += " WHERE phpbb_user_group.user_id IS NULL";
		sql += " AND users.user_id NOT IN(8)";
		var vUserInserts = [];
		var vUserIDs = [];
		
		var resultSet = sqlQuery( sql );
		
		while( resultSet.hasNextRow ) {
		
			var row = resultSet.getRow;
			vUserInserts.push( "(2," + row.get("user_id") + ",0,0)" );
			vUserIDs.push( row.get("user_id") );
		}
		
		if( vUserIDs.length > 0 ) {
			sql = "";
			
			sql += "INSERT INTO phpbb_user_group (";
			sql += " group_id,"
			sql += " user_id,"
			sql += " group_leader,"
			sql += " user_pending )";
			sql += " VALUES ";
			sql += vUserInserts.join(",");
			
			sqlQuery( sql );
			
			sql = "";
			sql += "UPDATE users";
			sql +=" LEFT JOIN phpbb_config ON phpbb_config.config_name='default_style'"
			sql +=" SET users.user_style=phpbb_config.config_value"
			sql +=" WHERE users.user_id IN(" + vUserIDs.join(",") + ")";
			
			sqlQuery( sql );
			
			sql = "";
			sql += "UPDATE users"
			sql +=" SET users.username_clean=LOWER(users.username)"
			sql +=" WHERE users.username_clean!=LOWER(users.username)";
			
			sqlQuery( sql );
			
			sql = "";
			sql += "UPDATE users ";
			sql += "SET user_dateformat='F jS, Y, g:i a' ";
			sql += "WHERE user_dateformat=''";
			
			sqlQuery( sql );
		}

		mudLog(constants.BRF, 102, "End of adding users to registered usergroup...");
		
		global.hasAddedUsersToRegisteredUsersUsergroup = true;
	}
	if(!global.hasUpdatedPostUsernames) {

		mudLog(constants.BRF, 102, "Start of forum username update...");
		sql = "UPDATE"
			+ "  phpbb_posts,"
			+ "  userArchive "
			+ "SET"
			+ "  phpbb_posts.post_username=userArchive.username "
			+ "WHERE"
			+ "  phpbb_posts.poster_id=userArchive.user_id;";
		sqlQuery(sql);
		  
		sql = "UPDATE"
			+ " phpbb_posts,"
			+ " users "
			+ "SET"
			+ "  phpbb_posts.post_username=users.username "
			+ "WHERE"
			+ "  phpbb_posts.poster_id=users.user_id;";
			
		sqlQuery(sql);

		mudLog(constants.BRF, 102, "End of forum username update...");
		mudLog(constants.CMP, 102, "[BootProcs] Forum post usernames updated.");
		global.hasUpdatedPostUsernames = true;
	}

	mudLog(constants.BRF, 102, "Start of OLC...");
	bootOLC();
	mudLog(constants.BRF, 102, "End of OLC...");
}





