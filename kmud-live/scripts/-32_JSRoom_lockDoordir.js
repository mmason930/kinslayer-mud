JSRoom.prototype.unlockDoor = function(dir,bOtherSide)
{
	if( this.doorExists(dir) && this.doorIsClosed(dir) ) {
		var flags = this.doorFlags(dir);
		flags &= ~(1<<2);
		this.setDoorFlags( dir, flags );
		
		if( bOtherSide ) {
			var otherRoom = this.direction(dir);
			var otherDir = revDir(dir);
			if( otherRoom && otherRoom.doorExists(otherDir) && otherRoom.doorIsClosed(otherDir) ) {
				otherRoom.unlockDoor(otherDir,false);
			}
		}
	}
}
