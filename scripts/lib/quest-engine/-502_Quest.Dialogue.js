var Quest = (function (Quest) {
	Quest.Dialogue = function(pulses, handle) {
		this.pulses = pulses;
		this.handle = handle;
	};

	Quest.Dialogue.prototype = {
		get isBeginDialogue() {
			return this.pulses == -1;
		},

		get isCompleteDialogue() {
			return this.pulses == -2;
		}
	};

	return Quest;
})(typeof Quest == "undefined" ? {} : Quest);

// Note: loadAllQuests() is called by bootProcs() after all scripts are loaded