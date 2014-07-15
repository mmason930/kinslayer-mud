	//Alder January 2010
	//Do not alter
	/************************************************************************
	 *  Recreate journal with entries for actor after loss to reboot/crash  *
	 ************************************************************************/
	 JSCharacter.prototype.recreateJournal = function() {
		if ( !this.journal ) {
			// this.send("RECREATING JOURNAL");
			this.CreateQuestJournal();//Make actor's journal object
			for ( var i = 0; i < global.vQuests.length; i++ ) {
				var name = global.vQuests[i].questName;
				if ( this.quest(name) != 0 ) {
					this.journalEdit("ADD",name,false);
				}
			}
		}
		return;
	}

