var script6622 = function(self, actor, here, args, extra)
{
	//Alder
//August 2010
//Quest: Armor of Ghosts
/** Command Trigger: enter **/
var qName = "Armor of Ghosts";
var vArgs = getArgList(args);
if ( actor.quest(qName) == 6 ) {
	_block;
	getCharCols(actor);
	actor.send(bld+cyn+"Reaching into the puddle of water you pull a stone lever which triggers a trapdoor by the monument to spring open. You then lower yourself into the chamber below the statue."+nrm);
	waitpulse 20;
	actor.moveToRoom(getRoom(6698));
	actor.comm("look");
}

};

