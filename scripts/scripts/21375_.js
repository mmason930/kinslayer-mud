var script21375 = function(self, actor, here, args, extra) {
	function chase( vArgs ) {
		var actor = vArgs[0];
		var pTarget = vArgs[1];
		
		if( pTarget != getSval(actor, 21375, "chasing") ) {
			return;
		}
		
		var tRoom;
		if( isNumber(pTarget) )
			tRoom = getRoom( pTarget );
		else
			tRoom = pTarget.room;
		
		var iStep = actor.room.firstStep( tRoom );
		if( iStep < 0 ) {
			if( actor.room == tRoom ) {
				if( !isNumber(pTarget) ) {
					actor.comm("hit " + pTarget.name);
					actor.comm("bash");
				}
				return;
			}
		}
		else {
			var rBefore = actor.room;
			var sCommand = dirToText( iStep );
			actor.comm( sCommand );
			
			if( rBefore == actor.room ) {
				if( actor.room.doorIsClosed( iStep ) ) {
					actor.comm("open " + actor.room.doorName( iStep ) );
					if( actor.room.doorIsClosed( iStep ) ) {
						actor.comm("call");
					}
				}
				else
					return;
			}
		}
		setTimeout( 2, chase, [actor, pTarget] );
	}
	var vArgs = getArgList(args);
	if( !str_cmp(vArgs[0], "chase") ) {
		_block;
		if( vArgs.length < 2 ) {
			actor.send("Chase who?");
			return;
		}
		var pTarget = actor.getCharVis( vArgs[1] );
		if( !pTarget ) {
			if( !isNumber(vArgs[1]) ) {
				actor.send("Nobody by that name, here.");
				return;
			}
			else {
				pTarget = parseInt( vArgs[1] );
			}
		}
		setSval( actor, 21375, "chasing", pTarget );
		
		chase( [actor, pTarget] );
	}
	if( !str_cmp(vArgs[0], "mvs") ) {
		actor.mvs += 50;
		_block;
	}
	if( !str_cmp(vArgs[0], "stop") ) {
		_block;
		setSval( actor, 21375, "chasing", undefined );
	}
}