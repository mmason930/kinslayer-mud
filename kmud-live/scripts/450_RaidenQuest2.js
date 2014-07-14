var script450 = function(self, actor, here, args, extra)
{
	waitpulse 10;
here.echo("The bandit pales and begins to dry wash his hands.");
waitpulse 12;
self.say("We were hired to take the Daughter-Heir...I didn't see the mans face..b-but he gave us a key to the place we were supposed to deliver her.");
waitpulse 24;
here.echo("Morgase rises from her throne, her face full of fury. 'Deliver her?! She is not some sack of wheat!' She collapses onto her throne, tears welling up in her angry eyes again.");
waitpulse 15;
here.echo("The bandit lets out a wail at the Queen's shout, and produces a small key from his tattered clothes.");
waitpulse 12;
self.loadObj(2201);
self.comm("give key avaine");
here.echo("The bandit shoves the key into Raiden's hands.");
waitpulse 6;
self.say("Please just don't kill me!");
waitpulse 12;
here.echo("Morgase studies the bandit closely before waving her hand and saying, 'Take him to the dungeons.' She then returns her gaze to Raiden.");
waitpulse 15;
here.echo("The two guards drag the bandit away, who howls in protest.");
waitpulse 15;
here.echo("Morgase says in a firm voice, 'Bring back my daughter, Raiden. You are dismissed.' She then turns to confer with Reene again.");

};

