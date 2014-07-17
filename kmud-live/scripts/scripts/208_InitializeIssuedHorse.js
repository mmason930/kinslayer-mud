var script208 = function(self, actor, here, args, extra) {
	//	June 2010 - Fogel
	//	Initializes svals for a freshly issued, bought or spawned horse
	waitpulse 6;
	if( self.leader )
	{
		setSval( self.leader, 207, "horse", self );
		setSval( self, 207, "owner", self.leader );
	}
	else if( self.riddenBy )
	{
		setSval( self.riddenBy, 207, "horse", self );
		setSval( self, 207, "owner", self.riddenBy );
	}
}