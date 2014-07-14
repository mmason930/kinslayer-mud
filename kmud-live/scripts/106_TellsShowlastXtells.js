var script106 = function(self, actor, here, args, extra)
{
	_block;
var vArgs = getArgList(args);
var start = 0;
actor.send("Syntax: tells [nrToShow] [start]");
if( vArgs.length == 1 ) {
	var start = 0;
	var nrToGet = 25;
}
else if( vArgs.length == 2 ) {
	var start = 0;
	var nrToGet = sqlEsc(parseInt(vArgs[1]));
}
else if( vArgs.length >= 3 ) {
	var nrToGet = sqlEsc(parseInt(vArgs[1]));
	var start = sqlEsc(parseInt(vArgs[2]));
}
nrToGet = Math.min(100,nrToGet);
nrToGet = Math.max(1,nrToGet);
if( !isNumber(nrToGet) ) {
	actor.send("Number of tells to fetch must be numeric.");
	return;
}
if( !isNumber(start) ) {
	actor.send("Starting point must be numeric.");
	return;
}
var sql = " SELECT"
		+ "   comm.invis_level,"
		+ "   comm.sender_id,"
		+ "   comm.timestamp,"
		+ "   comm.message"
        + " FROM comm"
		+ " WHERE comm.type='tell'"
		+ " AND comm.recipient_type='C'"
		+ " AND comm.recipient_id = " + actor.id
		+ " AND comm.sender_type='C'"
		+ " ORDER BY comm.timestamp DESC"
		+ " LIMIT " + start + "," + nrToGet;

var result = sqlQuery( sql );
getCharCols(actor,constants.CL_NORMAL);
while( result.hasNextRow ) {
	var row = result.getRow;
	
	var senderInvisLevel = parseInt(row.get("invis_level"));
	if( senderInvisLevel > actor.level )
		var senderName = "Someone";
	else
		var senderName = getUserNameByUserId(row.get("sender_id"));
	
	var dDate = new Date( parseInt(row.get("timestamp"))*1000 );
	actor.send( grn + "[" + dDate.strftime("%a %m-%d-%Y %H:%M:%S") + "] " + nrm + senderName + ": " + bld + cyn + row.get("message") + nrm );
}



};

