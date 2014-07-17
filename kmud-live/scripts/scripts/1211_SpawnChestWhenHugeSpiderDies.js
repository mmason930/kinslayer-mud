var script1211 = function(self, actor, here, args, extra) {
	function f( vArgs )
	{
		var room = vArgs[1];
		var chestLoads = [2039,  590, 2716, 4562, 4567];
	var chestPrcts = [ 30, 30, 30, 30, 30 ];
		if( getObjInListVis(vArgs[0], "chest", room.items) || getCharInListVis(vArgs[0], "hugespider", room.people) )
			return;
		room.echo("The webbing in the back of the room falls away, revealing a chest!");
		var chest = room.loadObj(1296);
		for( var i = 0; i < chestLoads.length; i++ )
		{
			if( random(0, 100) <= chestPrcts[i] )
				chest.loadObj( chestLoads[i] );
		}
	}
	setTimeout(3,f,[actor, here]);
}