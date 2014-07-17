var script40 = function(self, actor, here, args, extra) {
		_block;
		var vArgs = getArgList(args);
		var cmd = vArgs[1];
		if ( strn_cmp(cmd,"colors",4) ) {
			/** Displays All Colors **/
			getCharCols(actor);
			actor.send(grn+"Green"+nrm);
			actor.send(blu+"Blue"+nrm);
			actor.send(red+"Red"+nrm);
			actor.send(yel+"Yellow"+nrm);
			actor.send(cyn+"Cyan"+nrm);
			actor.send(mag+"Magenta"+nrm);
			actor.send(grn+"Green"+nrm);
			actor.send(grn+bld+"Bold Green"+nrm);
			actor.send(blu+bld+"Bold Blue"+nrm);
			actor.send(red+bld+"Bold Red"+nrm);
			actor.send(yel+bld+"Bold Yellow"+nrm);
			actor.send(cyn+bld+"Bold Cyan"+nrm);
			actor.send(mag+bld+"Bold Magenta"+nrm);
			actor.send(grn+bld+"Bold Green"+nrm);
			return;
		}
		var vQuests = global.vQuests;
		if ( cmd == "CRASH" ) {
			globJournals = [];/*Simulate crash/reboot*/here.echo("Simulating crash...globJournals cleared");
			return;
		}
		else {
			// actor.CreateQuestJournal();
			// actor.journal.entries = [];
			actor.recreateJournal();
			fwrite(actor.id+'entryNames1',"");
			here.echo(actor.name+" has a journal: "+actor.journal);
			if ( vArgs[2] ) {
				here.echo("Entries "+cmd+"ed.");
				if ( vArgs[2] == "all" ) {
					for each ( var quest in vQuests ) {
						actor.journalEdit(cmd,quest.questName,false);
					}
				}
				else if ( isNumber(vArgs[2]) == true ) {
					for ( var i = 0; i <= (vArgs[2]-1) && i < vQuests.length; i++ ) {
						actor.journalEdit(cmd,vQuests[i].questName,false);
					}
				}
				else {
					var qName = [];
					for ( var i = 2; i < vArgs.length; i++ ) {
						qName.push( vArgs[i] );
					}
					qName = qName.join(" ");
					for each ( var quest in vQuests ) {
						if ( !str_cmp(quest.questName,qName) ) {
							actor.journalEdit(cmd,quest.questName,false);
							break;
						}
					}
				}
			}
		}
		here.echo(actor.cyan(constants.CL_OFF)+"Your entries:"+actor.normal(constants.CL_OFF));
		if ( actor.journal ) {
			for each ( var entry in actor.journal.entries ) {
				here.echo(entry.questName);
			}
			here.echo(actor.cyan(constants.CL_OFF)+actor.journal.entries.length+" total entries."+actor.normal(constants.CL_OFF));
		}
}