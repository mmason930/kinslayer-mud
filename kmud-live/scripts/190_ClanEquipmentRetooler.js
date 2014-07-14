var script190 = function(self, actor, here, args, extra)
{
	// Script for retooling objects to their clan versions
// written by Fogel
var obj = extra.obj;
var clan = self.findClans();
if(clan.length) {
	clan = clan[0];
}
if (clan == 16) { // mob is in White Tower, check actor's clan to match Ajah
	for (i=17;i<24;i++) {
		if (actor.inClan(i))
			clan = i;
	}
}
if( !actor.inClan( clan ) || actor.race != self.race )
{
	_block;
	waitpulse 3;
	self.say("We don't serve your kind here!" );
	return;
}
if( !hasClanEquivalent( clan, obj.vnum ) )
{
	_block;
	waitpulse 3;
	self.comm( "say We don't have an equivalent item for that." );
	return;
}
else
{
	_noblock;
	waitpulse 5;
	self.comm( "say Thanks, here's your uniform piece!" );
	obj.setRetoolName( getClanGearName( clan, obj.vnum ) ); // Retool namelist goes in here
	obj.setRetoolDesc( getClanGearDesc( clan, obj.vnum ) ); // Retool description goes in here
	obj.setRetoolSDesc( getClanGearSDesc( clan, obj.vnum ) ); //Retool short description goes in here
	waitpulse 5;
	self.comm( "give all " + actor.name );
}



};

