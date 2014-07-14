var script23515 = function(self, actor, here, args, extra)
{
	if( !actor.inClan(15) )
    return;
if( extra.obj.vnum == 3189 && actor.quest('2009-BlackTower1') >= 100 )
{//Shovel Quest
    wait 1;
	self.say("Excellent, " + actor.name + "! Hopefully the rumors are true.");
	wait 3;
	self.comm("award " + actor.name + "15 3 Black Tower Quest");
	extra.obj.extract();
	actor.qval('2009-BlackTower1', -1);
}
if( extra.obj.vnum == 3173 && actor.quest('2009-BlackTower2') >= 100 )
{//Haunted Well Quest
    extra.obj.extract();
	
	actor.qval('2009-BlackTower2', -1);
}
else if( true )
{//...
}


};

