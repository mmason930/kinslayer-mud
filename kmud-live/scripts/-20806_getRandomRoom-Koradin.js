//Returns a random valid room in the mud that is in an open zone.
//Koradin - May 2010
function getRandomRoom()
{
	var attempt = 100;
	while (attempt > 0)
	{
		var room = getRoomByRnum( random(0, numberOfRooms()-1) );
		if (room)
		{
			if (isZoneOpen(room.zoneVnum))
				return room;
		}
		attempt -= 1;
	}
	return null;
}
