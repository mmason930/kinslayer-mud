function createQuest(questName, itemsRequired, qpReward, itemRewards, exp, gold, num)
{
    this.name = questName;
//    this.qval = qval;
    this.items = itemsRequired;
    this.qp = qpReward;
    this.itemRewards = itemRewards;
	this.exp = exp;
	this.gold = gold;
	this.dialogue = [];
	this.priorQuests = [];
	this.skills;
	if( num )
        this.maxCompletions = num
	else
	    this.maxCompletions = 1;
}
