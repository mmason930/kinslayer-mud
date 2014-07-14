var script670 = function(self, actor, here, args, extra)
{
	//Alder
//May 2010
/************************************************************
 *                       [ Extract ]                        *
 *                      Global Script                       *
 * Purpose: perform actions when players are extracted      *
 * Current events:                                          *
 *  - Remove actor's array from global.playerTitleArrays    *
 *  - Remove actor's journal object from globJournals       *
 *  - Clear actor's recipes                                 *
 *  - Extract actor's issued horse, if applicable			*
 *  - Delete actor's quest inventory                        *
 *  - Force actor to leave Arena                            *
 *                                                          *
 *  ......                                                  *
 * Please append any new events to this list                *
 * Syntax: actor is the person being extracted              *
 *                                                          *
 * NOTE: NEVER WAIT IN THIS SCRIPT                          *
 ************************************************************/
if ( self.vnum != -1 || getObjectClass(self) != "JSCharacter" ) {
	return;
}
var actor = self;
var id = actor.id;
var horse = getSval( actor, 207, "horse" );
// Remove actor's title array from global.actorTitleArrays
	accessTitles(id,"clear");
// Removes actor's recipe array from global.playerRecipes
	actor.accessRecipes("clear");
// Remove actor's journal object from globJournals
	var len = globJournals.length;
	for ( var i = 0; i < len; i++ ) {
		if ( globJournals[i].ownerID == id ) {
			globJournals.splice(i,1);
			getRoom(98).echo("Journal deleted: ID "+id);
			break;
		}
	}
// Get rid of any horse the actor might have issued
	if( horse )
	{
		horse.room.echo( horse.name + " senses that " + horse.hisher + " owner has left, and returns home." );
		horse.extract();
	}
	delete actor._questInv;
// If actor is in Arena, force exit
	if (arrContains(global.arenaZones, actor.room.zoneVnum) || arrContains(global.arenaPlayers, actor) || arrContains(getWaitingPlayers("all"), actor)) {
		actor.detach(14212);
		actor.detach(14214);
		actor.detach(14215);
		actor.detach(14222);
		actor.detach(14223);
		actor.detach(14224);
		actor.comm("exit arena");
		if (actor.race == constants.RACE_HUMAN)
			var newR = 500;
		else
			var newR = 502;
		actor.moveToRoom(getRoom(newR));
	}
// ...



};

