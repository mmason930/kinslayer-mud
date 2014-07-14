var script4593 = function(self, actor, here, args, extra)
{
	_block;
var vArgs = getArgList(args);
if(actor.position != constants.POSITION_FIGHTING) {
	var dest = here;
	if(here.vnum == 4593)
		dest = getRoom(4540);
	else if(here.vnum == 4540) 
		dest = getRoom(4593);
	var timerLength = 21 - (actor.strength + actor.dexterity + actor.dexterity)/3;
	actor.send("You back away from the edge and get a running start.");
	echoaround(actor, actor.name + " runs for the ledge!!!");
	actor.startTimer(timerLength);
	var complete = runTimer(actor);
	if(complete && actor.position != constants.POSITION_FIGHTING) {
		var fail = false;
		if(actor.dexterity > 16) {
			actor.send("\nYou jump clear of the wall and land smoothly.");
			echoaround(actor, actor.name + " jumps clear of the wall!!!");
		}
		else {
			if(random(actor.dexterity*2,100) < 50) {
				dest = getRoom(4541);
				actor.send("\nYou leap clear of the wall but fall short landing on the roof and tumbling toward the street!!!!");
				echoaround(actor, actor.name + " leaps clear of the wall and all you hear after that is a sqeal!");
				fail = true;
			}
		}
		actor.moveToRoom(dest);
		actor.comm("look");
		if(fail) {
			actor.send(red+"You land with a sickening crunch."+nrm);
			echoaround(actor, actor.name + " lands with a sickening crunch!");
			actor.damage(random(50,100));
		}
		else
			echoaround(actor, actor.name + " makes graceful landing.");
	}
}
else
	actor.send("What?!");

};

