JSRoom.prototype.lockDoor = function(dir, bOtherSide)
{
	if( this.doorExists(dir) && this.doorIsClosed(dir) ) {
		this.setDoorFlags( dir, this.doorFlags(dir) | (1<<2) );
	}
	if( bOtherSide ) {
			var otherRoom = this.direction(dir);
			var otherDir = revDir(dir);
			if( otherRoom && otherRoom.doorExists(otherDir) && otherRoom.doorIsClosed(otherDir) ) {
				otherRoom.lockDoor(otherDir,false);
			}
		}
}

