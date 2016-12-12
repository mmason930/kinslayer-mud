var MercUtil = (function(MercUtil) {

	MercUtil.performSave = function(actor, target, vArgs, here, myMercObj)
	{
		actor.send(grn + capFirstLetter(target.name) + " is saving..." + nrm);
		MercUtil.saveMercenaryEquipment(target);
		actor.send(grn + capFirstLetter(target.name) + " has finished saving." + nrm);
	};

	return MercUtil;
})(MercUtil || {});
