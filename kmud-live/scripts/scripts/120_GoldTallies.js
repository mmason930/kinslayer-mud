var script120 = function(self, actor, here, args, extra) {
	global.timeOfLastGoldTally = undefined;
	if( typeof global.timeOfLastGoldTally == "undefined" ) {
		var rs = sqlQuery("SELECT timestamp FROM tallies WHERE name='goldTotal' ORDER BY timestamp DESC LIMIT 1;");
		if( rs.hasNextRow ) {
			var row = rs.getRow;
			global.timeOfLastGoldTally = parseInt(row.get("timestamp"));
		} else {
			global.timeOfLastGoldTally = 0;
		}
	}
	//Enough time has elapsed.
	if( time() - global.timeOfLastGoldTally >= (60*60*24) ) {
		var rs = sqlQuery("SELECT SUM(gold+bank_gold) AS moneyTotal FROM users WHERE level < 100;");
		var row = rs.getRow;
		
		sqlQuery("INSERT INTO tallies(`name`,`value`,`timestamp`) VALUES('goldTotal','" + row.get("moneyTotal") + "','" + time() + "');");
		global.timeOfLastGoldTally = time();
		
	}
}