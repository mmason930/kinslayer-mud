var Quest = (function (Q) {
	var BEGUN_STR = "-HAS-BEGUN";
	var COMPLETED_STR = "-NR-COMPLETED";

	function Quest() {
		this.questName = "";
		this.strSummary = "";
		this.databaseID = 0;
		this.qp = 0;
		this.fp = 0;
		this.exp = 0;
		this.gold = 0;
		this.num = 0;
		this.unlockRank = 0;
		this.showGivenBy = 0;
		this.skillArray = [];
		this.editors = [];

		// Below properties must be loaded from external tables
		this.dialogue = [];
		this.taskArray = [];
		this.items = [];
		this.itemReward = [];
		this.priorQuests = [];
		this.tags = [];
		this.ownerVnum = [];
		this.extras = [];
	}

	Quest.prototype = {
		begin: function (actor) {
			actor.qval(this.questName + BEGUN_STR, 1);
		},

		abandon: function (actor) {
		},

		complete: function (actor) {
		},

		readyToComplete: function (actor) {
			// Check to make sure all necessary requirements are met for turning quest in
		},

		hasBegun: function (actor) {
			return actor.quest(this.questName + BEGUN_STR) == 1;
		},

		hasCompleted: function(actor) {
			return actor.quest(this.questName) == -1;
		},

		timesCompleted: function (actor) {
			return actor.quest(this.questName + COMPLETED_STR) || 0;
		}
	};

	// Mixin existing properties
	for (var p in Q) {
		if (Q.hasOwnProperty(p))
			Quest[p] = Q[p];
	}

	return Quest;
})(typeof Quest == "undefined" ? {} : Quest);