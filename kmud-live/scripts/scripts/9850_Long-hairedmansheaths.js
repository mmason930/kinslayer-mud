var script9850 = function(self, actor, here, args, extra) {
	if ( self.vnum == 9850 ) {
		self.comm("sheath dagger");
		setSval(self,9850,"POINT_A",self.room.vnum);	// Baerlon Inn Room
		setSval(self,9850,"POINT_B",1732);				// SE Corner of Fal Dara
		setSval(self,9850,"POINT_C",20106);				// SW Corner of Lugard
		setSval(self,9850,"POINT_D",21767);				// Aringill Inn
		setSval(self,9850,"DESTINATION",getSval(self,9850,"POINT_B"));
		// self.room.echo("POINT_A: "+getSval(self,9850,"POINT_A"));
		// self.room.echo("POINT_B: "+getSval(self,9850,"POINT_B"));
	}
}