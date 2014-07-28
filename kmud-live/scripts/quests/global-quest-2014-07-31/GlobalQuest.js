function Global2014Util()
{
	this.pedestalRoomIds = {};
	this.pedestalRoomIds[constants.RACE_HUMAN] = [1,2,3];
	this.pedestalRoomIds[constants.RACE_TROLLOC] = [4,5,6];

	this.pedestals = [];
}

Global2014Util.prototype.setupState3 = function()
{
	for(var race in this.pedestalRoomIds)
	{
		var roomId = this.pedestalRoomIds[race];

		getRoom(1).echo("Room ID: " + roomId);
	}
};

