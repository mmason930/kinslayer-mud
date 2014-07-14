//Boot OLC
var vOLC = [];
/**************************************************************************
 *                                                                        *
 * bootGateditOLC()                                                         *
 * Arguments: <None>                                                      *
 * Purpose: Initiates and adds the data structures for the editor         *
 *          menu to the main OLC vector.                                  *
 * ~~~ By: Galnor 05/22/2010                                              *
 *                                                                        *
 **************************************************************************/
function bootGateditOLC()
{
	var oConfig = new Object();
	var mode;
	oConfig.type = "gatedit";
	oConfig.modes = [];
	/**** Main Menu ****/
	mode = new Object();
	mode.mode = "MODE_MAIN";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( vArgs.length && isNumber( vArgs[ 0 ] ) ) {
			var id = parseInt( vArgs[0] );
			var gateKeeperObject = getGateKeeperByID( id );
			
			if( gateKeeperObject ) {
				actor.getOLC().gateKeeperObject = gateKeeperObject;
				actor.getOLC().switchToMode("MODE_EDIT_GATE_KEEPER");
			}
			else {
				actor.send("There is no entry with the ID you specified. Either enter an existing ID or choose another option.");
			}
			return;
		}
		if( fLetter == "A" ) {
			actor.getOLC().switchToMode( "MODE_ADD_GATE_KEEPER");
		}
		else if( fLetter == "D" ) {
			actor.getOLC().switchToMode( "MODE_DELETE_GATE_KEEPER");
		}
		else if( fLetter == "Q" ) {
			actor.cleanupOLC();
		}
		else {
			actor.send("Invalid option.\nTry again: ");
		}
	};
	mode.display = function(actor)
	{
		getCharCols(actor);
		/*** Grab the potions from OLC ***/
		var buffer = "";
		var vGateKeepers = getGateKeeperVector();
		for(var gateKeeperIndex = 0;gateKeeperIndex < vGateKeepers.length;++gateKeeperIndex) {
			var gateKeeperObject = vGateKeepers[ gateKeeperIndex ];
			buffer += bld + cyn + gateKeeperObject.databaseID + nrm + ") " + yel + (gateKeeperObject.gateKeeperName ? gateKeeperObject.gateKeeperName : "<None>") + nrm + "\n";
		}
		actor.send("~~~Gate Keeper Editor~~~\n");
		actor.send( buffer );
		actor.send(bld + grn + "A" + nrm + ") Add New Gate Keeper");
		actor.send(bld + grn + "D" + nrm + ") Delete Gate Keeper");
		actor.send(bld + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push( mode );
	
	mode = new Object();
	mode.mode = "MODE_ADD_GATE_KEEPER";
	mode.parser = function(actor,fLetter,vArgs)
	{
		actor.getOLC().gateKeeperObject = new Object();
		actor.getOLC().gateKeeperObject.gateKeeperName = vArgs.join(" ");
		actor.getOLC().switchToMode("MODE_EDIT_GATE_KEEPER");
	}
	mode.display = function(actor)
	{
		actor.send("Enter the name to identify this gate keeper : ");
	}
	oConfig.modes.push( mode );
	
	mode = new Object();
	mode.mode = "MODE_EDIT_GATE_KEEPER";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( vArgs.length > 0 )
		{
			if( vArgs[ 0 ] == "1" ) {
				if( actor.getOLC().gateKeeperObject.databaseID != undefined ) {
					actor.send("This entry already has an ID. It may only be set once.");
					return;
				}
				actor.getOLC().switchToMode("MODE_GATE_KEEPER_DATABASE_ID");
				return;
			}
			if( vArgs[ 0 ] == "2" ) {
				actor.getOLC().switchToMode("MODE_GATE_KEEPER_NAME");
				return;
			}
			if( vArgs[ 0 ] == "3" ) {
				actor.getOLC().switchToMode("MODE_GATE_KEEPER_MOB_VNUM");
				return;
			}
			if( vArgs[ 0 ] == "4" ) {
				actor.getOLC().switchToMode("MODE_GATE_KEEPER_MAIN_ROOM");
				return;
			}
			if( vArgs[ 0 ] == "5" ) {
				actor.getOLC().switchToMode("MODE_GATE_KEEPER_OTHER_ROOM");
				return;
			}
			if( vArgs[ 0 ] == "6" ) {
				actor.getOLC().switchToMode("MODE_GATE_KEEPER_PULSES_TO_CLOSE");
				return;
			}
			if( vArgs[ 0 ].toUpperCase() == "Q" ) {
				actor.getOLC().switchToMode("MODE_CONFIRM_SAVE");
				return;
			}
		}
		actor.send("Invalid option.\r\nTry again : ");
	}
	mode.display = function(actor)
	{
		getCharCols(actor);
		var gateKeeperObject = actor.getOLC().gateKeeperObject;
		var gateKeeperName = gateKeeperObject.gateKeeperName;
		var gateKeeperMob = (gateKeeperObject.gateKeeperVnum != undefined ? getMobProto( gateKeeperObject.gateKeeperVnum ) : null);
		var gateKeeperMainRoom = (gateKeeperObject.gateKeeperRoomVnum != undefined ? getRoom( gateKeeperObject.gateKeeperRoomVnum ) : null);
		var gateKeeperOtherRoom =(gateKeeperObject.otherRoomVnum != undefined ? getRoom( gateKeeperObject.otherRoomVnum ) : null);
		var gateKeeperPulses = (gateKeeperObject.pulsesToWaitForClose != undefined ? gateKeeperObject.pulsesToWaitForClose : 0);
		actor.send( bld + grn + "1" + nrm + ") Database ID(must be unique) : " + yel + (gateKeeperObject.databaseID ? gateKeeperObject.databaseID : "<None>") + nrm);
		actor.send( bld + grn + "2" + nrm + ") Name                        : " + yel + (gateKeeperName ? gateKeeperName : "<None>") + nrm);
		actor.send( bld + grn + "3" + nrm + ") Gate Keeper MOB             : " + yel + (gateKeeperMob ? "[ " + gateKeeperMob.vnum + " ] " + gateKeeperMob.name : "<None>") + nrm);
		actor.send( bld + grn + "4" + nrm + ") Gate Keeper Main Room       : " + yel + (gateKeeperMainRoom ? "[ " + gateKeeperMainRoom.vnum + " ] " + gateKeeperMainRoom.name : "<None>") + nrm);
		actor.send( bld + grn + "5" + nrm + ") Gate Keeper Joint Room      : " + yel + (gateKeeperOtherRoom ? "[ " + gateKeeperOtherRoom.vnum + " ] " + gateKeeperOtherRoom.name : "<None>") + nrm);
		actor.send( bld + grn + "6" + nrm + ") Pulses Before Gate Closes   : " + yel + (gateKeeperPulses) + nrm);
		actor.send("");//Newline sent.
		actor.send( bld + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push( mode );
	mode = new Object();
	mode.mode = "MODE_GATE_KEEPER_DATABASE_ID";
	mode.parser = function(actor,fLetter,vArgs)
	{
		var id = vArgs.length ? parseInt(vArgs[0]) : -1;
		if( getGateKeeperByID( id ) ) {
			actor.send("There is already a gate keeper with that ID. Try another.");
		}
		else {
			actor.getOLC().gateKeeperObject.databaseID = id;
			actor.getOLC().switchToMode("MODE_EDIT_GATE_KEEPER");
		}
	}
	mode.display = function(actor)
	{
		actor.send("Enter the unique ID number for this gate keeper : ");
	}
	oConfig.modes.push( mode );
	
	mode = new Object();
	mode.mode = "MODE_GATE_KEEPER_NAME";
	mode.parser = function(actor,fLetter,vArgs)
	{
		actor.getOLC().gateKeeperObject.gateKeeperName = vArgs.join(" ");
		actor.getOLC().switchToMode("MODE_EDIT_GATE_KEEPER");
	}
	mode.display = function(actor)
	{
		actor.send("Enter a new name for this gate keeper : ");
	}
	oConfig.modes.push( mode );
	
	mode = new Object();
	mode.mode = "MODE_GATE_KEEPER_MOB_VNUM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( vArgs.length < 1 || !isNumber(vArgs[0]) )
		{
			actor.send("Input is invalid. You must enter a virtual number.\r\nTry again : ");
			return;
		}
		var vnum = parseInt(vArgs[0]);
		if( !getMobProto( vnum ) ) {
			actor.send("There is no mob with that vnum.\r\nTry again : ");
			return;
		}
		actor.getOLC().gateKeeperObject.gateKeeperVnum = vnum;
		actor.getOLC().switchToMode("MODE_EDIT_GATE_KEEPER");
	}
	mode.display = function(actor)
	{
		actor.send("Enter the virtual number for the gate keeper mob : ");
	}
	oConfig.modes.push( mode );
	mode = new Object();
	mode.mode = "MODE_GATE_KEEPER_MAIN_ROOM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( vArgs.length < 1 || !isNumber(vArgs[0]) )
		{
			actor.send("Input is invalid. You must enter a virtual number.\r\nTry again : ");
			return;
		}
		var vnum = parseInt(vArgs[0]);
		if( !getRoom( vnum ) ) {
			actor.send("There is no room with that vnum.\r\nTry again : ");
			return;
		}
		actor.getOLC().gateKeeperObject.gateKeeperRoomVnum = vnum;
		actor.getOLC().switchToMode("MODE_EDIT_GATE_KEEPER");
	}
	mode.display = function(actor)
	{
		actor.send("Enter a vnum for the room in which the gate keeper resides : ");
	}
	oConfig.modes.push( mode );
	mode = new Object();
	mode.mode = "MODE_GATE_KEEPER_OTHER_ROOM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( vArgs.length < 1 || !isNumber(vArgs[0]) )
		{
			actor.send("Input is invalid. You must enter a virtual number.\r\nTry again : ");
			return;
		}
		var vnum = parseInt(vArgs[0]);
		if( !getRoom( vnum ) ) {
			actor.send("There is no room with that vnum.\r\nTry again : ");
			return;
		}
		actor.getOLC().gateKeeperObject.otherRoomVnum = vnum;
		actor.getOLC().switchToMode("MODE_EDIT_GATE_KEEPER");
	}
	mode.display = function(actor)
	{
		actor.send("Enter a vnum for the room for the room linking to the gate keeper's room : ");
	}
	oConfig.modes.push( mode );
	mode = new Object();
	mode.mode = "MODE_GATE_KEEPER_PULSES_TO_CLOSE";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( vArgs.length < 1 || !isNumber(vArgs[0]) )
		{
			actor.send("Input is invalid. You must enter a numeric value.\r\nTry again : ");
			return;
		}
		var iPulses = parseInt(vArgs[0]);
		if( iPulses <= 0 ) {
			actor.send("Input must be above zero.\r\nTry again : ");
			return;
		}
		actor.getOLC().gateKeeperObject.pulsesToWaitForClose = iPulses;
		actor.getOLC().switchToMode("MODE_EDIT_GATE_KEEPER");
	}
	mode.display = function(actor)
	{
		actor.send("Enter the number of pulses that will go by before the gate will close : ");
	}
	oConfig.modes.push( mode );
	mode = new Object();
	mode.mode = "MODE_CONFIRM_SAVE";
	mode.parser = function(actor,fLetter,vArgs)
	{
		var vGateKeepers = getGateKeeperVector();
		var index = -1;
		for(index = 0;index < vGateKeepers.length;++index) {
			if( vGateKeepers[ index ].databaseID == actor.getOLC().gateKeeperObject.databaseID )
				break;
		}
		if( fLetter == "Y" ) {
			if( index != vGateKeepers.length ) {//There is a gate keeper already in the array.
				vGateKeepers.splice(index,1);
			}
			addGateKeeper( actor.getOLC().gateKeeperObject );
			saveGateKeeperToDatabase( actor.getOLC().gateKeeperObject );
			actor.getOLC().gateKeeperObject = undefined;
			actor.getOLC().switchToMode("MODE_MAIN");
		}
		else if( fLetter == "N" ){
			delete actor.getOLC().gateKeeperObject;
			actor.getOLC().switchToMode("MODE_MAIN");
		}
		else {
			actor.send("Input must be 'Y' to save or 'N' not to save.");
		}
	}
	mode.display = function(actor)
	{
		actor.send("Do you wish to save your changes? (Y/N) : ");
	}
	oConfig.modes.push( mode );
	
	
	mode = new Object();
	mode.mode = "MODE_DELETE_GATE_KEEPER";
	mode.parser = function(actor,fLetter,vArgs)
	{
		var vGateKeepers = getGateKeeperVector();
		if( vArgs.length < 1 || !isNumber(vArgs[0]) ) {
			actor.send("Your input must be numeric.");
			return;
		}
		var id = parseInt( vArgs[0] );
		var gateKeeperObject = getGateKeeperByID( id );
		if( !gateKeeperObject ) {
			actor.send("No entry was found with that ID.");
		}
		else {
			removeGateKeeper( id );
			removeGateKeeperFromDatabase( id );
		}
		actor.getOLC().switchToMode( "MODE_MAIN" );
	}
	mode.display = function(actor)
	{
		actor.send("Enter the ID of the entry you wish to delete : ");
	}
	oConfig.modes.push( mode );
	
	vOLC.push( oConfig );
}
bootOLC();
