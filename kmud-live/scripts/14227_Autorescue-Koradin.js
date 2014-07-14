var script14227 = function(self, actor, here, args, extra)
{
	function autoRescue() {
	if (self.leader) {
		for each (var player in self.room) {
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

};

