let script9 = function(self, actor, here, args, extra) {
	_block;
	getCharCols(actor);

	let vArgs = getArgList(args);
	vArgs.splice(0, 1);
	let searchTerm = vArgs.join(" ");
	let hideFooter = false;
	let id;

	if (searchTerm.length > 0) {
		let file = global.helpManager.getSearchedFile(searchTerm);
		if (!file) {
			actor.send(red + "Your search did not return any results." + nrm);
			return;
		}
		actor.send(bld + cyn + "Your search returned the following page:" + nrm);
		id = file.id;
		hideFooter = true;
	}
	else {
		hideFooter = false;
		actor.attach(13);

		actor.send("You bust out your newbie guide and start reading:");
		act("$n busts out $s newbie guide and starts reading.", true, actor, null, null, constants.TO_ROOM);
	}

	if (id)
		global.helpManager.setCurrentFile(actor, id);
	else {
		global.helpManager.setCurrentFile(actor, 611);
	}

	global.helpManager.getHelpPage(actor, global.helpManager.getCurrentFile(actor), hideFooter);
}