var script1512 = function(self, actor, here, args, extra)
{
	//Bronze pick axe reverts back to strong 2h pick
//Koradin - May 2010
var actor = self.wornBy;
if (!actor)
	return;
if (self.vnum == 1510)
	var breakRoll = random(1,500);
else if (self.vnum == 1512)
	var breakRoll = random(1,375);
else if (self.vnum == 1515)
	var breakRoll = random(1,300);
if ( (breakRoll == 1 && getSval(actor,20850,"mining") == 1) || (breakRoll < 11 && getSval(actor.room,20850,"isMining") == 1) ) {
	actor.room.echo("The head of " + self.name + " cracks on impact against the rock wall.");
	actor.send("You stop using " +self.name +".");
	act("$n stops using " +self.name +".",true,actor,null,null,constants.TO_ROOM);
	actor.loadObj(1514);
	self.moveToRoom(getRoom(20899));
	self.extract();
	return;
}


};

