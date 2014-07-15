var script1252 = function(self, actor, here, args, extra) {
	if( getSval(self, 1252, 'WellStarted') != 1 ) {
	    for(var i = 1;i <= 9;++i)
	    {
	        if( !getSval(self, 1250, 'brick' + i) )
		        return;
	    }
	    setSval(self, 1252, 'WellStarted', 1);
	}
	var stage = getSval(self, 1252, 'WellStage');
	if( stage == 0 ) {
	    //All marbles are in place. Load boss.
	    self.inRoom.echo("The well begins to glow brightly...");
	    wait 2;
	    self.inRoom.echo("The ground begins to vibrate as the well hums.");
		wait 2;
		self.inRoom.echo("Waves of spiders begin crawling out of the well. You must defeat them before heading back!");
		wait 3;
	}
	var mobsLoaded = getSval(self, 1252, 'WellMobs');
	if( !mobsLoaded )
	    mobsLoaded = [];
	var found = false;
	for(var i = 0;i < mobsLoaded.length;++i)
	{
	    var mob = mobsLoaded[i];
	    if( mob.room ) {
		    found = true;
			break;
		}
	}
	if( found )
	    return;
	//All mobs are dead. Load next phase.
	++stage;
	mobsLoaded = [];//Clear the array.
	if( stage == 1 ) {
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
	    here.echo("A bunch of spiders begin crawling out of the well!");
	}
	else if( stage == 2 )
	{
		mobsLoaded.push( self.inRoom.loadMob(1207) );//Index 0 is the leader.
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		for(var i = 1;i < mobsLoaded.length;++i) {
		    mobsLoaded[i].comm("follow huge");
		}
		mobsLoaded[0].comm("group all");
	    here.echo("A bunch of spiders begin crawling out of the well!");
	}
	else if( stage == 3 )
	{
		mobsLoaded.push( self.inRoom.loadMob(1207) );//Index 0 is the leader.
		mobsLoaded.push( self.inRoom.loadMob(1207) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		mobsLoaded.push( self.inRoom.loadMob(1208) );
		for(var i = 1;i < mobsLoaded.length;++i) {
		    mobsLoaded[i].comm("follow huge");
		}
		mobsLoaded[0].comm("group all");
	    here.echo("A bunch of spiders begin crawling out of the well!");
	}
	else {
	    //By now, the player has won.
		setSval(self, 1252, 'WellStarted', 0);
		setSval(self, 1252, 'WellStage', 0);
		setSval(self, 1252, 'WellMobs', 0);
		for(var i = 1;i <= 9;++i) {
		    setSval(self, 1250, 'brick' + i, 0);
		}
	    here.echo("The ground stops vibrating, and the well's illumination dissipates.");
		wait 3;
		here.echo("You notice a black marble that has fallen out of one of the well's slabs.");
		here.loadObj(3173);
		wait 3;
		here.echo("Be sure to retrieve it and bring it back to the grizzled man, to show him what you did here.");
		return;
	}
	setSval(self, 1252, 'WellStage', stage);
	setSval(self, 1252, 'WellMobs', mobsLoaded);
}