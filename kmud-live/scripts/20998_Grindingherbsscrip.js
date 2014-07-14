var script20998 = function(self, actor, here, args, extra)
{
	waitpulse 12;
var mortar = undefined;
if( actor.hasItem(20999,false,true,false) ) {
    waitpulse 6;
    mortar = actor.eq( constants.WEAR_HOLD );
    waitpulse 6;
    actor.send("You start mashing the herb in the mortar with the pestle.");
    }
else {
    return;
}
waitpulse 25;
if( mortar.contents.length == 0 ) {
    actor.send("There's nothing in here to grind!");
    return;
}
else if( mortar.contents.length == 1) {
    waitpulse 3;
}
else if( mortar.contents.length > 1) {
    actor.send("You don't want to mix these items together!");
    return;
}
if( mortar.contents[0].vnum == 20998) {
    mortar.contents[0].extract();
    mortar.loadObj(20981);
}
else {
    actor.send("Sorry, this isn't possible yet.");
    return;
}

};

