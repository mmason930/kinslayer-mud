var script13514 = function(self, actor, here, args, extra) {
	var startRoom = actor.room;
	if( extra.direction == constants.DOWN )
	{
		_block;
		actor.send("You can't go down there without falling and hurting yourself!");
		return;
	}
	if( startRoom != self.direction( constants.NORTH ) )
	{
		_noblock;
		return;
	}
	waitpulse 14;
	if( actor.room == here )
	{
		here.echoaround(actor, actor.name + " begins slipping on the narrow ledge.");
		actor.send("You start to lose your footing and begin slipping...");
		waitpulse 12;
		here.echoaround(actor, actor.name + " tumbles backwards and falls down into a rocky hole!");
		actor.send("You slip and fall backwards into a pile of rocks!");
		waitpulse 12;
		actor.moveToRoom( here.direction( constants.DOWN ) );
		actor.comm("look");
		here.echoaround(actor, capFirstLetter(actor.name) + " grunts in pain as " + his_her(actor.sex) + " head hits a pile of rocks.");
		actor.send("OUF! You grunt in pain as your head hits a pile of dry rocks.");
		waitpulse 10;
		here.echoaround(actor, capFirstLetter(actor.name) + " rubs " + his_her(actor.sex) + " head.");
		actor.send("You rub your head.");
	}
}