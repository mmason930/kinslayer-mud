(function() {
	var WHERE = " where quest_id = {0} and user_id = {1} and itemName = '{2}';";
	var SELECT_COUNT = "select itemAmount from questInventory" + WHERE;
	var LOAD = "select * from questInventory where user_id = {0};";
	var UPDATE_COUNT = "update questInventory set itemAmount = {3}" + WHERE;
	var INSERT_COUNT = "insert into questInventory (quest_id, user_id, itemName, itemAmount) values({0}, {1}, '{2}', {3});";
	var DELETE_ITEM = "delete from questInventory where quest_id = {0} and user_id = {1} and itemName = '{2}';";

	function key(actor, quest) {
		return ("" + actor.id + (quest ? quest.databaseID : null));
	}

	var inventory = {};
	function cacheCount(actor, questId, itemName, count) {
		var inv = inventory[actor.id];
		if (!inv)
			inv = inventory[actor.id] = {};

		var bucketName = questId;
		var bucket = inv[bucketName];
		if (!bucket)
			bucket = inv[bucketName] = {};

		bucket[itemName] = count;
		if (count <= 0)
			delete bucket[itemName];
	}
	function getCount(actor, questId, itemName) {
		var container = inventory[actor.id];
		container = !container || container[questId];
		container = !container || container[itemName];
		return _.isNumber(container) ? container : null;
	}

	/**
	 * Adds an item to the character's quest inventory.
	 * @param {string} itemName Name of the item to add.
	 * @param {number} [count] Number of items to add.
	 * @param {Quest} [quest] The quest to which the item is related.
	 */
	JSCharacter.prototype.addQuestItem = function(itemName, count, quest) {
		if (arguments.length == 2 && typeof count == "object")
			quest = count;

		count = (_.isNumber(count) ? count : 1);
		var current = this.countQuestItem(itemName, quest);
		var newCount = Math.max(0, current + count);
		// If the count goes to 0, delete the row from the db
		var query = (newCount > 0 ? UPDATE_COUNT : DELETE_ITEM);
		// If current is null, need to insert instead of updating
		if (current == null) {
			// No row exists for this item, but they're trying to remove the item => nop
			if (newCount <= 0)
				return;
			query = INSERT_COUNT;
		}

		var questId = (quest ? quest.databaseID : -1);
		var stmt = prepareStatement(query, questId, this.id, itemName, newCount);
		sqlQuery(stmt);
		cacheCount(this, questId, itemName, newCount);

		getCharCols(this);
		if (count > 0) {
			this.send(cyn + capFirstLetter(itemName) + nrm + bld + " has been placed in your quest inventory." + (count > 1 ? " (" + count + ")" : "") + nrm);
		}
		else if (count < 0) {
			var numRemoved = current - newCount;
			this.send(cyn + capFirstLetter(itemName) + nrm + bld + " has been removed from your quest inventory." + (numRemoved > 1 ? " (" + (numRemoved) + ")" : "") + nrm);
		}
	};

	/**
	 * Removes an item from character's quest inventory.
	 * @param {string} itemName Name of the item to add.
	 * @param {number} [count] Number of items to remove.
	 * @param {Quest} [quest] The quest to which the item is related.
	 */
	JSCharacter.prototype.remQuestItem = function(itemName, count, quest) {
		if (arguments.length == 2 && typeof count == "object")
			quest = count;

		count = (_.isNumber(count) ? count : 1);
		this.addQuestItem(itemName, -count, quest);
	};

	/**
	 * Removes all items related to quest from character's quest inventory.
	 * @param {Quest} quest Quest whose items will be removed.
	 */
	JSCharacter.prototype.clearQuestItems = function(quest) {
		//var stmt = prepareStatement(DELETE_ITEM, quest.databaseID, this.id);
		//sqlQuery(stmt);
		// Clear the cached item counts for this actor/quest
		var inv = inventory[this.id];
		for (var id in inv) {
			if (id == quest.databaseID) {
				var bucket = inv[id];
				for (var itemName in bucket) {
					this.remQuestItem(itemName, bucket[itemName], quest);
				}
			}
		}
	};

	/**
	 * @param {string} itemName Name of the item to add.
	 * @param {number} [count] Number of items to check for.
	 * @param {Quest} [quest] The quest to which the item is related.
	 * @returns {bool} True if actor has at least count number of itemName in his quest inventory.
	 */
	JSCharacter.prototype.checkQuestItem = function(itemName, count, quest) {
		if (arguments.length == 2 && typeof count == "object")
			quest = count;

		count = (_.isNumber(count) ? count : 1);
		return this.countQuestItem(itemName, quest) >= count;
	};

	/**
	 * @returns {number|null} The number of itemName in character's quest inventory, or null if no records exist
	 * for that item in the database for this character and quest.
	 */
	JSCharacter.prototype.countQuestItem = function(itemName, quest) {
		var questId = (quest ? quest.databaseID : -1);
		// Try to get the cached value before querying db
		var cached = getCount(this, questId, itemName);
		if (_.isNumber(cached))
			return cached;

		var stmt = prepareStatement(SELECT_COUNT, questId, this.id, itemName);
		var rs = sqlQuery(stmt);

		var count = rs.hasNextRow ? _.parseInt(rs.getRow.get("itemCount")) : null; 
		// Cache the count
		count && cacheCount(this, questId, itemName, count);
		return (count);
	};

	JSCharacter.prototype.getQuestInventory = function() {
		var items = [];
		var inv = inventory[this.id];
		if (!inv) {
			// DB load
			mudLog(constants.NRM, 103, "Initializing quest inventory from db: " + this.name);
			var stmt = prepareStatement(LOAD, this.id);
			var rs = sqlQuery(stmt);
			if (!rs.hasNextRow)
				inventory[this.id] = {};

			while (rs.hasNextRow) {
				var row = rs.getRow;
				cacheCount(this, row.get("quest_id"), row.get("itemName"), _.parseInt(row.get("itemAmount")));
			}

			inv = inventory[this.id];
		}
		for (var bName in inv) {
			var bucket = inv[bName];
			for (var itemName in bucket) {
				items.push({ name: itemName, count: bucket[itemName] });
			}
		}
		return items;
	};
})();