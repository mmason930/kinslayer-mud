var script9905 = function(self, actor, here, args, extra)
{
	var vict = self.fighting;
var spell = random(1, 1);
if( !vict )
	return;
else if( spell == 1 )
{	
	act( "$n shoots a powerful blast of flames at you, causing you to scream in agony!", false, self, null, vict, constants.TO_VICT );
	act( "$n shoots a powerful blast of flames at $N, who lets out an agonizing scream!", false, self, null, vict, constants.TO_NOTVICT );
	vict.damage( random(30, 50) );
	if( random(1, 3) == 1 && ! vict.affectedBy(constants.AFF_BURN) )
	{
		vict.send( "Arrrgggh!!! Your body is on FIRE! That really does HURT!" );
		vict.affect( constants.AFF_BURN, 1, 0, 0 );
	}
	return;
}
// other spells can go here

};

