var script23231 = function(self, actor, here, args, extra) {
	//Alder December 2009
	/**
	Used in: QUEST - 'Finding the Blade'
	The anvil should only ever hold one dagger blade, so this script
	loads a new dagger blade to anvil ONLY IF IT DOES NOT CONTAIN ONE ALREADY.
	ALSO - SEE KJS -35
	**/
	self.limitObjLoadToContainer( 8101, 13203, 1 );
}