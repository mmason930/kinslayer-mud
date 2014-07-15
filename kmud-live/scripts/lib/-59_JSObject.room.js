JSObject.prototype.__defineGetter__("room", function()
{
	if( this.inRoom )
		return this.inRoom;
	var holder = this.findHolder;
	if( holder ) {
		if( holder.room )
			return holder.room;
		if( holder.inRoom )
			return holder.inRoom;
	}
	return undefined;
}
);
