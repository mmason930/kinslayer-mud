/*******************************************************************
 *                                                                 *
 * questDialogue()                                                 *
 * Arguments: questMaster - the quest master                       *
 *            quest - The quest from which the dialogue is derived *
 *            actor - the person doing the quest                   *
 * Returns: Nothing                                                *
 * Purpose: Handles script dialogue with proper event handling     *
 *                                                                 *
 * ~~~ By: Galnor 01/10/2010                                       *
 *                                                                 *
 *******************************************************************/
function questDialogue( questMaster, actor, quest )
{
	mudLog(constants.BRF, 100, "Quest Master: " + questMaster + ", Actor: " + actor + ", quest: " + quest + ", Pulses: " + quest.dialogue[0].pulses);
	if( quest.dialogue.length > 0 ) {
		setTimeout( quest.dialogue[0].pulses, questDlgHandler, [quest, 0, questMaster, actor] );
	}
}
