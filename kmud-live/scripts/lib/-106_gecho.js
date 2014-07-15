/*************************************************************************************
 *                                                                                   *
 * gecho()                                                                           *
 * Arguments: - str: Serves two purposes. Either it is a sting, and is simply the raw*
 *                   message passed to the players, of it is a function which takes  *
 *                   in a JSCharacter as an argument, and returns the message. This  *
 *                   is useful for getting color preferences that cannot be assumed. *
 *            - predicate: An option argument. Takes in a JSCharacter, and returns   *
 *                   true if the message should be sent, and false if not.           *
 * Purpose: Sends a message to every player in the game.                             *
 *                                                                                   *
 * ~~~ By: Galnor 03/12/2010                                                         *
 *                                                                                   *
 *************************************************************************************/
function gecho( str, predicate )
{
	var players = getConnectedPlayers();
	for(var i = 0;i < players.length;++i)
	{
		if( predicate == undefined || ( predicate != undefined && predicate( players[ i ] ) != false ) )
		{
			if( getObjectClass( str ) == "Function" )
				players[ i ].send( str(players[i]) );
			else
				players[ i ].send( str );
		}
	}
}
