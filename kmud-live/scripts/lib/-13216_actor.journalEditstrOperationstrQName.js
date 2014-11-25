//Alder January 2010
//Do not alter
    /********************************************** 
	 * Edit entry array for a character's journal *
	 * strOperation: "ADD", "REMOVE"              *
	 * strQName: Name of quest to add or remove   *
	 * boolShowMessage: show messages or not(opt) *
	 **********************************************/
	JSCharacter.prototype.journalEdit = function(strOperation, strQName, boolShowMessage) {
		/**CHECK FOR VALID FUNCTION INPUTS**/
		var add = strn_cmp(strOperation,"ADD",1);
		var remove = strn_cmp(strOperation,"REMOVE",1);
		if ( !add && !remove ) {
			// mudLog(3,103,"FLAGGED: Invalid strOperation passed to JSCharacter.entryEditor(strOperation,strQName)");
				return;
		}
		// this.send("ARG CHECKS: add? - "+add+" | remove? - "+remove)
		var quest = getQuestByName(strQName);
		if ( quest == null ) {
			mudLog(3,100,"(-13216) FLAGGED: Entry trying to be edited for non-existent quest: "+strQName+". Actor: "+this.name+". Room: "+this.room.vnum+". Cancelling entry creation and informing actor.");
			this.send(this.green(constants.CL_OFF)+"There has been an error in editing the quest: '"+strQName+"'. Speak with an immortal to resolve the issue."+this.normal(constants.CL_OFF));
			return;
		}
		var journal = this.journal;//Character's virtual journal object
		if ( !journal ) {
			this.recreateJournal();
			// this.send("You do not have a virtual journal, "+this.name+".");
			return;
		}
		var actor = this;
		/**LOOK FOR EXISTING ENTRY**/
		var targetEntry = false;
		for (var _autoKey in journal.entries) {
			var entry = journal.entries[_autoKey];
			if ( !str_cmp(entry.quest.name,strQName) ) {
				targetEntry = entry;//This is the entry object we are looking for
				// this.send("EXISTING ENTRY FOUND: "+targetEntry.name);
					break;
			}
		}
		/***********
		 *ADD ENTRY*
		 ***********/
		if ( add && !targetEntry ) {
			// this.send("NO ENTRY EXISTS WITH QUEST NAME: "+strQName);
			var jEntry = new EntryObj(actor,quest);
			journal.entries.push(jEntry);//This new entry is now part of the journal's entry list
			if ( this.quest(strQName) == 0 ) {
				this.qval(strQName,1);//Start actor on quest by qvalling
			}
			//this.send("Your qval for "+strQName+" is now "+this.quest(strQName));
			/**SEND JOURNAL UPDATE MESSAGE TO ACTOR**/
			if ( boolShowMessage != false ) {
				var status = "New Quest";
				this.send( this.yellow(constants.CL_OFF) + this.bold(constants.CL_OFF) + "Journal Updated -- Quest: '"+strQName+"' -- "+this.cyan(constants.CL_OFF)+status+this.normal(constants.CL_OFF) );
				this.comm("save");
			}
		}
		/**************
		 *REMOVE ENTRY*
		 **************/	
		else if ( targetEntry && remove ) {
			// var targetEntryPage = Math.ceil(targetEntry.entryNum/2);
			if ( journal.entries.length > 1 ) {
				var reconArray = [];//Array of entries excluding entry to be removed
				var marker = targetEntry.entryNum - 1;//Element index in journal.entries for entry to be removed
				/**SAVE ENTRIES BEFORE AND AFTER TARGET ENTRY**/
				for ( var i = 0; i < marker; i++ ) {
					reconArray.push(journal.entries[i]);
				}
				for ( var i = (marker + 1); i < journal.entries.length; i++ ) {
					reconArray.push(journal.entries[i]);
				}
				/**RECONSTRUCT JOURNAL ARRAY WITH TARGET ENTRY DELETED**/
				journal.entries = reconArray;
				/**ADJUST ENTRY NUMS FOR ALL ENTRIES**/
				for ( var i = 0; i < journal.entries.length; i++ ) {
					var thisEntry = journal.entries[i];
					thisEntry.entryNum = i + 1;
					// this.send("READJUSTING ENTRIES: "+thisEntry.name+" to position "+thisEntry.entryNum);
				}
			}
			else {
				journal.entries.pop();
			}
			/**REMOVE ALL QVALS FOR QUEST UPON REMOVAL**/
			this.qval(strQName, 0);//Set qval to 'Not Started'
			var tasks = quest.tasks;
			// this.send(strQName+": "+this.quest(strQName));
			var exploreTag = checkTag(strQName,'Exploration');
			for ( var j = 0; j < tasks.length; j++ ) {
				var task = tasks[j];
				if ( exploreTag == true ) {
					actor.deletePval(strQName+"_"+task[0]);
				}
				if ( !task[1] || task[1] == '' || task[1] == 'null' || task[1] == 'undefined' || task[1].length == 0 ) {
					var taskQuest = strQName + "_" + task[0];
					this.qval(taskQuest,0);//Qval for currently examined task in myQuest
					// this.send(taskQuest+": "+this.quest(taskQuest));
				}
			}
			if ( boolShowMessage != false )
				this.send( this.yellow(constants.CL_OFF) + this.bold(constants.CL_OFF) + "Journal Updated -- Quest: '"+strQName+"'"+" -- "+this.cyan(constants.CL_OFF)+"Abandoned"+this.normal(constants.CL_OFF) );
			this.comm("save");
		}
	// this.send("Your journal has " + actor.journal.entries.length + " entries.");
	return;
	}

