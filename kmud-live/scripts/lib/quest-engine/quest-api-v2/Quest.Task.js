﻿var Quest = (function(Quest) {
	var TASK_PREFIX = "_TASK_";
	/**
	 * Creates a new Quest task.
	 *
	 * @param {number|Quest.Task} idOrSource Either the id of the task to create, or an existing task object to copy values from.
	 * @param {Quest} quest The Quest with which this task is associated.
	 * @param {string} name The text/description of the task. E.g. fetch some bread.
	 * @param {string} _dynamicProgress Pass null if task uses a qval to track progress. A string containing JS code that, when evaluated, returns a function which takes 'actor' as an argument, and returns a number indicating the actor's current progress for this task. See itemCount function.
	 * @param {any} completedValue The progress value indicating the task is completed. The passed in type can be anything, as long as eval(completedValue) is a number.
	 * @param {number} unlockerTaskIndex The index of the task which must be completed before this one becomes available. -1 means this task is never locked.
	 */
	Quest.Task = function(idOrSource, quest, name, _dynamicProgress, completedValue, unlockerTaskIndex) {
		// Allow instantiating task from another task object
		if (arguments.length == 1) {
			var source = idOrSource;
			this.id = source.id;
			this.quest = source.quest;
			this.name = source.name;
			this._dynamicProgress = source._dynamicProgress;
			this._completedExpr = source.completedValue;
			this.unlockerTaskIndex = source.unlockerTaskIndex;
			return;
		}
		this.id = idOrSource;
		this.quest = quest;
		this.name = name;
		this._dynamicProgress = _dynamicProgress;
		this._completedExpr = completedValue;
		this.unlockerTaskIndex = parseInt(unlockerTaskIndex);
	};

	Quest.Task.prototype = {
		/**
		 * METHODS
		 */

		/**
		 * Call this method whenever the actor makes progress on this task.
		 * Ensures the following:
		 *	 - for qval tasks, qval is incremented
		 *  - if task reaches completion: dependent tasks are unlocked, and quest is completed if no tasks remain and this quest has no quest master
		 *  - any pertinent messages are sent to actor
		 */
		update: function(actor) {
			var block = getSval(actor, 100, this.qvalStr);
			if (block)
				return;

			// Block simultaneous calls to update
			setSval(actor, 100, this.qvalStr, 1);

			var _this = this;
			setTimeout(1, function() {
				// Unblock
				setSval(actor, 100, _this.qvalStr, 0);

				// Update qval if necessary
				if (_this.isQvalTask) {
					actor.qval(_this.qvalStr, _this.getProgress(actor) + 1);
				}

				getCharCols(actor, constants.CL_OFF);

				if (_this.getProgress(actor).completed) {
					// If the quest is ready to complete, and has no master, try to complete
					if (_this.quest.isMasterless && _this.quest.readyToComplete(actor)) {
						Quest.Util.tryCompleteQuest(actor, _this.quest);
						return;
					}
				}

				Quest.Util.journalUpdate(actor, _this.quest);
			});
		},

		/**
		 *  Returns true if actor has not completed this task and it is listed in their journal.
		 */
		isInProgress: function(actor) {
			return this.quest.hasBegun(actor) && this.hasUnlocked(actor) && !this.getStatus(actor).completed;
		},

		hasCompleted: function(actor) {
			return this.getProgress(actor) >= this.completedValue;
		},

		/**
		 * @returns {bool} Whether or not this task has been unlocked for actor.
		 */
		hasUnlocked: function(actor) {
			return this.unlockerTaskIndex < 0 || this.quest.tasks[this.unlockerTaskIndex].getStatus(actor).completed;
		},

		getProgress: function(actor) {
			if (this.isQvalTask) {
				return actor.quest(this.qvalStr);
			}
			return this.progressFunction(actor);
		},

		getStatus: function(actor) {
			var prog = this.getProgress(actor);
			return {
				started: prog > 0,										// Some progress has been made on this task
				progress: Math.min(prog, this.completedValue),	// The current progress made on this task
				completed: prog >= this.completedValue				// Value indicating whether the task is complete
			};
		},

		resetProgress: function(actor) {
			if (this.isQvalTask) {
				actor.qval(this.qvalStr, 0);
			}
		},

		/**
		 * PROPERTIES
		 */
		get dependentTasks() {
			var _this = this;
			var index = this.quest.tasks.indexOf(this);
			return this.quest.tasks.filter(function(t) {
				t.unlockerTaskIndex == index;
			});
		},

		get qvalStr() {
			return this.quest.qvalStr + TASK_PREFIX + this.id;
		},

		get questName() {
			return this.quest.name;
		},

		get completedValue() {
			return eval(this._completedExpr);
		},

		get isQvalTask() {
			return !(this.progressFunction instanceof Function);
		},

		get progressFunction() {
			return eval(this._dynamicProgress);
		}
	};

	return Quest;
})(typeof Quest == "undefined" ? {} : Quest);

// Reload all Quests to ensure prototypes aren't stale
loadAllQuests();