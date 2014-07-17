var script2504 = function(self, actor, here, args, extra) {
	//talisman WEAR
	//Koradin - April 2010
	_noblock;
	if (actor.affectedBy(constants.AFF_SANCTUARY))
		return;
	actor.affect(constants.AFF_SANCTUARY,1000,0);
	act("As you hold the talisman, you feel the sanctuary of the hero flow through you!",true,actor,null,null,constants.TO_CHAR);
	act("As $n holds the talisman, $e begins to glow with a bright light!",true,actor,null,null,constants.TO_ROOM);
}