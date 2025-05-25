var script1022 = function(self, actor, here, args, extra) {
	const vArgs = getArgList(args);
	if( vArgs.length >= 2 && vArgs[1] === "sandwich" ) {
		_block;

		actor.send('You start preparing a sandwich.');
		actor.startTimer(2);

		let success = runTimer(actor);
		if (success) {
			actor.send('You have successfully made a delicious sandwich!');
			here.loadObj(2113);
		}
	}
}