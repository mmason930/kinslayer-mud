//Alder December 2009
/**
Returns -1 if no doors in room have names that match supplied string.
Returns int dir if a door in room has name that matches supplied string.
**/
		JSRoom.prototype.doorName_cmp = function( strDoorName ) {
			/**0=north, 1=east...**/
			for ( var i = 0; i < 6; i++ )
			{
				if ( this.doorExists(i) )
				{
					if ( !str_cmp( this.doorName(i), strDoorName ) )//Check next dir if supplied name is not door name
						return i;//Return direction int if supplied name is door name
				}
			}
			return -1;//The supplied door name did not match any doors in the room
		}
