var script1251 = function(self, actor, here, args, extra)
{
	if( !actor.inClan(15) || actor.quest('The Haunted Well') != 10 )
    return;
_block;
var vArgs = getArgList(args);
var objMove;
var objTarget;
if( vArgs.length <= 2 )
{
    actor.send("Place what into what?");
	return;
}
objMove = getObjInListVis(actor, vArgs[1], actor.inventory);
if( !objMove )
{
    actor.send("You don't have a " + vArgs[1] + ".");
	return;
}
objTarget = getObjInListVis(actor, vArgs[2], here.items);
if( !objTarget )
{
    actor.send("You don't see a " + vArgs[2] + ".");
	return;
}
if( objTarget.vnum == 1250 && (objMove.vnum == 3170 || objMove.vnum == 3171 || objMove.vnum == 3172) )
{
    var aSlots = [];
	var color;
	if( objMove.vnum == 3170 )
	{
	    color = "green";
	    aSlots.push(1);
	    aSlots.push(2);
	    aSlots.push(3);
	}
	else if( objMove.vnum == 3171 )
	{
	    color = "blue";
	    aSlots.push(4);
	    aSlots.push(5);
	    aSlots.push(6);
	}
	else if( objMove.vnum == 3172 )
	{
	    color = "red";
	    aSlots.push(7);
	    aSlots.push(8);
	    aSlots.push(9);
	}
	var found = false;
	var j;
	for(j = 0;j < aSlots.length;++j)
	{
	    if( !getSval(objTarget, 1250, 'brick' + aSlots[j]) )
		{
		    found = true;
			break;
		}
	}
	if( !found )
	{
	    actor.send("There is nowhere to place that marble.");
		return;
	}
    actor.send("You place " + objMove.name + " into a slab in " + objTarget.name + ".");
//	here.echo("Name: " + ("brick"+aSlots[j]) + " - " + color);
	setSval(objTarget, 1250, 'brick' + aSlots[j], color);
	if( j == aSlots.length-1 ) {
	    here.echo(capFirstLetter(self.name) + " emits a shimmering "  + color + " glow.");
    }
	
    objMove.extract();
}
else
{
    actor.send("You don't seem to be able to place that there.");
	return;
}
	

};

