var script10256 = function(self, actor, here, args, extra) {
	var moratTorm = 1121;
	var morat = getMobAtRoom( here.vnum, moratTorm );
	var vict = self.fighting;
	if( morat && morat.fighting )
		self.comm("rescue morat");
	if( vict && random(0, 2) )
	{
		act("$n slashes at your body with $s sharp claws!", true, self, null, vict, constants.TO_VICT);
		act("$n slashes at $N's body with $s sharp claws!", true, self, null, vict, constants.TO_NOTVICT);
		vict.damage(random(5, 15), self);
		return;
	}
}