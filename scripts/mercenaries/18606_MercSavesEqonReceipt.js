var script18606 = function(self, actor, here, args, extra) {

	waitpulse 1;

	//TODO: We should not do this for every single item received. Probably better to do the storage periodically.

	MercUtil.saveMercenaryEquipment(self);
};
