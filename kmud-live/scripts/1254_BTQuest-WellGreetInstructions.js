var script1254 = function(self, actor, here, args, extra)
{
	if( actor.quest('The Haunted Well') == 2 ) {
//Give them brief instructions.
    waitpulse 1;
	actor.questSend("Here it is! The 'haunted' well that the old man spoke of.");
	wait 3;
	actor.questSend("After you INSPECT the well, you notice some crevices in the side.");
	wait 3;
	actor.questSend("You notice that only nine of them are unfilled. The rest are filled with variously colored marbles.");
	wait 3;
	actor.questSend("In order to draw the 'haunted' force out, you need to PLACE the missing marbles into the well.");
	wait 3;
	actor.questSend("Once you find them, that is!");
	actor.qval('The Haunted Well', 10);
}

};

