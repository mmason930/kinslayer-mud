var script4585 = function(self, actor, here, args, extra) {
	var ch = self.wornBy;
	if( !ch )
		return;
		
	if ( ch.fighting )
	{	
		var vict = ch.fighting;
		act( "A bolt of lightning arcs from $p to $N, stunning them!", false, ch, self, vict, constants.TO_CHAR );
		act( "A bolt of lightning arcs from $p to $N, stunning them!", false, ch, self, vict, constants.TO_NOTVICT );
		act( "A bolt of lightning arcs from $p, jolting and stunning you!", false, ch, self, vict, constants.TO_VICT );
		vict.comm("grunt");
		vict.position = constants.POS_SITTING;
		vict.isBashed = true;
		vict.lag(28);
		waitpulse 28;
		vict.isBashed = false;
		if( !vict.fighting )
			vict.position = constants.POS_STANDING;
		else
			vict.position = constants.POS_FIGHTING;
	}
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
}