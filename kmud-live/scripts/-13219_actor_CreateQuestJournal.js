//Alder January 2010
//Do not alter
    /**********************************
	 * Create actor's virtual journal * 
	 **********************************/
	if( !globJournals ) {
		var globJournals = [];//Array that holds every journal object
	}
	JSCharacter.prototype.CreateQuestJournal = function() {
		if ( this.journal ) {
			// this.room.echo("You are already linked to a virtual journal object "+this.journal.ownerName+".");
		}
		else {
			var newJournal = new JournalObj(this);
			globJournals.push(newJournal);
			// this.room.echo("Your virtual journal object has been created " + newJournal.ownerName +".");
		}
	}
/*********************************************
 *   Create new object of class JournalObj   *
 *********************************************/
 function JournalObj(actor) {
	this.ownerID = actor.id;//Journal can easily be found in vJournals array using owner's unique ID
	this.ownerName = actor.name;//Name of owner
	this.entries = [];//Array that will hold entry objects
	this.chapterPage = [0,0,undefined];//Array: [chapter,pageInChapter]
}
	
