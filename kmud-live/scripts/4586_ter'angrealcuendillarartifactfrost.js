var script4586 = function(self, actor, here, args, extra)
{
	var ch = self.wornBy;
if( !ch )
	return;
	
if ( ch.fighting )
{	
	var vict = ch.fighting;
	act( "A wave of frost from $p covers $N!", false, ch, self, vict, constants.TO_CHAR );
	act( "A wave of frost from $p covers $N!", false, ch, self, vict, constants.TO_NOTVICT );
	act( "A wave of frost from $p covers you, slowing you down!", false, ch, self, vict, constants.TO_VICT );
	var mv_dmg = (10 + random(0, 15)) * ( vict.mvs / (vict.maxMove + 1) );
	vict.mvs -= mv_dmg;
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

};

