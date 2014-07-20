var script14227 = function(self, actor, here, args, extra) {
	function autoRescue() {
		if (self.leader) {
			for (var _autoKey in self.room) {
				var player = self.room[_autoKey];
				if (player.fighting == self.leader) {
					self.comm("rescue "+self.leader.namelist.split(" ")[0]);
				}
			}
		}
	}
	autoRescue();
	wait 3;
	autoRescue();
	wait 3;
	autoRescue();
	wait 3;
	autoRescue();
}