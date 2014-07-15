//Alder January 2010
//Do not alter  
        JSCharacter.prototype.dispJournal = function(cmd) {
				var journal = this.journal;
				getCharCols(this,constants.CL_OFF);
                var scl = red+bld;
                var nl = "\n";
                var tab = "\t";
                // this.send(scl+"-----Top of dispJournal-----"+nrm);
                // this.send(scl+"Current chapter: "+journal.chapterPage[0]+nrm);
                // this.send(scl+"CMD is a number: "+isNumber(cmd)+nrm);
                // this.send("CMD: '"+cmd+"'");
                var arrayChapters = [];//Holds the arrays of entries per chapter
                arrayChapters.push("placeHolder");//Now all entry array elements will equal their chapter number
                /*xxxEntries is array of entry objects per status*/
                var newEntries = [];//1
                var progressEntries = [];//2
                var readyEntries = [];//3
                var completedEntries = [];//4
                var allEntries = [];//5
				var achievements = [];//7
				for each ( var ach in global.vAchievements ) {
					if ( this.isAchUnlocked(ach.achVnum) || !ach.achHide ) {
						achievements.push(ach);
					}
				}
				var playerTitles = this.titles;//6
                arrayChapters.push(newEntries);//1
                arrayChapters.push(progressEntries);//2
                arrayChapters.push(readyEntries);//3
                arrayChapters.push(completedEntries);//4
                arrayChapters.push(allEntries);//5
				arrayChapters.push(playerTitles);//6
				arrayChapters.push(achievements);//7
                /**CREATE ARRAY IDENTIFIERS FOR FUNCTION**/
                for ( var i = 1; i < arrayChapters.length; i++ ) {
                        var array = arrayChapters[i];
                        array.unshift(i);
                }
                /***COUNT NUMBER OF ENTRIES WITH EACH QUEST STATUS AND PLACE THEM IN ARRAYS***/
                for ( var i = 0; i < journal.entries.length; i++ ) {
                        var entry = journal.entries[i];
                        var quest = entry.quest;
                        var qName = quest.questName;
                        /**ADD TO COMPLETED ENTRIES**/
                        if ( this.quest(qName) == -1 ) {
                                completedEntries.push(entry);
                                allEntries.push(entry);
                        }
                        else if ( this.quest(qName) > 0 ) {
                                var taskData = this.questTaskProgress(qName);
                                var tasksCompleted = taskData[0];//Boolean--Are all tasks complete?
                                var taskStarted = taskData[taskData.length-1];//Boolean--Is any task started?
                                /**ADD TO READY ENTRIES**/
                                if ( tasksCompleted == true ) {
                                        readyEntries.push(entry);
                                        allEntries.push(entry);
                                }
                                /**ADD TO PROGRESS ENTRIES**/
                                else if ( taskStarted == true ) {
                                        progressEntries.push(entry);
                                        allEntries.push(entry);
                                }
                                /**ADD TO NEW ENTRIES**/
                                else if ( taskStarted == false ) {
                                        newEntries.push(entry);
                                        allEntries.push(entry);
                                }
                        }
                }
				/**CMD CHECK: "CLOSE"**/
                if ( strn_cmp(cmd,"CLOSE JOURNAL",1) == true ) {
					this.detach(13214);
					if ( journal.chapterPage[0] != 0 && journal.chapterPage[1] != 0 && journal.chapterPage[0] < 5 ) {
						// this.send("Setting [2] to "+arrayChapters[journal.chapterPage[0]][journal.chapterPage[1]].questName);
						// The following line stores the current chapter and page in a third index of journal.chapterPage
						journal.chapterPage[2] = arrayChapters[journal.chapterPage[0]][journal.chapterPage[1]].quest.questName;
					}
					this.send("You snap closed a leather-bound journal.");
					act("$n closes a leather-bound journal with a loud snap.",true,this,null,null,constants.TO_ROOM);
					//Monitoring (Alder)
					var alder = global.alder;
					if ( alder ) {
						getCharCols(alder);
						alder.send(grn+"[ "+this.name+" closing journal. ]"+nrm);
					}
					//*****************
					return;
                }
				/**CMD CHECK: "REREAD"**/
                if ( strn_cmp(cmd,"REREAD",1) == true ) {
					// this.send("Reread");
					// this.send(journal.chapterPage[2]);
					if ( journal.chapterPage[2] != undefined && journal.chapterPage[2].length > 0 ) {
						for ( var i = 1; i < arrayChapters.length; i++ ) {
							var check = false;
							// this.send("Top of chapters loop");
							var chapter = arrayChapters[i];
							if ( chapter != undefined ) {
								for ( var j = 0; j < chapter.length; j++ ) {
									var entry = chapter[j];
									if ( entry.quest && !str_cmp(entry.quest.questName,journal.chapterPage[2]) ) {
										// this.send("Should be setting new chapter and page");
										journal.chapterPage[0] = i;
										journal.chapterPage[1] = j;
										var check = true;
										break;
									}
								}
							}
							if ( check == true ) {
								break;
							}
						}
					}	
					this.send(this.chapterCompile(arrayChapters,journal.chapterPage[0],journal.chapterPage[1]));
					return;
                }
                var targetPage = null;
                var targetChapter;
                /*********************************
                 *    Table of Contents Tier     *
                 *********************************/
                 //Valid commands: close, reread, chapter number
                if ( journal.chapterPage[0] == 0 ) {
                        targetPage = 0;
                        if ( isNumber(cmd) == true ) {
                                // this.send("Command is a number");
                                cmd = Number(cmd);
                                if ( cmd < 1 || cmd > 7 )
                                        targetChapter = -1;//We want the TOC screen to redisplay with an "invalid number" message
                                else
                                        targetChapter = cmd;
                        }
                        /**AT THIS POINT, THE CMD IS SIMPLY INVALID**/
                        else {
                                targetChapter = "invalid";
                        }
                }//END: Table of Contents Tier
                /***********************
                 *    Chapter Tier     *
                 ***********************/               
                else if ( journal.chapterPage[0] > 0 && journal.chapterPage[0] < 8 ) {
                        var entriesArray = arrayChapters[journal.chapterPage[0]];//Array of entries for current chapter
                        targetChapter = journal.chapterPage[0];
                        // this.send("Current chapter: "+journal.chapterPage[0]);
                        // this.send("Page in chapter: "+journal.chapterPage[1]);
                        if ( strn_cmp(cmd,"TABLE OF CONTENTS",1) == true ) {
                                this.send(this.chapterCompile(arrayChapters,0,0));
                                return;
                        }
                        /**COMMANDS BELOW WORK WHILE IN ENTRIES**/
                        if ( journal.chapterPage[1] > 0 && journal.chapterPage[1] <= (entriesArray.length-1) ) {
                                //Valid cmds: entry num, reread, TOC, close, next, previous, entry list, brag(conditional)
                                if ( strn_cmp(cmd,"NEXT",1) == true ) {
                                        targetPage = journal.chapterPage[1] + 1;
                                }
                                else if ( strn_cmp(cmd,"PREVIOUS",1) == true ) {
                                        targetPage = journal.chapterPage[1] - 1;
                                }
                                else if ( strn_cmp(cmd,"ENTRIES LIST",1) == true ) {
                                        targetPage = 0;
                                }
								else if ( strn_cmp(cmd,"DESELECT",1) == true && journal.chapterPage[0] == 7 ) {
									this.remSpecTitle();
									targetPage = journal.chapterPage[1];
								}
								else if ( strn_cmp(cmd,"BRAG",1) == true && journal.chapterPage[0] == 7 && this.isAchUnlocked( entriesArray[journal.chapterPage[1]].achVnum ) ) {
									var ach = entriesArray[journal.chapterPage[1]];
									act(bld+grn+"You brag about your achievement: "+ach.achName+"!"+nrm,true,this,null,null,constants.TO_CHAR);
									act(bld+grn+"$n brags about $s achievement: "+ach.achName+"!"+nrm,true,this,null,null,constants.TO_ROOM);
									targetPage = journal.chapterPage[1];
								}
								else if ( isNumber(cmd) == true ) {     
                                        // this.send("Command is a number");
                                        cmd = Number(cmd);
										if ( journal.chapterPage[0] == 7 ) {
											var titles = this.getTitlesByAchievement(entriesArray[journal.chapterPage[1]].achVnum);
											if ( cmd > 0 && cmd <= titles.length ) {
												getCharCols(this,constants.CL_OFF);
												var title = titles[cmd-1];
												var sTitle = entriesArray[cmd];
												if ( title.title == this.getSpecTitle().title ) {
													this.remSpecTitle();
													targetPage = journal.chapterPage[1];
												}
												else {
													this.setSpecTitle(title);
													targetPage = journal.chapterPage[1];
												}
											}
											else {
												targetPage = -1;
											}
										}
										else {
											if ( cmd <= 0 || cmd >= entriesArray.length )
													targetPage = -1;//We want to just display last viewed page with a "bad number" msg
											else
													targetPage = cmd;
										}
                                }
                                /**AT THIS POINT, THE CMD IS SIMPLY INVALID**/
                                else {
                                        targetPage = "invalid";
                                }
                        }
                        else {
                                if ( strn_cmp(cmd,"TABLE OF CONTENTS",1) == true ) {
                                        targetChapter = 0;//Set chapter to TOC
                                        targetPage = 0;
                                }
                                if ( strn_cmp(cmd,"NEXT",1) == true  && journal.chapterPage[0] != 6 ) {
                                        targetPage = 1;
                                }
                                else if ( strn_cmp(cmd,"PREVIOUS",1) == true  && journal.chapterPage[0] != 6 ) {
                                        targetPage = entriesArray.length - 1;
                                }
								else if ( strn_cmp(cmd,"DESELECT",1) == true && journal.chapterPage[0] == 6 ) {
										this.remSpecTitle();
										targetPage = 0;
								}
                                else if ( isNumber(cmd) == true ) {     
                                        // this.send("Command is a number");
										cmd = parseInt(cmd);
										if ( cmd <= 0 || cmd >= entriesArray.length )
											targetPage = -1;//We want to just display last viewed page with a "bad number" msg
										else {
											if ( journal.chapterPage[0] == 6 ) {
												var sTitle = entriesArray[cmd];
												if ( sTitle.title == this.getSpecTitle().title ) {
													this.remSpecTitle();
												}
												else {
													this.setSpecTitle(playerTitles[cmd]);
												}
												targetPage = journal.chapterPage[1];
											}
											else {
												targetPage = Number(cmd);
											}
										}
                                }
                                /**AT THIS POINT, THE CMD IS SIMPLY INVALID**/
                                else {
                                        targetPage = "invalid";
                                }
                        }
                }//END: Chapter Tier
                this.send(this.chapterCompile(arrayChapters,targetChapter,targetPage));
                // this.send(grn+bld+"\nCurrent chapter: "+journal.chapterPage[0]);
                // this.send(grn+bld+"Current page: "+journal.chapterPage[1]);
                // this.send(scl+nl+"-----Bottom of dispJournal-----"+nrm);
        }//END OF SCRIPT
		
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		
/*******************************************
 *        Creates Chapter Display          *
 *******************************************/
JSCharacter.prototype.chapterCompile = function(arrayChapters,intTargetChapter,intTargetPage) {
        // this.send("Target chapter: "+intTargetChapter);
        // this.send("Target page: "+intTargetPage);
        var intUsageMsg;
        var journal = this.journal;
		// this.send("targetChapter: "+intTargetChapter);
		// this.send("targetPage: "+intTargetPage);
		if ( intTargetChapter == 0 || intTargetPage == 0 ) {
			// this.send("Setting [2] to undefined");
			journal.chapterPage[2] = undefined;
		}
        /**USE USAGE MSG 2 IF INVALID NUMBER SELECTED**/
        if ( intTargetChapter < 0 || intTargetChapter > 7 || intTargetPage < 0 ) {
                // this.send("That number is invalid");
                intUsageMsg = 2;
                intTargetChapter = journal.chapterPage[0];
                intTargetPage = journal.chapterPage[1];
        }
        /**USE USAGE MSG 3 IF CMD IS INVALID**/
        else if ( getObjectClass(intTargetChapter) == "String" || getObjectClass(intTargetPage) == "String" ) {
                // this.send("That is not a valid command.");
                intUsageMsg = 3;
                intTargetChapter = journal.chapterPage[0];
                intTargetPage = journal.chapterPage[1];
        }
        /**RETURN TO USE USAGE MSG 1 IF SELECTED CHAPTER IS EMPTY**/
        if ( arrayChapters[intTargetChapter].length == 1 ) {
                // this.send("Chapter "+intTargetChapter+": empty");
                intUsageMsg = 1;
                intTargetChapter = journal.chapterPage[0];
                intTargetPage = journal.chapterPage[1];
        }
        var entriesArray = arrayChapters[intTargetChapter];
        if ( arrayChapters[journal.chapterPage[0]].length == 1 ) {
                intTargetChapter = 0;
                intTargetPage = 0;
                intUsageMsg = 0;
        }
        /*COLOR SHORTCUTS*/
        var cyn = this.cyan(constants.CL_OFF);
        var yel = this.yellow(constants.CL_OFF);
        var grn = this.green(constants.CL_OFF);
        var red = this.red(constants.CL_OFF);
        var blu = this.blue(constants.CL_OFF);
        var mag = this.magenta(constants.CL_OFF);
        var bld = this.bold(constants.CL_OFF);
        var nrm = this.normal(constants.CL_OFF);
        var usageMsg = "";//1=Empty chapter/2=Invalid number/3=Invalid cmd
        switch(Number(intUsageMsg)) {
                case 1:
                        usageMsg = cyn+"\n\nThe chapter you selected is empty."+nrm;
                        break;
                case 2:
                        usageMsg = cyn+"\n\nYou must select a number that is within the given range."+nrm;
                        break;
                case 3:
                        usageMsg = cyn+"\n\nYour command was invalid. Any portion of the above commands will work.\nEX: 'r' or 'rer' for the 'Reread' command."+nrm;
                        break;
        }
        /*DISPLAY SHORTCUTS*/
        var header = "";
        var body = "";
        var footer = "";
        var pageLimit = entriesArray.length - 1;
        var footReread = "["+grn+"R"+nrm+"] Reread";
        var footClose = "["+grn+"C"+nrm+"] Close Journal";
        var footSelChap = "["+grn+"#"+nrm+"] Select Chapter ("+grn+"1-7"+nrm+")";
        var footSelPage1 = "["+grn+"#"+nrm+"] Select Page ("+grn+"1-"+pageLimit+nrm+")";//Used with chapter entry list
        var footNext = "["+grn+"N"+nrm+"] Next Page";
        var footPrev = "["+grn+"P"+nrm+"] Previous Page";
        var footTOC = "["+grn+"T"+nrm+"] Table of Contents";
        var footEntList = "["+grn+"E"+nrm+"] Entry List";
		var footBrag = "["+grn+"B"+nrm+"] Brag";
        var LINE = "____________________________________________________________________________";//"_"*76
        var nl = "\n";
        var tab = "\t";
        var scl = red+bld;
        /****COMPILE TABLE OF CONTENTS****/
        if ( intTargetChapter == 0 ) {
                // this.send("Compiling TOC");
                var newEntries = arrayChapters[1];
                var progressEntries = arrayChapters[2];
                var readyEntries = arrayChapters[3];
                var completedEntries = arrayChapters[4];
                var allEntries = arrayChapters[5];
				var achievements = arrayChapters[7];
				var playerTitles = arrayChapters[6];
                header = ("\n" +bld+" Table of Contents\n"+nrm+LINE+"\n\n");
                body = (grn+"1"+nrm+") "+strPadding("New Quests",".",38,"right")+"{ "+cyn+(newEntries.length-1)+nrm+" } \n\n"+
				        grn+"2"+nrm+") "+strPadding("Quests In Progress",".",38,"right")+"{ "+cyn+(progressEntries.length-1)+nrm+" }\n\n"+
						grn+"3"+nrm+") "+strPadding("Quests Ready to Turn In",".",38,"right")+"{ "+cyn+(readyEntries.length-1)+nrm+" }\n\n"+
						grn+"4"+nrm+") "+strPadding("Completed Quests",".",38,"right")+"{ "+cyn+(completedEntries.length-1)+nrm+" }\n\n"+
						grn+"5"+nrm+") "+strPadding("All Quests",".",38,"right")+"{ "+cyn+(allEntries.length-1)+nrm+" }\n\n"+
						grn+"6"+nrm+") "+strPadding("All Available Titles",".",38,"right")+"{ "+cyn+(playerTitles.length-1)+nrm+" }\n\n"+
						grn+"7"+nrm+") "+strPadding("Achievements",".",38,"right")+"{ "+cyn+(this.getUnlockedAchievements().length)+" / "+(achievements.length-1)+nrm+" }");
						
				footer = nl+strPadding("\n","-",78,"right")+"\n"+footSelChap+tab+footReread+tab+footClose+usageMsg;
                /**SINCE TOC HAS ACTUALLY COMPILED, THEN SET THAT AS JOURNAL'S CHAPTER**/
                journal.chapterPage[0] = intTargetChapter;//this puts the this in the TOC Tier
                journal.chapterPage[1] = 0;
        }
        /****COMPILE CHAPTER****/
        else {                  /*Trying to open Chapters 1-6*/
                // this.send("Since target chapter was not 0, you have selected a chapter");
                if ( entriesArray.length > 1 ) {
                        // this.send("Chapter "+intTargetChapter+": has entries");
                        /**DETERMINE CHAPTER TITLE**/
                        var chapterTitle = "";
                        switch(intTargetChapter) {
                                case 1:
                                        chapterTitle = " New Quests";
                                        break;
                                case 2:
                                        chapterTitle = " Quests In Progress";
                                        break;
                                case 3:
                                        chapterTitle = " Quests Ready to Turn In";
                                        break;
                                case 4:
                                        chapterTitle = " Completed Quests";
                                        break;
                                case 5:
                                        chapterTitle = " All Quests";
                                        break;
								case 6:
                                        chapterTitle = " All Available Titles";
                                        break;
								case 7:
										chapterTitle = " Achievements - "+(this.getUnlockedAchievements().length)+" / "+pageLimit+" Unlocked";
                                        break;
                        }
                        // this.send("Displaying chapter entry list: true");
                        header = "\n" +bld+chapterTitle+nrm+"\n"+LINE+"\n";//Header never changes
                        /**DISPLAY FOR CHAPTER ENTRY LIST**/
                        if ( intTargetPage == 0 || intTargetPage == (pageLimit+1) ) {
                                var qNameArray = [];                                    //Those parameters can only be reached with n/p/r
                                if ( pageLimit < 10 )
                                        padVal = 2;
                                else if ( pageLimit < 100 )
                                        padVal = 3;
                                else
                                        padVal = 4;
                                if ( intTargetChapter < 6 ) {
									for ( var i = 1; i < entriesArray.length; i++ ) {
										var qName = entriesArray[i].quest.questName;
										var mobName = entriesArray[i].questOwnerName;//(this.quest(qName+"_GIVER") != 0 ? "Given by "+getMobName(this.quest(qName+"_GIVER")) : '');
										qNameArray.push("\n"+strPadding(grn+i+nrm," ",padVal,"left")+") "+strPadding(qName," ",40,"right")+yel+mobName+nrm);
									}
									body = qNameArray.join("");
								}
								// Achievements
								else if ( intTargetChapter == 7 ) {
									getCharCols(this,constants.CL_OFF);
									for ( var i = 1; i < entriesArray.length; i++ ) {
										var ach = entriesArray[i];
										// this.send(ach.achName);
										body += "\n"+strPadding(grn+i+nrm," ",padVal,"left")+") "+( !this.isAchUnlocked(ach.achVnum) ? red : grn+bld )+strPadding(ach.achName," ",25,"right")+nrm;
										var titles = this.titles;
										if ( titles.length > 0 ) {
											var titleList = this.getTitlesByAchievement(ach.achVnum);
											if ( titleList.length > 0 ) {
												var plur = (titleList.length > 1 ? "s" : "");
												body += titleList.length+" title"+plur+" available"+nrm;
											}
										}
									}
								}
								else if ( intTargetChapter == 6 ) {
									getCharCols(this,constants.CL_OFF);
									for ( var i = 1; i < entriesArray.length; i++ ) {
										body += "\n"+strPadding(grn+i+nrm," ",padVal,"left")+") "+(entriesArray[i].title == this.getSpecTitle().title ? cyn+bld : nrm)+strPadding(entriesArray[i].title," ",25,"right")+nrm+"("+entriesArray[i].type+")";
									}
								}
								var footPP = footPrev+" ("+grn+pageLimit+nrm+")";
								var footNP = footNext+" ("+grn+1+nrm+")";
								if ( intTargetChapter != 6 ) {
									footer = nl+(strPadding("\n","-",78,"right")+"\n"+footNP+tab+footSelPage1+tab+footClose+nl+footPP+tab+footTOC+tab+footReread)+usageMsg;
                                }
								else {
									footer = nl+strPadding("\n","-",78,"right")+"\n"+"["+grn+"#"+nrm+"] Select Title ("+grn+( pageLimit > 0 ? 1 : 0 )+"-"+pageLimit+nrm+")"+tab+footReread+tab+footClose+nl+"["+grn+"D"+nrm+"] Deselect Title"+tab+footTOC+usageMsg;
								}
								/**SET JOURNAL CHAPTER TO THIS CHAPTER NUMBER, AND PAGE TO ZERO FOR ENTRY LIST SCREEN**/
                                journal.chapterPage[0] = intTargetChapter;
                                journal.chapterPage[1] = 0;
                        }//END: Chapter Entry List
                        /**SPECIFIC ENTRY**/
                        else if ( intTargetPage > 0 && intTargetPage <= pageLimit ) {
                                if ( intTargetChapter < 6 ) {
									var quest = entriesArray[intTargetPage].quest;
									body = nl+bld+cyn+intTargetPage+nrm+entriesArray[intTargetPage].compile(this,quest);
									var footSelPage2 = "\n["+grn+"#"+nrm+"] Select Page ("+grn+intTargetPage+"/"+pageLimit+nrm+")";//Used with specific entries
								}
								else if ( intTargetChapter == 7 ) {
									var ach = entriesArray[intTargetPage];
									body += nl+( !this.isAchUnlocked(ach.achVnum) ? red : grn )+ach.achName+nl+nrm+(ach.achDesc != undefined ? nl+ach.achDesc : "No description");
									var titleList = this.getTitlesByAchievement(ach.achVnum);
									if ( titleList.length > 0 ) {
										body += cyn+"\n\nAvailable titles"+nrm+": "+nrm+"\n";
										var st = "   ";
										for ( var i = 0; i < titleList.length; i++ ) {
											body += "\n"+st+grn+(i+1)+nrm+") "+(titleList[i].title == this.getSpecTitle().title ? bld+cyn : nrm)+strPadding(titleList[i].title," ",20,"right")+nrm+"("+titleList[i].type+")";
										}
									}
									var footSelPage2 = (titleList.length > 0 ? "\n["+grn+"#"+nrm+"] Select Title ("+grn+( titleList.length > 0 ? 1 : 0 )+"-"+titleList.length+nrm+")"+tab+"["+grn+"D"+nrm+"] Deselect Title" : "");
								}
                                footer = nl+(strPadding("\n","-",78,"right")+"\n"+footNext+tab+tab+footEntList+tab+tab+footClose+nl+footPrev+tab+footTOC+tab+footReread+footSelPage2+(ach != undefined && this.isAchUnlocked(ach.achVnum) ? (footSelPage2.length ? tab+footBrag : "\n"+footBrag) : ""))+usageMsg;
                                /**SET JOURNAL CHAPTER TO THIS CHAPTER NUMBER, AND PAGE TO THIS ENTRY PAGE**/
                                journal.chapterPage[0] = intTargetChapter;
                                journal.chapterPage[1] = intTargetPage;
                        }//END: Specific entry
                }//END: compile cChapter Tier(chapter title or specific entry)
        }
        //  this.send("Returning ( header + body + footer )");
        return ( header + body + footer );
	}



