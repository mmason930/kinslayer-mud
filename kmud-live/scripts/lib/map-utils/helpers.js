var MapUtil = (function() {
	return {
		packageRoom: function(jsroom) {
			return {
				vnum: jsroom.vnum,
				name: jsroom.name,
				sector: jsroom.sector
			};
		},

		getExitsInZone: _.memoize(function(zone) {
			var jsrooms = this.getRoomsInZone(zone);
			var exits = {};
			jsrooms.forEach(function(jsroom) {
				if (jsroom.neighbors) {
					jsroom.neighbors.forEach(function(r) {
						if (!r) return;
						var exit = {
							start: jsroom.vnum,
							dir: jsroom.firstStep(r),
							destination: r.vnum,
							destinationZone: r.zoneVnum
						};
						var key = [exit.start, exit.destination].sort().join("->");
						exits[key] = exit;
					});
				}
			});
			return exits;
		}, _.identity),

		getRoomsInZone: _.memoize(function(zone) {
			var zoneRooms = [];
			for (var rnum = 0, room; room = getRoomByRnum(rnum++) ;) {
				if (room.zoneVnum == zone)
					zoneRooms.push(room);
			}
			return zoneRooms;
		}, _.identity)
	};
})();