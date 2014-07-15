var script9804 = function(self, actor, here, args, extra) {
	var vArgs = getArgList( args );
	if( vArgs.length < 2 ) {
		return;
	}
	var eq = [];
	for(var i = 0;i < constants.NUM_WEARS;++i)
	{
		if( actor.eq( i ) )
			eq.push( actor.eq(i) );
	}
	var targetObj = getObjInListVis( actor, vArgs[1], [eq,actor.inventory,here.items] );
	if( !targetObj || targetObj.name != "a rusty chain saw" )
		return;
	_block;
	var status = getSval(targetObj, 9804, "onoff");
	if( status == "on" ) {
		actor.send("It appears to be on already.");
		return;
	}
	var gasoline = getSval(targetObj, 9804, "gasoline");
	var chanceOfStarting = 80;
	if( gasoline != undefined && gasoline < 10 ) {
		chanceOfStarting = 0;
	}
	var roll = random(1,100);
	actor.send("You jerk your arm outward, attempting to start the chain saw.");
	act("$n jerks his arm outward, attempting to start his chain saw.", true, actor, null, null, constants.TO_ROOM);
	var predicate = function(a) { 
		return (a.position >= constants.POS_SLEEPING);
	}
	if(chanceOfStarting >= roll) {
		here.echo("The motor of " + targetObj.name + " roars loudly as it starts up.");
		here.echoToNeighbors("The loud roar of a motor starting up can be heard from nearby.", 3, predicate );
		setSval( targetObj, 9804, "onoff", "on" );
		function eatGas( vArgs ) {
			var saw = vArgs[0];
			var init = undefined;
			if( vArgs.length >= 2 )
				init = vArgs[1];
			if( !init ) {
				var gas = getSval(saw, 9804, "gasoline");
				if( gas == undefined ) gas = 100;
				gas = Math.max(0, gas-1);
			
				if( gas == 0 ) {
					saw.room.echo( capFirstLetter( saw.name ) + " dwindles down to a halt." );
					setSval( saw, 9804, "onoff", "off" );
				}
				setSval( saw, 9804, "gasoline", gas );
				//saw.room.echo("Gas remaining: " + gas);
			}
			if( getSval( saw, 9804, "onoff" ) == "on" )
				setTimeout( 60, eatGas, [saw] );
		}
		eatGas( [targetObj] );
	}
	else {
		here.echo("The motor of " + targetObj.name + " roars softly, then sputters out.");
		here.echoToNeighbors("The soft roar of a motor trying to start up can be heard from nearby.", 1, predicate);
	}
	
}