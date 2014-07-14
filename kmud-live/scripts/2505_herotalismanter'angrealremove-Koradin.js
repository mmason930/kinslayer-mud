var script2505 = function(self, actor, here, args, extra)
{
	//hero talisman remove
//Koradin - April 2010
_noblock;
actor.unaffect(constants.AFF_SANCTUARY);
act("The added peace fades with the surrounding light as you remove the talisman.",true,actor,null,null,constants.TO_CHAR);
act("The light slowly fades from $n.",true,actor,null,null,constants.TO_ROOM);

};

