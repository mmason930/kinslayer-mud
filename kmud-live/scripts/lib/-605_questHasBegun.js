function questHasBegun(actor, questName)
{
	return (actor.quest(questName + "-HAS-BEGUN") == 1 ? true : false);
}
