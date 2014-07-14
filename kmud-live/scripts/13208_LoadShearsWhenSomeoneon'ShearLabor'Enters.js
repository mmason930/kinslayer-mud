var script13208 = function(self, actor, here, args, extra)
{
	// actor.send("Entered...");
if ( actor.quest('Shear Labor') <= 0 ) {
	return;
}
var check1 = false;
var check2 = false;
for each( var item in self.items ) {
	if (item.vnum == 13208)
		check1 = true;
	if ( item.vnum == 20994 )
		check2 = true;
}
if ( check1 == false ) {
	// actor.send("Loading...");
	self.loadObj(13208);
}
if ( check2 == false ) {
	self.loadObj(20994);
}

};

