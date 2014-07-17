function setupWorldDigItems() {
	mudLog(constants.BRF,102,"Loading dig items throughout the world...");
	global.allowedDigSectors = [
		constants.SECT_FIELD,
		constants.SECT_FOREST,
		constants.SECT_HILLS,
		constants.SECT_MOUNTAIN,
		constants.SECT_ROAD
	];//Must be sorted.
	constants.BURIED_ITEMS_ROOM_VNUM=11;
	var basicItems = [
		{ objectVnum:1, count:1  },
		{ objectVnum:2, count:1  },
		{ objectVnum:3, count:1  },
		{ objectVnum:4, count:10, zoneVnum: 85 }
	];
	
	//Cycle through the world list and produce a list only consisting of valid dig rooms.
	//Simultaneously, we need to go through and find the top and bottom of each zone.
	//zoneBottomIndex[ <zoneNumber> ] = validRoomsIndex.
	var validRooms = [];
	var zoneBottomIndexes = new Object();
	var zoneTopIndexes = new Object();
	var nextRoom = getRoomByRnum( 0 );
	var prevZoneVnum = null;
	var zoneOneOhTwoFound=false;
	for(var roomIndex = 0;nextRoom;++roomIndex) {
		var room = nextRoom;
		var nextRoom = getRoomByRnum( roomIndex+1 );
		if( canDigInRoom(room) && isZoneOpen(room.zoneVnum) ) {
			delete getDigItems(room);
			validRooms.push( room );
			
			var thisRoomZoneVnum = room.zoneVnum;
	
			if( prevZoneVnum != thisRoomZoneVnum ){ //We are now at the BOTTOM of a zone.
				zoneBottomIndexes[ thisRoomZoneVnum ] = roomIndex;	
			}
			if( !nextRoom ){ //We are now at the TOP of a zone.
				zoneTopIndexes[ thisRoomZoneVnum ] = roomIndex;
			}
			if( prevZoneVnum && prevZoneVnum != thisRoomZoneVnum ) //The previously viewed room was the top of a zone.
			{
				zoneTopIndexes[ prevZoneVnum ] = roomIndex-1;
			}
			
			prevZoneVnum = thisRoomZoneVnum;
		}
	}
	getRoom(98).echo("~~Top~~");
	for(var zNum in zoneTopIndexes) {
		getRoom(98).echo("Zone: " + zNum + " - " + getRoomByRnum(zoneTopIndexes[ zNum ]).vnum);
	}
	getRoom(98).echo("~~Bottom~~");
	for(var zNum in zoneBottomIndexes) {
		getRoom(98).echo("Zone: " + zNum + " - " + getRoomByRnum(zoneBottomIndexes[ zNum ]).vnum);
	}
	//validRooms should now be in sorted order by both vnum and index.
	return;
	
	for( var basicItemIndex = 0;basicItemIndex < basicItems.length;++basicItemIndex ) {
		var item = basicItems[ basicItemIndex ];
		
		while( item.count > 0 ) {
			
			var iIndexLow = 0;
			var iIndexHigh = validRooms.length-1;
			var room = validRooms[ random(iIndexLow,iIndexHigh) ];
			
			var obj = getRoom( constants.BURIED_ITEMS_ROOM_VNUM ).loadObj( item.objectVnum );
			addDigItem( obj, room );
			
			item.count--;
		}
	}
	mudLog(constants.BRF,102,"Dig items loaded.");
}
function canDigInRoom( room ) {
	return binarySearch( global.allowedDigSectors, room.sector ) != -1;
}
function getDigItems( room ) {
	var digItems = (room.digItems ? room.digItems : (room.digItems=[]));
	//Remove any object that may happen to be invalid.
	for(var digItemIndex = 0;digItemIndex < digItems.length;++digItemIndex) {
		if( !digItems[ digItemIndex ].isValid ) {
			digItems.splice(digItemIndex,1);
			--digItemIndex;
		}
	}
	return digItems;
}
function addDigItem( digItem, room ) {
	digItem.moveToRoom( getRoom( constants.BURIED_ITEMS_ROOM_VNUM ) );
	sortedInsert( getDigItems(room), digItem, function(e){return e.vnum;} );
}
function getRandomDigItem( room ) {
	var vDigItems = getDigItems( room );
	var itemIndex;
	
	if( !vDigItems || vDigItems.length == 0 )
		return null;
	return vDigItems[ random(0,vDigItems.length-1) ];
}
function getAndRemoveRandomDigItem( room ) {
	var vDigItems = getDigItems( room );
	var itemIndex;
	
	if( !vDigItems || vDigItems.length == 0 )
		return null;
	var itemIndex = random(0,vDigItems.length-1);
	var item = vDigItems[ itemIndex ];
	vDigItems.splice(itemIndex,1);
	
	return item;
}
