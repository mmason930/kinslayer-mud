JSRoom.prototype.closeDoor = function(dir,bOtherSide)
{
	this.setDoorFlags( dir, this.doorFlags(dir) | (1<<1) );
	if( bOtherSide ) {
		var otherRoom = this.direction( dir );
		if( otherRoom ) {
			otherRoom.closeDoor( revDir(dir), false );
		}
	}
}
