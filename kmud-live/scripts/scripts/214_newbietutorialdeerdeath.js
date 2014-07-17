var script214 = function(self, actor, here, args, extra) {
	function afterWait(args) {
	var actor = args[0];
	if (getSval(actor,501,"tutorial") == 14);
		actor.comm("imdonewiththedeer");
	}
	setTimeout(1,afterWait,[actor]);
}