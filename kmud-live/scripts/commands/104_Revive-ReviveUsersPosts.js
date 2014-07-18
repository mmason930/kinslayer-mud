var script104 = function(self, actor, here, args, extra) {
	if( actor.level < 103 ) { 
		return;
	}
	_block;
	var vArgs = getArgList( args );
	var types = ["post_author", "post_count"];
	if( vArgs.length < 2 ) {
		actor.send("Syntax: Revive <type> <target>");
		actor.send("Possible types: " + types.join(", "));
		return;
	}
	var type = vArgs[1];
	var found = false;
	for each(var t in types) {
		if( t== type.toLowerCase() ) {
			found = true;
			break;
		}
	}
	if( !found ) {
		actor.send("Invalid type.");
		actor.send("Possible types: " + types.join(" "));
		return;
	}
	var nrArgs = vArgs.length;
	if( !str_cmp(type, "post_author") )
	{
		if( nrArgs < 4 )
		{
			actor.send("Syntax: revive post_author <postID> <userName>");
			return;
		}
		if( !isNumber( vArgs[2] ) ) {
			actor.send("Post ID must be numeric.");
			return;
		}
		
		var postID = parseInt( vArgs[2] );
		var userName = sqlEsc( capFirstLetter(vArgs[3].toLowerCase()) );
		
		var sql 	= " SELECT user_id"
				+ " FROM users"
				+ " WHERE username = " + sqlEscapeQuoteString(userName);
		
		var userQuery = sqlQuery(sql);
		if( userQuery.numRows == 0 ) {
			actor.send("There is no user by that name.");
			return;
		}
		var userID = parseInt(userQuery.getRow.get("user_id"));
		sqlQuery("UPDATE phpbb_posts p  SET poster_id='" + userID + "' WHERE post_id='" + postID + "';");
		mudLog(constants.NRM, Math.max(actor.invis,102), actor.name + " has made " + userName + " the owner of post #" + postID + ".");
	}
	else if( !str_cmp(type, "post_count") )
	{
		if( nrArgs < 3 ) {
			actor.send("Syntax: revive post_count {all | <playerName>}");
			return;
		}
		var userName = capFirstLetter(vArgs[2].toLowerCase());
		if( !str_cmp(userName, "all") ) {
			sqlQuery("UPDATE phpbb_users u SET u.user_posts=(SELECT COUNT(*) FROM phpbb_posts p WHERE p.poster_id=u.user_id GROUP BY p.poster_id LIMIT 1);");
			mudLog(constants.NRM, Math.max(actor.invis,102), actor.name + " has resynchronized all user post counts.");
		} else {
			var postQuery = sqlQuery("SELECT COUNT(u.user_id) AS cnt, u.user_id FROM phpbb_posts p JOIN phpbb_users u ON u.user_id=p.poster_id WHERE u.username='" + sqlEsc(userName) + "' GROUP BY u.user_id;");
			if( postQuery.numRows == 0 ) {
				actor.send("That user does not exist.");
				return;
			}
			var row = postQuery.getRow;
			var nrPosts = parseInt(row.get("cnt"));
			var userID = parseInt(row.get("user_id"));
			
			sqlQuery("UPDATE users SET user_posts='" + nrPosts + "' WHERE user_id='" + userID + "';");
			mudLog(constants.NRM, Math.max(actor.invis,102), actor.name + " has revived " + userName + "'s post count. New post count: " + nrPosts + ".");
		}
	}
	
}