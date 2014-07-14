var script4583 = function(self, actor, here, args, extra)
{
	var ch = self.wornBy;
if( !ch )
	return;
	
act( capFirstLetter(self.name) + " glows a soothing blue color, surrounding $n!", false, ch, self, ch, constants.TO_ROOM );
act( ch.cyan(constants.CL_COMPLETE) + capFirstLetter(self.name) + " envelopes you in a soothing blue aura, and you feel a bit better!" + ch.normal(constants.CL_COMPLETE), false, ch, self, ch, constants.TO_CHAR );
ch.hps += 20 + random(0, 5);
waitpulse 1;
if( random(0, 100) < 10 )
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

