var script20901 = function(self, actor, here, args, extra) {
	//For quest 'Snakes and Berries'
	//Koradin - January 2010
	waitpulse 2;
	if (actor.questTaskProgress('Snakes and Berries')[1].quotient >= 1)
		return;
	else
	{
		if (!actor.hasItem(21431,true,true,true))
			actor.questSend("To gather the wild berries, type 'pick berries'.");
	}
}