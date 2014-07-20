var Quest = (function (Quest) {
	Quest.ItemReward = function(vnum, count, loadPercent) {
		this.vnum = vnum;
		this.count = count;
		this.loadPercent = loadPercent;
	};

	return Quest;
})(typeof Quest == "undefined" ? {} : Quest);