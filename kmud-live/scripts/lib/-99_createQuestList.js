// REMEMBER TO ADD JSCRIPTS 13250, 13247, and 13248 TO YOUR QUEST MASTER!!!!
function itemCount(vnum) {
	return ( function(actor) {return actor.getItems(vnum,true,true,true).length;} );
}

// function getQuestName( entryObj ) {
	// var validateName = function() {
		// var qName = getQuestByName(entryObj.questName);
		// if ( qName == null && entryObj.questID != undefined ) {
			// qName = getQuestById(entryObj.questID).questName;
		// }
		
	// return ( function() { 
function scalpCount(param) {
	if ( isNumber(param) ) {
		return ( function(actor) { return actor.findPlayerScalps(Number(!actor.race),param).length; } );
	}
	if ( Array.isArray(param) ) {
		return ( function(actor) { return actor.findScalpsByNameAndRace(param,Number(!actor.race)).length; } );
	}
	return null;
}
/*** Dialogue Class ***/
function QuestDlg( _pulses, _handle )
{
	this.pulses = _pulses;
	this.handle = _handle;
	return this;
}
/*** Dialogue Callback ***/
function questDlgHandler( aList )
{
	var quest   = aList[0];
	var dIndex  = aList[1];
	var self    = aList[2];
	var actor   = aList[3];
	var dialogs = aList[4];

	eval( dialogs[ dIndex ].handle );
	if( ++dIndex < dialogs.length ) {
		setTimeout( dialogs[dIndex].pulses, questDlgHandler, [quest,dIndex,self,actor,dialogs] );
	}
}
/*** Get a quest by its name ***/
function getQuestByName( qName )
{
	var getKey = function( quest ) {
		return quest.questName;
	};
	
	var index = binarySearch( global.vQuests, qName, getKey, 0, global.vQuests.length-1 );
	return ( index == -1 ? null : global.vQuests[index] );
	// for each( var quest in global.vQuests ) {
		// if ( !str_cmp(quest.questName,qName) ) {
			// return quest;
		// }
	// }
	// return null;
}
/*** Return an array of quests whose owner vnum is mVnum ***/
function getQuestsByMob( mVnum ) {
	var myQuests = [];
	if ( mVnum != -1 ) {
		for each(var quest in global.vQuests) {
			if( arrContains(quest.ownerVnum,mVnum) == true )
				myQuests.push( quest );
		}
	}
	return myQuests;
}


