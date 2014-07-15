var script671 = function(self, actor, here, args, extra) {
	//Galnor
	//January 18, 2014
	/************************************************************
	 *                       [ Extract ]                        *
	 *                      Global Script                       *
	 * Purpose: perform actions when players, mobs, or objects  *
	 *          are extracted from the game.                    *
	 * Current events:                                          *
	 *  - Unhitch carts from horses                             *
	 *                                                          *
	 *  ......                                                  *
	 * Please append any new events to this list                *
	 *                                                          *
	 * NOTE: NEVER WAIT IN THIS SCRIPT                          *
	 * NOTE: Code in this should be as efficient as possible.   *
	 *       This will be called for every extraction.          *
	 ************************************************************/
	
	if(self.hitchedBy)
	{
		self.hitchedBy.hitchedTo = null;
		self.hitchedBy = null;
	}
	if(self.hitchedTo)
	{
		self.hitchedTo.hitchedBy = null;
		self.hitchedTo = null;
	}
}