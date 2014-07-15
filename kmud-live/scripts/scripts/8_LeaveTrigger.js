var script8 = function(self, actor, here, args, extra) {
	_block;
	actor.startTimer(2);
	var success = runTimer(actor);
	if( !success )
	{
		actor.comm("look");
	}
		
}