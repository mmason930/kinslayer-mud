var script1207 = function(self, actor, here, args, extra) {
	_noblock;
	while( true )
	{
	    wait 2;
	    if( !self.fighting )
		    continue;
	    var prob = random(1,100);
		if( prob <= 10 && !self.isBashed) {
		    //Bite
			self.room.echoaround(self.fighting, capFirstLetter(self.name) + " bites " + self.fighting.name + "!\r\n");
			self.fighting.send(capFirstLetter(self.name) + " bites you!\r\n");
			self.fighting.damage(random(5,15));
			self.fighting.affect(constants.AFF_POISON,1,0,0);
		}
	}
}