var script21320 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if( vArgs.length < 2 )
	return;
var target = getCharInListVis(actor,vArgs[1],here.people);
if( !target )
	return;
act("You slap $N with " + self.name + ".", false, actor, null, target, constants.TO_CHAR);
act("You are slapped by $n with " + self.name + ".", false, actor, null, target, constants.TO_VICT);
act("$n slaps $N with " + self.name + ".", true, actor, null, target, constants.TO_NOTVICT);
_block;

};

