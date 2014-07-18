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
	if( quest.dialogue.length > 0 ) {
		var dialogs = quest.dialog.filter(function(dialogElement) {
			return dialogElement.pulses >= 0;
		});
		setTimeout( quest.dialogue[0].pulses, questDlgHandler, [quest, 0, questMaster, actor, dialogs] );
	}
}
