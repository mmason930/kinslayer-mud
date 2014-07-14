var script20700 = function(self, actor, here, args, extra)
{
	//No WHERE or YELL command
//Koradin - May 2010
var vArgs = getArgList(args);
if (!strn_cmp(vArgs[0],"where",2) && !strn_cmp(vArgs[0],"yell",2))
{
	_noblock;
	return;
}
if (strn_cmp(vArgs[0],"where",2))
	var message = "You lose all sense of direction in this place...";
else if (strn_cmp(vArgs[0],"yell",2))
	var message = "Your sound can't carry that far before being swallowed up...";
_block;
actor.send(message);

};

