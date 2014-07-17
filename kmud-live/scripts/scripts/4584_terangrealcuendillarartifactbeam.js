var script4584 = function(self, actor, here, args, extra) {
	var ch = self.wornBy;
	if( !ch )
		return;
		
	if ( ch.fighting )
	{	
		var vict = ch.fighting;
		act( "A beam of white light blasts forth from $p, hitting $N!", false, ch, self, vict, constants.TO_CHAR );
		act( "A beam of white light blasts forth from $p, hitting $N!", false, ch, self, vict, constants.TO_NOTVICT );
		act( "A beam of white light blasts forth from $p, hitting you in the chest!", false, ch, self, vict, constants.TO_VICT );
		var dmg = (20 + random(0, 10));
		vict.damage(dmg);
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