var script10305 = function(self, actor, here, args, extra) {
	if( self.fighting && self.fighting.race == constants.RACE_TROLLOC && self.fighting.vnum == -1 ) {
		self.comm("yell Hurry! The defense of Fal Moran has been broken! The foul, disgusting beast " + self.fighting.name + " is attacking me at " + here.name + "! Argh!!");
	}
}