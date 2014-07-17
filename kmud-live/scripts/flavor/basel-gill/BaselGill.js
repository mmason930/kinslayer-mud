var script172 = function(self, actor, here, args, extra) {
	function gillEmotes(player)
	{
		var rand = random(1,6);
		switch( rand )
		{
		case 1:
			player.comm("think");
			break;
		case 2:
			player.comm("emote rubs his chin in thought.");
			break;
		case 3:
			player.comm("say Get on with it!");
			break;
		case 4:
			player.comm("emote yawns.");
			break;
		case 5:
			player.comm("say Are you awake? It's your bloody turn.");
			break;
		case 6:
			player.comm("emote scratches his butt absently.");
			break;
		}
	}
	function manEmotes(player) {
		var rand = random(1,7);
		switch( rand )
		{
		case 1:
			player.comm("emote mutters as he looks at the board.");
			break;
		case 2:
			player.comm("Let's try to finish this before you die, old man!");
			break;
		case 3:
			player.comm("twiddle");
			break;
		case 4:
			player.comm("emote mutters something, and peers around.");
			break;
		}
	}
	function makeMove(player) {
		player.comm("emote picks up a piece and carefully places it on the board.");
		setSval(getRoom(121),121,"whoseTurn",(player.vnum == 101 ? 150 : 101))
	}
	function getCurrentPlayer() {
		var room = getRoom(121);
		var vnum = getSval(room,121,"whoseTurn");
		if( !vnum )
			return getMobAtRoom(121,101);
		return getMobAtRoom(121,vnum);
	}
	function getOtherPlayer() {
		var player = getCurrentPlayer();
		if( !player || !player.isValid )
			return null;
		return getMobAtRoom(121, (player.vnum == 150 ? 101 : 150) );
	}
	var player = getCurrentPlayer();
	var other = getOtherPlayer();
	if( !player || !other )
		return;
	var rand = random(1,2);
	if( rand == 1 )
		makeMove(player);
	else if( other.vnum == 150 )
		manEmotes(other);
	else if( other.vnum == 101 )
		gillEmotes(other);
}