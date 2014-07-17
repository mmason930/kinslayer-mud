var script9 = function(self, actor, here, args, extra) {
	_block;
	getCharCols(actor);
	
	var vArgs = getArgList(args);
	vArgs.splice(0, 1);
	var searchTerm = vArgs.join(" ");
	var hideFooter = false;
	
	if (searchTerm.length > 0) {
		var file = global.helpManager.getSearchedFile(searchTerm);
		if (!file)
			var msg = red+"Your search did not return any results. Showing Table of Contents:";
		else {
			var msg = bld+cyn+"Your search returned the following page:";
			var id = file.id;
		}
		actor.send(msg+nrm);
		hideFooter = true;
	}
	else {
		hideFooter = false;
		actor.attach(13);
		
		actor.send("You bust out your newbie guide and start reading:\m");
		act("$n busts out $s newbie guide and starts reading.", true, actor, null, null, constants.TO_ROOM);
	}
	
	if (id)
		global.helpManager.setCurrentFile(actor, id);
	else {
		global.helpManager.setCurrentFile(actor, 611);
	}
	
	global.helpManager.getHelpPage(actor, global.helpManager.getCurrentFile(actor), hideFooter);
	
	
	
	
}