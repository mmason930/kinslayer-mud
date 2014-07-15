/*** Galnor 12/19/2009 - Set the quest to an initiated state ***/
function questBegin(actor, questName)
{
	actor.qval(questName + "-HAS-BEGUN", 1);
}
