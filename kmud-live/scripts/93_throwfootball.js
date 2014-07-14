var script93 = function(self, actor, here, args, extra)
{
	_block;
var vArgs = getArgList(args);
var vict = getCharInListVis(actor, vArgs[1], here.people)
if( vict )
{
	var success = random(1, 10);
	if( success > 2 )
	{
		act("You pass $p to $N, who catches it skillfully.", true, actor, self, vict, constants.TO_CHAR);
		act("$n passes $p to $N, who catches it skillfully.", true, actor, self, vict, constants.TO_NOTVICT);
		act("You skillfully catch $p passed to you by $n", true, actor, self, vict, constants.TO_VICT);
		self.moveToChar(vict);
	}
	else
	{
		act("You pass $p to $N, but $E clumsily drops it!", true, actor, self, vict, constants.TO_CHAR);
		act("$n passes $p to $N, but $E clumsily drops it!", true, actor, self, vict, constants.TO_NOTVICT);
		act("You attempt to catch $p passed to you by $n, but you clumsily drop it!", true, actor, self, vict, constants.TO_VICT);
		self.moveToRoom(here);
	}
}
else
{
	actor.send("Pass to who?\r\n");
}

};

