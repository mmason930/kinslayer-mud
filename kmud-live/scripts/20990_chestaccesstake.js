var script20990 = function(self, actor, here, args, extra)
{
	var arg_list = getArgList(args);
if( arg_list.length <= 1 || actor.inClan( self.clan ) )
{//Person taking items from chest is member of clan. No limitations necessary.
    _noblock;
    return;
}
if( arg_list[1] == "all" )
{//Prevent raiders from cleaning the chest out.
    actor.send("Take one thing at a time, you greedy person!");
    _block;
    return;
}
if( !isName(arg_list[2], self.namelist) )
{//Item is not being taken from chest. Don't interfere.
    _noblock;
    return;
}
if( actor.vnum != -1 )
{//We don't care about mobs...
    _noblock;
    return;
}
var varName = "chestLooter" + actor.id;
var o = getSval(self, 20990, varName);
o++;
if( o > 3 )
{
    actor.send("You can't take anymore items from this chest.");
    _block;
    return;
}
actor.send("You can take " + (3 - o) + " more items from this chest.");
setSval(self, 20990, varName, o);
_noblock;

};

