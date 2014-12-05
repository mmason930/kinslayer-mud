Quest = (function(Q) {
	var existingQuests = Q.allQuests || [];
	var QVAL_PREFIX = "QUEST_";
	var COMPLETED_STR = "_NR-COMPLETED";
	var GIVER_STR = "_GIVER";

	var _questsByName = [];
	var _questsById = [];
	var _playerQuests = {};

	function cacheQuest(actor, quest) {
		if (!_playerQuests[actor.id]) {
			_playerQuests[actor.id] = {};
		}

		_playerQuests[actor.id][quest.databaseID] = quest;
	}

  /**
   * @constructs Quest
   */
	function _Quest() {
		this.name = "";
		this.summary = "";
		this.databaseID = 0;
		this.qp = 0;
		this.fp = 0;
		this.exp = 0;
		this.gold = 0;
		this.maxCompletions = 0;
		this.unlockRank = 0;
		this.showGivenBy = 0;
		this.skills = [];
		this.editors = [];
		this.allowedRaces = [];

		// Below properties must be loaded from external tables
		this.dialogue = [];
		this.tasks = [];
		this.items = [];
		this.itemRewards = {};
		this.priorQuests = [];
		this.tags = [];
		this.ownerVnums = [];
		this.extraChecks = [];
	}

	_Quest.prototype = /** @lends Quest.prototype */{
		/**********************
		 * METHODS
		 **********************/

		/**
		 * Effectively places this quest in the actor's journal by setting the quest state to begun
		 * and recording which mob issued the quest.
		 */
		begin: function(actor, giverVnum) {
			actor.qval(this.qvalStr, 1);
			if (isNumber(giverVnum)) {
				actor.qval(this.qvalStr + GIVER_STR, giverVnum);
			}

			// Make sure this quest is cached in actor's "own quests"
			cacheQuest(actor, this);
		},

		abandon: function(actor) {
			actor.qval(this.qvalStr, 0);
			this.clearState(actor);
			_playerQuests[actor.id] && delete _playerQuests[actor.id][this.databaseID];
		},

		/**
		 * Marks this quest as complete. If the quest can be repeated, the state of the quest
		 * and all tasks will be reset.
		 * @returns Number of times actor may repeat the quest.
		 */
		complete: function(actor) {
			actor.qval(this.qvalStr, -1);

			var completedCount = this.timesCompleted(actor) + 1;
			actor.qval(this.qvalStr + COMPLETED_STR, completedCount);
			if (this.canRepeat(actor)) {
				this.resetTasks(actor);
			}
      else {
        // Precautionary check to prevent any weird issues where a quest might be completed
        // more than the max times, allowing for infinite repeat
        completedCount = this.maxCompletions;
      }

			// Make sure this quest is cached in actor's "own quests"
			cacheQuest(actor, this);
      
      mudLog(constants.BRF, 100, actor.name + " completed quest: " + this.name);
      
			return this.maxCompletions - completedCount;
		},

		/**
		 * @param {JSCharacter} [self] The quest master mob the actor is interacting with. Possibly referred to in
		 * extra check code.
		 * @returns {bool} True if actor meets all requirements for viewing/accessing this quest.
		 */
		canAccess: function(actor) {
			// Try to discern who 'self' (quest master) might be
			var people = actor.room.people;
			var vnums = this.ownerVnums;
			var self = _.find(people, function(p) { return _.contains(vnums, p.vnum); });
			var quest = this;
			return (
				this.allowedRaces.indexOf(actor.race) != -1 &&
				this.extraChecks.every(function(code) {
					return eval(code);
				}) &&
				this.priorQuests.every(function(questId) {
					return Quest.getById(questId).hasCompleted(actor);
				})
			);
		},

		/**
		 * @returns {bool} True if actor can repeat this quest.
		 */
		canRepeat: function(actor) {
			return this.maxCompletions < 0 || (this.maxCompletions - this.timesCompleted(actor)) > 0;
		},

		/**
		 * @returns {bool} True if every task has been completed.
		 */
		readyToComplete: function(actor) {
			return this.tasks.every(function(task) {
				return task.getStatus(actor).completed;
			});
		},

		/**
		 * @returns {bool} True if any task has a non-zero progress value.
		 **/
		anyTaskStarted: function(actor) {
			for (var i = 0, task; task = this.tasks[i++];) {
				if (task.getStatus(actor).started)
					return true;
			}
			return false;
		},

		/**
		 * @returns {bool} True if actor has begun quest and has not yet completed it. Can also think of
		 * this as indicating the quest is in the actor's journal, but not in the 'Completed' chapter.
		 */
		hasBegun: function(actor) {
			return actor.quest(this.qvalStr) > 0;
		},

		/**
		 * @returns {bool} True if quest is marked as completed in actor's journal.
		 */
		hasCompleted: function(actor) {
			return actor.quest(this.qvalStr) == -1;
		},

		/**
		 * @returns {number} Number of times actor has completed this quest.
		 */
		timesCompleted: function(actor) {
			return actor.quest(this.qvalStr + COMPLETED_STR) || 0;
		},

		/**
		 * Rolls back any progress actor has made on this quest. This includes resetting
		 * tasks and removing all items connected with this quest from the actor's quest inventory.
		 */
		clearState: function(actor) {
			this.resetTasks(actor);
			actor.clearQuestItems(this);
		},

		/**
		 * Resets all tasks to the state they were in when the quest began.
		 */
		resetTasks: function(actor) {
			for (var i = 0, task; task = this.tasks[i++];) {
				task.resetProgress(actor);
			}
		},

		updateTask: function(actor, taskIndex) {
			this.tasks[taskIndex].update(actor);
		},

		/**
		 * @returns {string} A string representing the overall status of the actor's progress on this quest.
		 */
		statusString: function(actor, colors) {
			colors && getCharCols(actor);
			if (this.hasCompleted(actor)) {
				if (this.canRepeat(actor)) {
					return (colors ? red + bld : "") + "Completed, repeatable" + nrm;
				}
				return (colors ? red : "") + "Completed" + nrm;
			}
			if (!this.hasBegun(actor))
				return (colors ? nrm : "") + "Not Started" + nrm;
			if (this.readyToComplete(actor))
				return (colors ? mag + bld : "") + "Ready to Turn In" + nrm;
			if (this.anyTaskStarted(actor))
				return (colors ? grn + bld : "") + "In Progress" + nrm;
			return (colors ? bld + cyn : "") + "New Quest" + nrm;
		},

		/**
		 * @returns number The vnum of the mob that issued this quest to actor.
		 */
		giverVnum: function(actor) {
			return actor.quest(this.qvalStr + GIVER_STR);
		},

		/**
		 * This could be defined with a property getter, but using a method makes it more obvious that this is
		 * a potentially "expensive" operation, depending on how many quests are in the game.
		 * @returns {Quest[]} An array of all quests which list this quest in their [array of priorQuests]{@link Quest.priorQuests}.
		 */
		getDependentQuests: function() {
			var thisId = this.databaseID;
			return Quest.allQuests.filter(function(quest) {
				return _.contains(quest.priorQuests, thisId);
			})
		},

		/**********************
		 * PROPERTIES
		 **********************/
		get qvalStr() {
			return QVAL_PREFIX + this.databaseID;
		},

		/**
		 * @returns {bool} True if there is not quest master for this quest.
		 */
		get isMasterless() {
			return _.isEmpty(this.ownerVnums);
		},

		/**
		 * Alias for databaseID.
		 */
		get id() {
			return this.databaseID;
		},
    
    /**
		 * Alias for name.
		 */
		get questName() {
			return this.name;
		}
	};

	/**********************
	 * Static methods
	 **********************/
	var _getName = function(quest) {
		return quest.name.toLowerCase();
	};

	var _getId = function(quest) {
		return (quest ? quest.databaseID : -1);
	};

	/**
	 * Makes quest available at the global scope via the {@link Quest.getById} and {@link Quest.getByName} methods.
	 * @param {Quest} quest The quest to add.
	 * @param {bool} commit If true, the quest's data will also be committed to the database.
	 */
	_Quest.addQuest = function(quest, commit) {
		// Do the save first, so we ensure the databaseID is set
		if (commit)
			saveQuest(quest);

		// Protect against duplicates in our arrays
		if (!Quest.getById(quest.databaseID)) {
			sortedInsert(_questsById, quest, _getId);
			sortedInsert(_questsByName, quest, _getName);
		}

		Quest.getById.cache[quest.databaseID] = quest;
	};

	/**
	 * Makes quest unavailable at the global scope via the Quest.getBy* methods.
	 * @param {Quest} quest The quest to remove.
	 * @param {bool} commit If true, the quest will be deleted from teh database.
	 */
	_Quest.deleteQuest = function(quest, commit) {
		_.pull(_questsById, quest);
		_.pull(_questsByName, quest);
		if (commit)
			deleteQuestFromDatabase(quest);
	};

	_Quest.clearQuests = function() {
		_questsById = [];
		_questsByName = [];

		Quest.getById.cache = {};
	};

	_Quest.getById = _.memoize(function(id) {
		// Memoize this function to improve performance: id->quest relationship should never change
		var keyWrap = { databaseID: id };
		var arr = _questsById;
		var getKey = _getId;

		var candidate = arr[_.sortedIndex(arr, keyWrap, getKey)];
		return (getKey(candidate) == getKey(keyWrap) ? candidate : null);
	}, _.identity);

	_Quest.getByName = function(name) {
		if (!name)
			return null;

		var keyWrap = { name: name.toString().toLowerCase() };
		var arr = _questsByName;
		var getKey = _getName;

		var candidate = arr[_.sortedIndex(arr, keyWrap, getKey)];
		return (getKey(candidate) == getKey(keyWrap) ? candidate : null);
	};

	_Quest.getByMaster = function(vnum) {
		return Quest.allQuests.filter(function(quest) {
			return _.contains(quest.ownerVnums, vnum);
		});
	},

	/**
	 * @returns {Quest[]} An array of quests that actor has either completed or at least begun.
	 */
	_Quest.getQuestsFor = function(actor) {
		var ownQuests = _playerQuests[actor.id];
		// We don't have a cached version of their quests, so build it
		if (!ownQuests) {
			_playerQuests[actor.id] = ownQuests = {};
			for (var i = 0, quest; quest = _questsById[i++];) {
				if (quest.hasBegun(actor) || quest.hasCompleted(actor)) {
					ownQuests[quest.databaseID] = quest;
					//mudLog(constants.BRF, 104, "Caching own quest " + quest.databaseID + " for " + actor.name);
				}
			}
		}
		return _.values(ownQuests);
	};

	_Quest.__defineGetter__("allQuests", function() {
		return _.clone(_questsByName);
	});

	for (var i = 0, q; q = existingQuests[i++];)
		_Quest.addQuest(q);

	// Mixin existing properties
	for (var p in Q) {
		if (Q.hasOwnProperty(p) && !_Quest.hasOwnProperty(p))
			_Quest[p] = Q[p];
	}

	return _Quest;
})(typeof Quest == "undefined" ? {} : Quest);

// Reload all Quests to ensure prototypes aren't stale
loadAllQuests();