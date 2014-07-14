var script4582 = function(self, actor, here, args, extra)
{
	var ch = self.wornBy;
if( !ch )
	return;
	
act( "Blue energy flows from $p to $n, revitalizing $m!", false, ch, self, ch, constants.TO_ROOM );
act( ch.cyan(constants.CL_COMPLETE) + "Blue energy flows from $p to you, and you feel a bit recharged!" + ch.normal(constants.CL_COMPLETE), false, ch, self, ch, constants.TO_CHAR );
ch.mvs += 20 + random(0, 5);
waitpulse 1;
if( random(0, 100) == 7 )
{
	here.echo( capFirstLetter(self.name) + " glows white hot and starts to shake." );
	waitpulse 14;
	here.echo( capFirstLetter(self.name) + " makes a loud hissing sound as it incinerates itself!" );
	waitpulse 1;
	var gloves = ch.eq(constants.WEAR_HANDS);
	if( gloves )
	{
		act( capFirstLetter(gloves.name) + " has been destroyed.", false, ch, gloves, ch, constants.TO_CHAR );
		act( "Your hands have been badly burned!", false, ch, gloves, ch, constants.TO_CHAR );
		gloves.extract();
	}
	ch.damage( 60 + random(0, 15) );
	self.extract();	
}

};

