var script8168 = function(self, actor, here, args, extra)
{
	// Enter Clan Room Script for Dreadlords
var vArgs = getArgList(args);
if(!actor.inClan(12)) {
	return;
}
_block;
if(actor.position != constants.POS_STANDING){
        actor.send("You're not in the correct position.");
        return;
}
if(strn_cmp(vArgs[1],"flamingdoor",4)) {
        if(actor.affectedBy(constants.AFF_NOQUIT)) {
                actor.send("You must collect yourself first.");
        }
        else {
                actor.send("You knock on the flamingdoor.");
                waitpulse 5;
                actor.moveToRoom(getRoom(8196));
				actor.send("You enter the flamingdoor, and it swiftly closes behind you.");
                getRoom(8196).echoaround(actor, actor.name + " enters from the west, and the shadoweddoor closes behind " + actor.him_her() + ".");
				here.echo( actor.name + " is let inside the flamingdoor, which quickly closes afterward." );
                actor.comm("look");
        }
        return;
}
return;

};

