var script8196 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
_block;
if(!actor.inClan(12)) {
    return;
}
if(actor.position != constants.POS_STANDING){
    actor.send("You're not in the correct position.");
    return;
}
waitpulse 5;
actor.send("You open the flamingdoor and exit the room.");
echoaround(actor, actor.name + " opens the flamingdoor and leaves.");
actor.moveToRoom(getRoom(8168));
getRoom(8168).echoaround(actor, actor.name + " enters from the west.");
actor.comm("look");
return;

};

