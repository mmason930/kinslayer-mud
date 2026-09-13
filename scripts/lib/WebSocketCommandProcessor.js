function WebSocketCommandProcessor() {
	this.commandProcessors = {};

	setTimeout(3, function() { global.webSocketCommandProcessor.loadCommandProcessors(); });
}

function getMapInfo(userId) {
	var player = getPlayerById(userId);
	var room = player.room;
	var jsrooms = MapUtil.getRoomsInZone(room.zoneVnum);
	var response = {
		method: "Load Help File",
		subroutine: "Get Map",
		zone: { vnum: room.zoneVnum, name: room.zoneName },
		// Only get rooms that have exits (otherwise we really can't map it)
		rooms: jsrooms.filter(function(r) {
			return !r.neighbors.every(function(n) {
				return !n;
			});
		}).map(MapUtil.packageRoom),
		exits: MapUtil.getExitsInZone(room.zoneVnum)
	};

	return JSON.stringify(response);
}

WebSocketCommandProcessor.prototype.loadCommandProcessors = function() {
	this.commandProcessors["Load Help File"] = function(json, command, userId) {
		command = JSON.parse(json);

		if (command.subroutine === "Get Map")
			return getMapInfo(userId);

		var helpFile = global.helpManager.getHelpFileById(command.helpFileId);

		if (!helpFile)
			helpFile = global.helpManager.getHelpFileByName(command.helpFileId);

		var parentHelpFile = null;

		var response = {
			method: "Load Help File"
		};

		if (helpFile == null)
			response.error = "Could not find help file.";
		else {
			global.helpManager.createWebSocketMessageForHelpFile(helpFile, response);
		}

		return JSON.stringify(response);
	};

	this.commandProcessors["Save Help File"] = function(json, command, userId) {
		if (command.helpFileId == null)
			var helpFile = global.helpManager.createHelpFile(command.parentId);
		else
			var helpFile = global.helpManager.getHelpFileById(command.helpFileId);

		if (helpFile == null) {
			return JSON.stringify({method: command.method, errors: ["Could not find help file."]});
		}

		// Older editor tabs may omit this field; preserve an existing restriction.
		var minimum = command.minimumLevel === undefined ? (helpFile.minimumLevel || 0) : command.minimumLevel;
		if(!global.helpManager.isValidMinimumLevel(minimum))
			return JSON.stringify({method: command.method, errors: ["Minimum level must be a whole number from 0 to 105."]});
		helpFile.minimumLevel = minimum;
		helpFile.name = command.name;
		helpFile.syntax = command.syntax;
		helpFile.description = command.description;
		helpFile.keywords = command.keywords;
		helpFile.parentId = command.parentId;
		var isNew = helpFile.isNew();

		if (isNew) {
			helpFile.createdByUserId = userId;
			helpFile.createdDatetime = new Date();
		}

		helpFile.lastModifiedByUserId = userId;
		helpFile.lastModifiedDatetime = new Date();

		global.helpManager.saveHelpFileToDatabase(helpFile);

		if (isNew)
			global.helpManager.addHelpFileInternally(helpFile);


		mudLog(constants.BRF, 100, getUserNameByUserId(userId) + " has saved help file #" + helpFile.id + ": " + helpFile.name);

		var response = { method: command.method };
		global.helpManager.createWebSocketMessageForHelpFile(helpFile, response);
		return JSON.stringify(response);
	}
};

WebSocketCommandProcessor.prototype.generateErrorResponse = function(method, errors) {
	return {
		method: method,
		errors: errors
	};
};

WebSocketCommandProcessor.prototype.processCommand = function(json, userId) {
	var command = JSON.parse(json);

	if (command.method == null) {
		mudLog(constants.BRF, 100, "Could not process JavaScript web socket command. No method set. User ID: " + userId);
		return null;
	}

	// The editor page checks this too, but direct socket commands must be authorized.
	if(command.method === "Load Help File" || command.method === "Save Help File") {
		var level = -1;
		if(typeof userId === "number" && isFinite(userId) && userId > 0 && userId % 1 === 0) {
			var player = getPlayerById(userId);
			if(player && player.isValid) level = player.level;
			else {
				var result = sqlQuery("SELECT level FROM users WHERE user_id = " + userId);
				if(result && result.hasNextRow) level = result.getRow.getInteger("level");
			}
		}
		if(!(level >= 100))
			return JSON.stringify({method: command.method, error: "Help editing requires level 100 or higher.",
				errors: ["Help editing requires level 100 or higher."]});
	}

	var commandProcessor = this.commandProcessors[command.method];

	if (commandProcessor == null) {
		mudLog(constants.BRF, 100, "Could not proecss JavaScript web socket command. No processor found for method `" + command.method + "`. User ID: " + userId);
		return null;
	}

	return commandProcessor(json, command, userId);
}

WebSocketCommandProcessor.prototype.reload = function() {
	delete global.webSocketCommandProcessor;
	global.webSocketCommandProcessor = new WebSocketCommandProcessor();
}


