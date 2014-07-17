var script1019 = function(self, actor, here, args, extra) {
	var slum = self.leader;
	if (!slum)
	return;
	if (slum.eq(constants.WEAR_EAR_1)) {
	slum.eq(constants.WEAR_EAR_1).extract();
	return
	}
	else if (slum.eq(constants.WEAR_NECK_1)) {
	slum.eq(constants.WEAR_NECK_1).extract();
	return
	}
	else if (slum.eq(constants.WEAR_BACK)) {
	slum.eq(constants.WEAR_BACK).extract();
	return;
	}
}