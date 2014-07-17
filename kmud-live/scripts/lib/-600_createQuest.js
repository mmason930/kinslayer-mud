function createQuest(questName, itemsRequired, qpReward, itemReward, exp, gold, num)
{
    this.questName = questName;
//    this.qval = qval;
    this.items = itemsRequired;
    this.qp = qpReward;
    this.itemReward = itemReward;
	this.exp = exp;
	this.gold = gold;
	this.dialogue = [];
	this.priorQuests = [];
	this.skillArray;
	if( num )
        this.num = num
	else
	    this.num = 1;
}
