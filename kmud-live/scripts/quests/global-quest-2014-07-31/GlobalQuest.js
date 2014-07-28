function Global2014Util()
{
	this.pedestalObjectId = 23204;
	this.pedestalMaxHitPoints = 500;
	this.pedestalRoomIds = {};
	this.pedestalRoomIds[constants.RACE_HUMAN] = [1,2,3];
	this.pedestalRoomIds[constants.RACE_TROLLOC] = [4,7,6];

	this.objectIdToPedestalMap = {};
}

Global2014Util.prototype.setupStage3 = function()
{
	var self = this;
	this.objectIdToPedestalMap = {};

	for(var race in this.pedestalRoomIds)
	{
		this.pedestalRoomIds[race].forEach(function(roomId) {

			getRoom(roomId).getObjects(function(self.pedestalObjectId)).forEach(function(oldPedestal) {

				mudLog(constants.BRF, 100, "Extracting old pedestal in room #" + roomId + ".");
			});

			self.setupPedestal(race, roomId);
		});
	}
};

Global2014Util.prototype.setupPedestal = function(race, roomId)
{
	var object = getRoom(roomId).loadObj(this.pedestalObjectId);

	var pedestal = new Global2014Pedestal(object, this.pedestalMaxHitPoints, race);

	this.objectIdToPedestalMap[object.id] = pedestal;
	mudLog(constants.BRF, 100, "Pedestal loaded in room #" + roomId + " for race " + raceNumToText(race) + ".");
};

global.global2014Util = new Global2014Util();
