var script13256 = function(self, actor, here, args, extra)
{
	if( actor.questInProgress("A Hindering Blockade") ) {
	waitpulse 4;
	if( actor.hasItem(23261,true,true,true) == null ) {
		self.say("Yes. I have exactly what you need, right here.");
		wait 4;
		self.loadObj(23261);
		self.comm("give sledge " + actor.name);
	}
	else {
		self.say("It appears you already have a sledgehammer.");
		return;
	}
}
	

};

