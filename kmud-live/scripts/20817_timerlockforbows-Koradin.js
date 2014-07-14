var script20817 = function(self, actor, here, args, extra)
{
	var charArgs = args
_block
if (actor.getPval("timerLock"))
{
actor.send("Try one thing at a time!")

return
}
else
{
actor.setPval("timerLock", args, true)

return
}

};

