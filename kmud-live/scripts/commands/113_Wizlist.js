var script113 = function(self, actor, here, args, extra) {
	return;
	var vArgs = getArgList(args);
	var all = false;
	_block;
	if( vArgs.length > 0 && !str_cmp(vArgs[1],"all") )
		all = true;
	//var bit = constants.PLR_NOWIZLIST;
	var bit = 12;
	var sql = "SELECT "
	        +" users.username,"
			+" users.level, "
			+" users.last_logon "
			+" FROM users "
			+" WHERE users.level >= 100 "
			+" AND !(plr & ( 1 << " + bit + " )) "
		    +" ORDER BY level DESC;";
	var query = sqlQuery(sql);
	var vImms = new Object();
	var iTopImmLevel = 105;
	var iBotImmLevel = 100;
	for(var iLevel = iTopImmLevel;iLevel >= iBotImmLevel;--iLevel) {
		vImms[ iLevel ] = [];
		while( query.hasNextRow ) {
			var row = query.getRow;
			actor.send( row.get("users.username") );
			var iUserLevel = parseInt(row.get("users.level"));
			if( iUserLevel != iLevel )
				break;
			var iDaysSinceLastLogin = Math.floor((time() - parseInt("users.last_logon")) / 60 / 60 / 24);
			var sUserName = row.get("users.username");
			var sBuffer = "";
			
			sBuffer += sUserName;
			
			vImms[ iLevel ].push( sBuffer );
		}
	}
	actor.send( vImms[ 105 ].join(", ") );
}