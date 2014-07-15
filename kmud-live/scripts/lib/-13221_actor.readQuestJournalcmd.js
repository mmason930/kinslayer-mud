//Alder December 2009
JSCharacter.prototype.viewMercList = function(cmd) {
	/*Close journal*/
	if ( strn_cmp(cmd,"close",1) ) {
		this.detach(13221);
		this.send("You snap closed a leather-bound journal.");
		this.room.echoaround(this, capFirstLetter(this.name) + " closes a leather-bound journal with a loud snap.");
			return;
	}
	var entryTotal = this.quest('totalJournalEntries');
	var lastPage = Math.ceil(entryTotal/2);//Two entries max per page
	var BUFFER = "\n__________________________________________________________________________\n";
	/**This first if is only used for printing the journal RIGHT after actor 'reads journal'**/
	if ( cmd == "for_initiate_read_journal" ) {
		if ( this.quest('savedJournalPage') <= lastPage )
			var currPage = this.quest('savedJournalPage');
		else
			var currPage = 1;
	}
	//Next page 
	else if ( strn_cmp(cmd,"next",1) ) {
		var currPage = this.quest('savedJournalPage')+1;
		this.room.echoaround(this, this.name + " flips a page, reading intently.");
	}
	//Back a page
	else if ( strn_cmp(cmd,"back",1) ) {
		var currPage = this.quest('savedJournalPage')-1;
		this.room.echoaround(this, this.name + " flips a page, reading intently.");
	}
	//Reread the page
	else if ( strn_cmp(cmd, "reread",1) ) {
		var currPage = this.quest('savedJournalPage');
		this.room.echoaround(this, this.name + " flips a page, reading intently.");
	}
	//Page number	
	else if ( isNumber( cmd ) ) {
		var currPage = Number(cmd);
		this.room.echoaround(this, this.name + " flips a page, reading intently.");		
	}
	else {         //Invalid command
		this.send("Valid commands for use with the journal are: (n)ext, (b)ack, (r)eread, (c)lose, or a numeric value.");
			return;
	}
	if ( currPage > lastPage || currPage <= 0 ) {
		this.send(this.bold(constants.CL_OFF) + "Invalid page number. You are currently on page (" + this.quest('savedJournalPage') + "/" + lastPage + ")" + this.normal(constants.CL_OFF));//The directed page is not within realms of the page limit
			return;
	}
	this.qval('savedJournalPage', currPage);
	//this.room.echo("Last journal page: " + this.quest('savedJournalPage'));
	var firstEntryNumOnPage = (currPage*2)-1;
	var lastEntryNumOnPage = firstEntryNumOnPage+1;
	var pagingPrompt = "[(N)ext turns the page, (b)ack turns back a page, (r)eread shows the page again,\n(c)lose leaves the journal, or enter a number to turn to a specific page (" + currPage + "/" + lastPage + ") ]";
	var entry = fread(this.id + ".totalJournalEntries(" + firstEntryNumOnPage + ")");
	entry = entry.split(";");
	this.send(BUFFER);
	for ( var i = 0; i < entry.length; i++ ) {
		this.send(entry[i]);
	}
	if ( entryTotal >= lastEntryNumOnPage ) {
		this.send(BUFFER);
		entry = fread(this.id + ".totalJournalEntries(" + lastEntryNumOnPage + ")");
		entry = entry.split(";");
		for ( var i = 0; i < entry.length; i++ )
			this.send(entry[i]);
	
	}
	this.send(BUFFER);
	this.send("\n" + pagingPrompt);
		return;
}
