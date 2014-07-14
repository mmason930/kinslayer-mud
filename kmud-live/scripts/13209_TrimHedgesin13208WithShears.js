var script13209 = function(self, actor, here, args, extra)
{
	_block;
if ( actor.hasItem(13208,true,false,false) == null ) {
	actor.send("That would require you to be using garden shears.");
	return;
}
if ( actor.quest("Shear Labor_trim Hilde Saram's hedges") != 0 ) {
	actor.send("The hedges look fine. You'd probably just ruin them.");
	return;
}
waitpulse 1;
actor.send("You begin trimming the messy hedges...");
actor.startTimer(2);
var success = runTimer(actor);
if ( success ) {
	actor.send("\nThe hedges are beginning to shape up nicely...");
}
else {
	actor.send("You cease working on the hedges...lazy bum!");
	return;
}
waitpulse 1;
actor.startTimer(3);
var success = runTimer(actor);
if ( success ) {
	actor.send("\nThe hedges look perfect! Your work here is done.\n");
	for each ( var item in actor.inventory ) {
		if ( item.vnum == 13208 ) {
			item.extract();
			break;
		}
	}
}
else {
	actor.send("You cease working on the hedges...lazy bum!");
	return;
}
waitpulse 1;
actor.updateJournalTask('Shear Labor',0);

};

