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
		let msg;
		if (!file) {
			msg = red + "Your search did not return any results. Showing Table of Contents:";
		}
		else {
			msg = bld + cyn + "Your search returned the following page:";
			id = file.id;
		}
		actor.send(msg + nrm);
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