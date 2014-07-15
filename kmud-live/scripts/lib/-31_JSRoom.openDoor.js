JSRoom.prototype.openDoor = function(dir,bBothSides)
{
	if( this.doorExists(dir) ) {
		var flags = this.doorFlags(dir);
		flags &= ~(1<<1);
		this.setDoorFlags( dir, flags );
	}
	if( bBothSides ) {
		var otherRoom = this.direction( dir );
		if( otherRoom ) {
			otherRoom.openDoor( revDir(dir,false) );//Open the connected room's door, but only one-way.
		}
	}
}
