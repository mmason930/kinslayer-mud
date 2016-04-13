var script1200 = function(self, actor, here, args, extra) {
	if( actor.inClan(15) && actor.quest('2009-BlackTower2') == 0 || actor.quest('2009-BlackTower2') == 2 )
	{
	    wait 1;
		self.comm("peer");
		wait 2;
		self.say("Did he send you? To... To help with my dilemma?");
		wait 3;
		self.say("Very well, let me explain.");
		wait 3;
		self.say("I was wandering through the path when I took a wrong turn and stumbled upon a well.");
		wait 3;
		self.say("By then I was too tired to head back, at least not right away. So I leaned against it to take a minute to rest.");
		wait 3;
		self.say("A few minutes later, a bunch of spiders came crawling out, and the room was filled with a strange mist!");
		wait 3;
		self.say("By the time I fled, I cared not about what caused it, for I never had any plans on going back there");
		wait 3;
		self.say("But the game nearby is running low, and most of the animals by that well have not been hunted. It may be necessary that I return to the land near the well.");
		wait 3;
		self.say("So, I need you to go inspect the bloody thing. I swear, it is haunted, and I dare not go myself. I'm too old to be chasing after ghosts.");
		wait 3;
		self.say("If you bring me proof that you've rid it of whatever evil haunts it, I will make it worth your while.");
		wait 3;
		self.comm("sigh");
		actor.qval('2009-BlackTower2', 2);
	}
}