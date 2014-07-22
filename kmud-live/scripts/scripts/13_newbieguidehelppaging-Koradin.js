var script13 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	_block;
	
	if(strn_cmp(vArgs[0], "Close", 1))
	{
		actor.send("You close the newbie guide carefully and put it away.");
		act("$n closes $s newbie guide carefully and puts it away.",true,actor,null,null,constants.TO_ROOM);
		actor.detach(13);
		return;
	}
	if(strn_cmp(vArgs[0], "Back", 1))
	{
		var file = global.helpManager.getCurrentFile(actor);
		
		if(file.parentId == null) { // At the Table of Contents, close the book
			actor.send("You close the newbie guide carefully and put it away.");
			act("$n closes $s newbie guide carefully and puts it away.",true,actor,null,null,constants.TO_ROOM);
			actor.detach(13);
		}
		else {
			global.helpManager.setCurrentFile(actor, file.parentId);
			global.helpManager.getHelpPage(actor, global.helpManager.getCurrentFile(actor));
		}
		
		return;
	}
	
	var file = global.helpManager.getCurrentFile(actor);
	var fileId = file == null ? null : file.id;
	var childrenFiles = global.helpManager.getHelpFilesByParentId(fileId);
	
	if (isNaN(vArgs[0]) || vArgs[0] < 1 || vArgs[0] > childrenFiles.length)
	{
		getCharCols(actor);
		actor.send(red + "That is not a valid choice." + nrm);

		setTimeout(3, function() {

			var helpFile = global.helpManager.getCurrentFile(actor);

			if(helpFile != null) {
				global.helpManager.getHelpPage(actor, helpFile);
			}
		});
		return;
	}
	
	var newHelpFile = childrenFiles[ vArgs[0] - 1 ];
	global.helpManager.setCurrentFile(actor, newHelpFile.id);
	global.helpManager.getHelpPage(actor, newHelpFile);
};