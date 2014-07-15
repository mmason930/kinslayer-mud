var GATE_KEEPER_TABLE = "gatekeepers";
function getGateKeeperVector()
{
	if( !global.vGateKeepers )
		global.vGateKeepers = [];
	return global.vGateKeepers;
}
function loadAllGateKeepers()
{
	mudLog(constants.NRM, 102, "Booting all gate keepers...");
	var vGateKeepers = getGateKeeperVector();
	var sQuery = "SELECT * FROM " + GATE_KEEPER_TABLE + ";";
	var rs = sqlQuery( sQuery );
	while( vGateKeepers.length )
		vGateKeepers.pop();
	
	while( rs.hasNextRow ) {
		var row = rs.getRow;
		bootSingleGateKeeper( row );
	}
	mudLog(constants.NRM, 102, "All gate keepers have been loaded...");
}
function saveGateKeeperToDatabase( gateKeeperObject )
{
	if( gateKeeperObject.databaseID == undefined ) {
		mudLog(constants.BRF,102,"Unable to save gate keeper entry. There is an invalid ID set on the entry.");
		return;
	}
	var sQuery = "REPLACE DELAYED INTO " + GATE_KEEPER_TABLE + " SET"
		+ " id=" + sqlEsc(gateKeeperObject.databaseID) + ","
		+ " gateKeeperName='"      + (gateKeeperObject.gateKeeperName ? sqlEsc(gateKeeperObject.gateKeeperName) : "NULL") + "',"
		+ " gateKeeperVnum="       + (gateKeeperObject.gateKeeperVnum ? sqlEsc(gateKeeperObject.gateKeeperVnum) : "NULL") + ","
		+ " gateKeeperRoomVnum="   + (gateKeeperObject.gateKeeperRoomVnum ? sqlEsc(gateKeeperObject.gateKeeperRoomVnum) : "NULL") + ","
		+ " otherRoomVnum="        + (gateKeeperObject.otherRoomVnum ? sqlEsc(gateKeeperObject.otherRoomVnum) : "NULL") + ","
		+ " pulsesToWaitForClose=" + (gateKeeperObject.pulsesToWaitForClose ? sqlEsc(gateKeeperObject.pulsesToWaitForClose) : "NULL");
	sqlQuery( sQuery );
}
function saveAllGateKeepers()
{
	mudLog(constants.NRM, 102, "Saving all gate keepers...");
	var vGateKeepers = getGateKeeperVector();
	for(var i = 0;i < vGateKeepers.length;++i)
	{
		saveGateKeeperToDatabase( vGateKeepers[ i ] );
	}
	mudLog(constants.NRM, 102, "All gate keepers have been saved...");
}
function bootSingleGateKeeper( gateKeeperRow )
{
	var vGateKeepers = getGateKeeperVector();
	var gateKeeperObject = new Object();
	gateKeeperObject.gateKeeperName			= gateKeeperRow.get("gateKeeperName");
	gateKeeperObject.databaseID				= parseInt( gateKeeperRow.get("id") );
	gateKeeperObject.gateKeeperVnum			= parseInt( gateKeeperRow.get("gateKeeperVnum") );
	gateKeeperObject.gateKeeperRoomVnum		= parseInt( gateKeeperRow.get("gateKeeperRoomVnum") );
	gateKeeperObject.otherRoomVnum			= parseInt( gateKeeperRow.get("otherRoomVnum") );
	gateKeeperObject.pulsesToWaitForClose	= parseInt( gateKeeperRow.get("pulsesToWaitForClose") );
	addGateKeeper( gateKeeperObject );
}
function cloneGateKeeperObject( gateKeeperObject )
{
	var clone = new Object();
	clone.gateKeeperName = gateKeeperObject.gateKeeperName;
	clone.databaseID = gateKeeperObject.databaseID;
	clone.gateKeeperVnum = gateKeeperObject.gateKeeperVnum;
	clone.gateKeeperRoomVnum = gateKeeperObject.gateKeeperRoomVnum;
	clone.otherRoomVnum = gateKeeperObject.otherRoomVnum;
	clone.pulsesToWaitForClose = gateKeeperObject.pulsesToWaitForClose;
	return clone;
}
function addGateKeeper( gateKeeperObject )
{
	var vGateKeepers = getGateKeeperVector();
	
	if( typeof gateKeeperObject.databaseID == "undefined" ) {
		mudLog(constants.NRM, 102, "ERROR : Attempting to add a gate keeper with an undefined ID.");
		return;
	}
	for(var i = 0;i < vGateKeepers.length;++i) {
		if( vGateKeepers[ i ].databaseID > gateKeeperObject.databaseID ) {
			vGateKeepers.splice(i,0,gateKeeperObject);
			return;
		}
	}
	vGateKeepers.push( gateKeeperObject );
}
function removeGateKeeper( iGateKeeperID )
{
	var vGateKeepers = getGateKeeperVector();
	for(var i = 0;i < vGateKeepers.length;++i) {
		if( vGateKeepers[ i ].databaseID == iGateKeeperID ) {
			vGateKeepers.splice(i,1);
			return;
		}
	}
}
function removeGateKeeperFromDatabase( iGateKeeperID )
{
	var sql = "DELETE FROM gatekeepers WHERE id='" + sqlEsc( iGateKeeperID ) + "';";
	sqlQuery( sql );
}
function getGateKeeperByID( iGateKeeperID )
{
	var vGateKeepers = getGateKeeperVector();
	for(var i = 0;i < vGateKeepers.length;++i) {
		if( vGateKeepers[ i ].databaseID == iGateKeeperID ) {
			return vGateKeepers[ i ];
		}
	}
	return null;
}
