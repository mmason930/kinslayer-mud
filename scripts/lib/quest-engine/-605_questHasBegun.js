function questHasBegun(actor, questName) {
	return Quest.getByName(questName).hasBegun(actor);
	//return (actor.quest(questName + "-HAS-BEGUN") == 1 ? true : false);
}
