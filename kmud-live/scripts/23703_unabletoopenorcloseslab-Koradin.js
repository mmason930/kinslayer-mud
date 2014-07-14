var script23703 = function(self, actor, here, args, extra)
{
	//Room trigger so people can't open or close the slab door.
//Maerwynn smob area.
//Koradin - December 2009
var arg_array = getArgList(args);
if (arg_array.length < 2)
{
_noblock;
return;
}
if (arg_array[1] != "slab")
{
_noblock;
return;
}
actor.send("The slab is far too heavy to move!");
_block;
return;

};

