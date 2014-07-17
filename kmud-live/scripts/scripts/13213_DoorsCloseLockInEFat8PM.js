var script13213 = function(self, actor, here, args, extra) {
	//	mudLog(3, 103, "Room #" + here.vnum + " - " + getSval(self,13213, "Ready"));
		var tolerance = random(-2,2);//The doors will close at different times between 5pm and 11pm
		/***DOORS CLOSE AROUND 8 PM***/
		//If time is right and this room hasn't handled doors yet
		if ( ( getHour() == (20 + tolerance) || getHour() == (22) ) && (getSval(self, 13213, "Ready") == "no" || !getSval(self, 13213, "Ready")) )
		{
	//		mudLog(constants.BRF, 102, "Closing door in room #" + here.vnum);
			setSval(self, 13213, "Ready", "yes");
			if ( self.people )//If anyone is in the room, teleport the owner of the room, kick everyone else out
			{
				for ( var i = 0; i < 6; i++ )
				{
					if ( ( self.direction(i) ) && ( self.direction(i).sector == constants.SECT_ROAD || self.direction(i).sector == constants.SECT_FIELD ) )//If examined room is FIELD or ROAD do the following
					{
						var targetRoom = self.direction(i);//This room is where people will get dumped in a moment
						var oppDoorDir = i;
						break;
					}
				}	
				var people = self.people;//People in room
				var owner = null;
				for ( var p = 0; p < people.length; p++ )
				{
					if( isName("teleport", people[p].namelist) )
					{
						owner = people[p];
						if ( owner.followers ) {
							for ( var f = 0; f < owner.followers; f++ )//Teleport the mob's followers
								owner.followers[f].moveToRoom( getRoom(13200) );
						}
						owner.moveToRoom( getRoom(13200) );
						setSval( owner, 13213, "Home", self );
						here.echo( capFirstLetter( owner.name ) + " begins to put things away before sending you out into the open air." );
						break;
					}
				}
				//Loop again, this time kicking everyone out.
				people = self.people;
				for( var p = 0; p < people.length; p++ )
				{
					if (people[p] && targetRoom) {
						people[p].moveToRoom( targetRoom );//Send current person to outside room
						people[p].comm("look");
						people[p].send("The " + targetRoom.doorName( revDir(oppDoorDir) ) + " is closed from the other side.");
					}
				}
			}
			//Close all doors, lock shop doors
			for ( var i = 0; i < 6; i++ )
			{
				if ( self.doorExists(i) )
				{
					self.closeDoor(i);
					self.direction(i).closeDoor( revDir(i) );
					if ( self.doorPick(i) > 0 )
					{
						self.lockDoor(i);
						self.direction(i).lockDoor( revDir(i) );
					}
				}
			}
		}
		/***DOORS OPEN AROUND 8 AM***/
		
		//If time is right and this room hasn't handled doors yet
		else if ( ( getHour() == (8 + tolerance) || getHour() == (10) ) && (getSval(self, 13213, "Ready") == "yes"|| !getSval(self, 13213, "Ready")) )
		{
	//		mudLog(constants.BRF, 102, "Opening door in room #" + here.vnum);
			setSval(self, 13213, "Ready", "no");
			var people = getRoom(13200).people;
			var owner = null;
			for ( var p = 0; p < people.length; p++ )
			{
				var person = people[p];
				if ( getSval( person, 13213, "Home" ) == self )//If this person's home room is this room
				{
					if ( people[p].followers ) {
								for ( var f = 0; f < people[p].followers; f++ )//Teleport the mob's followers
									people[p].followers[f].moveToRoom( self );
					}
					person.moveToRoom( self );
					here.echo( capFirstLetter( person.name ) + " gets ready for the day ahead and welcomes the outside air by opening the door.");
					owner = person;
				}
			}
			//Unlock shop doors, open all doors, shop owners yell when doors are open
			for ( var i = 0; i < 6; i++ )
			{
				if ( self.doorExists(i) && str_cmp( self.direction(i).name,"The Weapon Shop Storage Room" ) == true )
				{
					if( self.doorIsLocked( i ) ) {
						self.unlockDoor(i);
						self.direction(i).unlockDoor( revDir(i) );
						if( owner )
							owner.comm("yell " + capFirstLetter(self.name) + " is now open for the day!");
					}
					self.openDoor(i);
					self.direction(i).openDoor( revDir(i) );
				}
			}
		}
	
}