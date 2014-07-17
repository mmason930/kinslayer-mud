var script1219 = function(self, actor, here, args, extra) {
	var obj = self;
	var cont;
	waitpulse 3;
	if( !(cont = obj.containedBy) )
		return;
	if( !cont.extraFlags(constants.ITEM_CHEST) )
		return;
	wait 2;
	if( obj.containedBy == cont ) {
		here.echo("A loud hissing noise begins to emanate from the chest containing the shocklance!");
		wait 5;
	}
	else {
		here.echo("The shocklance cools off, and is no longer in danger of exploding.");
		return;
	}
	if( obj.containedBy == cont ) {
		here.echo("The hissing noise gets unbearably loud, and a bright white light begins to shine from inside!");
		wait 10;
	}
	else {
		here.echo("The shocklance cools off, and is no longer in danger of exploding.");
		return;
	}
	if( obj.containedBy == cont ) {
		here.echo("You start to smell smoke! You better take the shocklance out QUICK!");
		wait 10;
	}
	else {
		here.echo("The shocklance cools off, and is no longer in danger of exploding.");
		return;
	}
	if( obj.containedBy == cont ) {
		here.echo("BOOM! A large explosion rocks the area, destroying the chest and all of its contents!");
		cont.extract();
	}
	else {
		here.echo("The shocklance cools off, and is no longer in danger of exploding.");
		return;
	}
}