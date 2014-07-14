var script101 = function(self, actor, here, args, extra)
{
	_block;
if( actor.level < 102 ) {
	actor.send("You can't use that!");
	return;
}
var vArgs = getArgList( args );
vArgs.splice(0,1);
if( vArgs.length == 0 ) {
	actor.send("Syntax: jstext <words>");
	actor.send("jstext will search for any JavaScrip that contains the supplied text.");
	return;
}
var newArgs = sqlEsc(vArgs.join(" "));
var qBuffer = "SELECT vnum,name,text FROM js_scripts WHERE text LIKE '%" + newArgs + "%' ORDER BY vnum ASC;";
var query = sqlQuery( qBuffer );
if( query.numRows == 0 ) {
	actor.send("There were no scripts that contained that text.");
	return;
}
getCharCols( actor );
actor.send("The following scripts contain the supplied text:\n");
while( query.hasNextRow ) {
	var row = query.getRow;
	var sVnum = row.get("vnum");
	sVnum = strPadding(sVnum, " ", 8, "left");
	actor.send(" [" + cyn + bld + sVnum + nrm + "] " + grn + bld + row.get("name") + nrm);
}

};

