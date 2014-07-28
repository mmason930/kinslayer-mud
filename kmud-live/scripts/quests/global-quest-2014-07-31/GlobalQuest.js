function Global2014Util()
{
	this.pedestalObjectId = 23204;
	this.pedestalMaxHitPoints = 500;
	this.pedestalRoomIds = {};
	this.pedestalRoomIds[constants.RACE_HUMAN] = [1,2,3];
	this.pedestalRoomIds[constants.RACE_TROLLOC] = [4,7,6];

	this.pedestals = [];
}

Global2014Util.prototype.setupState3 = function()
{
	for(var race in this.pedestalRoomIds)
	{
		this.pedestalRoomIds[race].forEach(function(roomId) {

			getRoom(1).echo("Room ID: " + roomId);
		});
	}
};

Global2014Util.prototype.setupPedestal = function(race, roomId)
{
	var object = getRoom(roomId).loadObj(this.pedestalObjectId);

	var pedestal = new Global2014Pedestal(object, this.pedestalMaxHitPoints, race);

	mudLog(constants.BRF, 100, "Pedestal loaded in room #" + roomId + " for race " + raceNumToText(race) + ".");
};

global.global2014Util = new Global2014Util();
