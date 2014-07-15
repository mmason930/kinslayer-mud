var script1209 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	if( !str_cmp(vArgs[1], "web") && !actor.fighting )
	{
		_block;
		actor.startTimer(4.2);
		var success = runTimer(actor);
		if (success)
		{
			actor.send("Your powerful bash smashes the web into tatters, clearing the way through!");
			here.echoaround(actor, actor.name + "'s powerful bash smashes a huge spiderweb into tatters, clearing the way through!\r\n");
			getRoom( (here.vnum == 1207 ? 1208 : 1207) ).echo("The huge spiderweb is destroyed from the other side!");
			var itemListA = getRoom(1207).items;
			var itemListB = getRoom(1208).items;
		   
			for(var itemIndex = 0;itemIndex < itemListA.length;++itemIndex) {
		   
				if(itemListA[ itemIndex ].vnum == 20639 || itemListA[ itemIndex ].vnum == 20640) {
				
					itemListA[ itemIndex ].extract();
				}
			}
		   
			for(var itemIndex = 0;itemIndex < itemListB.length;++itemIndex) {
		   
				if(itemListB[ itemIndex ].vnum == 20639 || itemListB[ itemIndex ].vnum == 20640) {
				
					itemListB[ itemIndex ].extract();
				}
			}
	//	   getSval(getRoom(1207), 1208, "web").extract();
	//	   setSval(getRoom(1207), 1208, "web", 0);
	//	   getSval(getRoom(1208), 1208, "web").extract();
	//	   setSval(getRoom(1208), 1208, "web", 0);
		}
	}
	else if( actor.fighting && !str_cmp(vArgs[1], "web") )
	{
		actor.send("You're too busy concentrating on your current target!");
		return;
	}
	else
	{
		_noblock;
		return;
	}
}