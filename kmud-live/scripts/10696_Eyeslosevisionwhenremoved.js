var script10696 = function(self, actor, here, args, extra)
{
	if ( actor.race == constants.RACE_HUMAN && actor.affectedBy(constants.AFF_NIGHT_VISION) ) {
     actor.unaffect(constants.AFF_NIGHT_VISION);
}

};

