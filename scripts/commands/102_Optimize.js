var script102 = function(self, actor, here, args, extra) {
	/*********************************************
	 *                                           *
	 * Command: Optimize                         *
	 * Purpose: Optimize MySQL tables            *
	 *                                           *
	 * ~~~By: Galnor @01/26/2010                 *
	 *                                           *
	 *********************************************/
	_block;
	console.log("Testing 123.");
	if( actor.level < 103 ) {
		actor.send("You can't use that!");
		return;
	}
	var vArgs = getArgList( args );
	function showUsage( actor ) {
		actor.send("Syntax: Optimize MySQL");
	}
	function callback( vArguments ) {
		var cb = vArguments[0];
		var query = vArguments[1];
		if( query.hasNextRow ) {
			var row = query.getRow;
			sqlQuery("OPTIMIZE TABLE " + row.getByIndex(0));
			setTimeout( 8, cb, [cb,query,vArguments[2]] );
		}
		else {
			var actorInvis = vArguments[2];
			mudLog(constants.BRF, Math.max(102,actorInvis), "MySQL optimization complete.");
		}
	}
	if( vArgs.length < 2 ) {
		showUsage(actor);
		return;
	}
	if( str_cmp(vArgs[1], "mysql") ) {
		showUsage(actor);
		return;
	}
	mudLog(constants.BRF, Math.max(actor.invis,102), "MySQL optimization run by " + actor.name);
	var query = sqlQuery("SHOW TABLES;");
	callback( [callback,query,actor.invis] );
	
}