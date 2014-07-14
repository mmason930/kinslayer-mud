var script1022 = function(self, actor, here, args, extra)
{
	var vArgs = getArgList(args);
if( vArgs[1] == "sandwich" ){
_block;
	actor.send('You start preparing a sandwich.');
	actor.startTimer(2);
	var success = runTimer(actor);
	if (success){
	actor.send('You have successfully made a delicious sandwich!');
	here.loadObj(2113);
	}
}

};

