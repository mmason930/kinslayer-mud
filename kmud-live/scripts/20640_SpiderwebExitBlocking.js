var script20640 = function(self, actor, here, args, extra)
{
	if( (extra.direction == constants.EAST && here.vnum == 1207) ||
	(extra.direction == constants.WEST && here.vnum == 1208) )
{
	actor.send("You try to leave, but the web blocks your way!");
	return 0;
}
return 1;

};

