var script23205 = function(self, actor, here, args, extra)
{
	var arg_list = getArgList(args);
actor.send("You push the big red button.");
here.echoaround(actor, actor.name + " pushes the big red button.");
_block;
if( arg_list.length > 1 && arg_list[1] == "button" )
{
    if( arg_list.length >= 5 )
    {
        getMyShip(getRoom(arg_list[2])).setShipCoordinates( arg_list[3], arg_list[4] );
        actor.send("Ship coordinates set to: " + arg_list[3] + ", " + arg_list[4] + ".");
    }
}
else if( arg_list.length > 1 && arg_list[1] == "ship_reset" )
{
	getRoom(23239).init = false;
	getRoom(23210).init = false;
}
else if( arg_list.length > 1 && arg_list[1] == "ship_init" )
{
	here.echo("Ship One: " + getRoom(23210).init);
	here.echo("Ship Two: " + getRoom(23239).init);
}
else if( arg_list.length > 1 && arg_list[1] == "modes" )
{
	if( arg_list.length < 3 ) {
		actor.send("You must specify the OLC type.");
		return;
	}
	var type = arg_list[2];
	here.echo("Modes for " + type + " : ");
	for(var i = 0;i < vOLC.length;++i) {
		if( !str_cmp(vOLC[i].type,type) ) {
			for(var m = 0;m < vOLC[i].modes.length;++m) {
				here.echo(vOLC[i].modes[m].mode);
			}
		}
	}
}
else if( arg_list.length > 1 && arg_list[1] == "sql" )
{
	var query = sqlQuery("SELECT COUNT(*) AS cnt FROM objects;");
	here.echo( "There are " + query.getRow.get("cnt") + " objects loaded in the game." );
/***
	var query = sqlQuery("SELECT * FROM users WHERE level >= 100 ORDER BY level DESC;");
	while( query.hasNextRow ) {
		var row = query.getRow;
		here.echo( row.get("name") + " - " + row.get("level") );
	}
***/
}
else if( arg_list.length > 1 && arg_list[1] == "qval" )
{
    if(arg_list.length < 4) {
	    actor.send("Which quest and to which value?");
		return;
	}
	actor.qval(arg_list[2], arg_list[3]);
}
else if( arg_list.length > 1 && arg_list[1] == "mudlog" )
{
	arg_list.shift();
	arg_list.shift();
	mudLog(2, 102, arg_list.join(" "));
}
else if( arg_list.length > 1 && arg_list[1] == "qreset" )
{
	if( arg_list.length < 3 ) {
		actor.send("Specify the owner's vnum.");
		return;
	}
	var target = null;
	if( arg_list.length < 4 )
		target = actor;
	else {
		target = actor.getCharVis( arg_list[3] );
	}
	if( !target ) {
		actor.send("Nobody by that name has been found.");
		return;
	}
	if( target.vnum != -1 ) {
		actor.send("Mobs do not have quests.");
		return;
	}
	var quests = getQuestsByMob( arg_list[2] );
	for each( q in quests ) {
		if( target.quest( q.questName ) )
			target.qval( q.questName, 0 );
		if( target.quest( q.questName + "-NR_COMPLETED" ) )
			target.qval( q.questName + "-NR_COMPLETED", 0 );
	}
	actor.send( capFirstLetter(actor.name) + "'s quests reset for owner #" + arg_list[2] + "." );
	actor.send("Number of quests reset: " + quests.length);
}
else if( arg_list.length > 1 && arg_list[1] == "exp" )
{
	var levelExp =
				[
					0, 1, 2000, 5000, 10000, 20000, 40000, 70000, 100000, 150000, 225000, 325000, 525000,
					825000, 1225000, 1725000, 2325000, 3025000, 3825000, 4725000, 5725000, 7000000, 9000000,
					12000000, 16000000, 21000000, 27000000, 34000000, 42000000, 51000000, 61000000, 71000000,
					81000000, 91000000, 101000000, 116000000, 131000000, 146000000, 161000000, 176000000, 196000000,
					216000000, 236000000, 256000000, 276000000, 296000000, 316000000, 336000000, 356000000, 376000000,
					396000000, 999999999
				];
	function totalReq( level ) {
		if( level >= 50 || level <= 0 )
			return -1;
		return levelExp[level];
	}
	function partialReq( level ) {
		if( level >= 50 || level <= 0 )
			return -1;
		return (levelExp[level+1] - levelExp[level]);
	}
	if( arg_list.length < 2 ) {
		actor.send("Specify the level you wish to view.");
		return;
	}
	var nr = Number(arg_list[2]);
	var div = Number(arg_list[3]);
	if( !nr || nr <= 0 || nr >= 50 )
	{
		actor.send("Invalid level. Must be between 1 and 49.");
		return;
	}
	if( !div ) {
		here.echo("EXP required for level " + nr + " : " + partialReq(nr));
	} else {
		here.echo("EXP required for level " + nr + " / " + div + " : " + (partialReq(nr)/div));
	}
}
else
    _noblock;

};

