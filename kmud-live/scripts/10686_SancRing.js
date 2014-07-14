var script10686 = function(self, actor, here, args, extra)
{
	if (!actor.affectedBy(constants.AFF_SANCTUARY) && !actor.affectedBy(constants.AFF_NOQUIT) ) {
actor.send("You are encased in a protective shield of darkness.");
actor.affect(constants.AFF_SANCTUARY, 9,0,0);
}

};

