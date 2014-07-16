delete bootHelpeditOLC;
function bootHelpeditOLC()
{
	var oConfig = new Object();
	var mode;
	oConfig.type = "helpedit";
	oConfig.modes = [];
	
/** MAIN MENU **/
	mode = new Object();
	mode.mode = "MODE_MAIN";
	mode.parser = function(actor,fLetter,vArgs) 
	{
		var helpTopics = global.helpManager.getHelpFilesByParentId(null);
		var cmd = vArgs[0];
		for (var i = 0; i < helpTopics.length; i++) {
			if (cmd == (i+1)) {
				actor.getOLC().helpFile = helpTopics[ i ];
				actor.getOLC().switchToMode("MODE_EDIT_FILE");
				return;
			}
		}
		if (fLetter == "Q") {
			actor.cleanupOLC();
			return;
		}
		if (fLetter == "N") {
			var helpFile = global.helpManager.createHelpFile();
			
			helpFile.createdByUserId = actor.id;
			helpFile.createdDatetime = new Date();
			
			actor.getOLC().helpFile = helpFile;
			actor.getOLC().switchToMode("MODE_EDIT_FILE");
			return;
		}
	}
	mode.display = function(actor) {
		/** Initialize data for a new help file **/
		
		var helpTopics = global.helpManager.getHelpFilesByParentId(null);
		
		actor.send("Help Editor\n" + strPadding("", "_", 85, "right") + "\n");
		actor.send("Select a number to edit a topic:");
		actor.send(" ");
		
		for ( var i = 0; i < helpTopics.length; i++) {
			actor.send( grn + (i+1) + nrm + ") "+helpTopics[i].name );
		}
		actor.send(" ");
		actor.send( grn + "N" + nrm + ") Create New Help Topic" );
		actor.send( grn + "Q" + nrm + ") Quit" );
	}
	oConfig.modes.push( mode );
/** EDIT FILE **/
	mode = new Object();
	mode.mode = "MODE_EDIT_FILE";
	mode.parser = function(actor,fLetter,vArgs) 
	{
		var file = actor.getOLC().helpFile;
		var cmd = vArgs[0];
		if (fLetter == "Q") {
			
			var isNew = file.isNew();
			
			file.lastModifiedByUserId = actor.id;
			file.lastModifiedDatetime = new Date();
			
			global.helpManager.saveHelpFileToDatabase(file);
			
			//If the help file is new, let's add it internally as well.
			if(isNew) {
			
				global.helpManager.addHelpFileInternally(file);
			}
			
			delete actor.getOLC().helpFile;
			actor.getOLC().switchToMode("MODE_MAIN");
			return;
		}
//		else if (fLetter == "D" && global.helpManager.getHelpFilesByParentId(file.id).length == 0) {
//			actor.getOLC().switchToMode("MODE_DELETE_FILE");
//			return;
//		}
		else if (fLetter == "N") {
			actor.getOLC().switchToMode("MODE_EDIT_FILE_NAME");
			return;
		}
		else if (fLetter == "S") {
			actor.getOLC().switchToMode("MODE_EDIT_FILE_SYNTAX");
			return;
		}
		else if (fLetter == "E") {
			actor.getOLC().switchToMode("MODE_EDIT_FILE_DESCRIPTION");
			return;
		}
		else if (fLetter == "K") {
			actor.getOLC().switchToMode("MODE_EDIT_FILE_KEYWORDS");
			return;
		}
		else if (fLetter == "C") {
			actor.getOLC().switchToMode("MODE_EDIT_CHILDREN");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var helpFile = actor.getOLC().helpFile;
		
		actor.send("Help Editor - "+grn+"Edit "+(actor.quest("NEW_FILE") == 1 ? "Topic" : "File")+nrm+"\n"+strPadding("","_",85,"right")+"\n");
		actor.send( grn + "N" + nrm + ") Name: " + helpFile.name );
		actor.send( grn + "S" + nrm + ") Syntax: " + helpFile.syntax );
		actor.send( grn + "E" + nrm + ") Description: " + helpFile.description );
		actor.send( grn + "K" + nrm + ") Keywords: " + helpFile.keywords );
		actor.send( grn + "C" + nrm + ") Children");
		actor.send(" ");
//		actor.send( grn + "D" + nrm + ") Delete File");
		actor.send( grn + "Q" + nrm + ") Save and Quit" );
	}
	oConfig.modes.push( mode );
/** DELETE FILE **/
	mode = new Object();
	mode.mode = "MODE_DELETE_FILE";
	mode.parser = function(actor,fLetter,vArgs) 
	{
		var cmd = vArgs[0];
		var file = global.helpManager.getHelpFileById(actor.getOLC().id);
		if (fLetter == "N") {
			actor.getOLC().switchToMode("MODE_MAIN");
			return;
		}
		if (fLetter == "Y") {
			getCharCols(actor);
			actor.send(red+"File '"+file.name+"' deleted."+nrm);
			global.helpManager.deleteHelpFileFromDatabase(file);
			actor.getOLC().switchToMode("MODE_MAIN");
			return;
		}
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send("Are you sure you wish to delete this file (Y/N) ?");
		return;
	}
	oConfig.modes.push( mode );
/** EDIT FILE NAME**/
	mode = new Object();
	mode.mode = "MODE_EDIT_FILE_NAME";
	mode.parser = function(actor,fLetter,vArgs) 
	{//
		actor.getOLC().helpFile.name = vArgs.join(" ");
		actor.getOLC().switchToMode("MODE_EDIT_FILE");
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send("Enter the help file's name: ");
		return;
	}
	oConfig.modes.push( mode );
/** EDIT FILE SYNTAX**/
	mode = new Object();
	mode.mode = "MODE_EDIT_FILE_SYNTAX";
	mode.parser = function(actor,fLetter,vArgs) 
	{
		actor.getOLC().helpFile.syntax = vArgs.join(" ");
		actor.getOLC().switchToMode("MODE_EDIT_FILE");
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send("Enter the help file's syntax: ");
		return;
	}
	oConfig.modes.push( mode );
/** EDIT FILE KEYWORDS**/
	mode = new Object();
	mode.mode = "MODE_EDIT_FILE_KEYWORDS";
	mode.parser = function(actor,fLetter,vArgs) 
	{
		actor.getOLC().helpFile.keywords = vArgs.join(" ");
		actor.getOLC().switchToMode("MODE_EDIT_FILE");
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send("Enter the list of keywords, separated by spaces: ");
		return;
	}
	oConfig.modes.push( mode );
/** EDIT FILE DESCRIPTION**/
	mode = new Object();
	mode.mode = "MODE_EDIT_FILE_DESCRIPTION";
	mode.parser = function(actor,fLetter,vArgs) 
	{
		actor.getOLC().helpFile.description = vArgs.join(" ");
		actor.getOLC().switchToMode("MODE_EDIT_FILE");
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		actor.send("Enter the help file's description: ");
		return;
	}
	oConfig.modes.push( mode );
/** EDIT CHILDREN**/
	mode = new Object();
	mode.mode = "MODE_EDIT_CHILDREN";
	mode.parser = function(actor,fLetter,vArgs) 
	{
		var option = vArgs[ 0 ];
		if(isNumber(option)) {
			option = parseInt(option) - 1;//The numbering starts at 1, so we subtract to get the index.
			var children = global.helpManager.getHelpFilesByParentId(actor.getOLC().helpFile.id);
			
			if(option < 0 || option >= children.length) {
				actor.send("That is not a valid option.");
			}
			else {
				var helpFile = children[ option ];
				actor.getOLC().helpFile = helpFile;
				actor.getOLC().switchToMode("MODE_EDIT_FILE");
			}
		}
		else if (fLetter == "Q") {
			actor.getOLC().switchToMode("MODE_EDIT_FILE");
		}
		else if (fLetter == "A") {
		
			var newHelpFile = global.helpManager.createHelpFile(actor.getOLC().helpFile.id);
			actor.getOLC().helpFile = newHelpFile;
			actor.getOLC().switchToMode("MODE_EDIT_FILE");
		}
		return;
	}
	mode.display = function(actor) {
		getCharCols(actor);
		var file = actor.getOLC().helpFile;
		actor.send("Help Editor - "+grn+"Edit Children"+nrm+"\n"+strPadding("","_",85,"right")+"\n");
		actor.send("Select a number to edit a child:");
		actor.send(" ");
		var children = global.helpManager.getHelpFilesByParentId(file.id);
		for ( var index = 0; index < children.length; ++index) {
			actor.send( grn + (index+1) + nrm + ") "+children[index].name );
		}
		actor.send(" ");
		actor.send( grn + "A" + nrm + ") Add Child" );
		actor.send( grn + "Q" + nrm + ") Quit" );
	}
	oConfig.modes.push( mode );
	
	vOLC.push( oConfig );
}
bootOLC();










