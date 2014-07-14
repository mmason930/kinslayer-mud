//Alder
//May 2010
/*******************************************************************
 * strTitle: the title (String)                                    *
 * strType: type of title, normal,suffix,prefix (String)           *
 * strCat: category title falls under (E.g., pk or clans           *
 * achVnum: vnum of achievement unlocking the title (optional int) *
 *******************************************************************/
JSCharacter.prototype.unlockTitle = function(strTitle, strType, strCat, achVnum) {
	var playerTitles = this.titles;
	var title = new Object();
	title.title = strTitle;
	title.type = strType.toLowerCase();
	var type = title.type;
	if ( type != "suffix" && type != "prefix" ) {
		title.type = "normal";
	}
	title.category = strCat.toLowerCase();
	var cat = title.category;
	if ( !strn_cmp(cat,"arena",1) && !strn_cmp(cat,"pk",1) && !strn_cmp(cat,"clans",1) && !strn_cmp(cat,"quests",1) && !strn_cmp(cat,"trades",1) && !strn_cmp(cat,"silly",1) && !strn_cmp(cat,"imm",1) ) {
		title.category = "quests";
	}
	title.achVnum = (achVnum != undefined ? achVnum : 0);
	
	function blockDupe(element, index, array) {
		if ( !str_cmp(element.title,strTitle) ) {
			return element;
		}
	}
	
	if ( !playerTitles.some(blockDupe) ) {
		sqlQuery("DELETE FROM userTitle WHERE user_id="+this.id+" && title=\""+sqlEsc(title.title)+"\" && type='"+sqlEsc(title.type)+"' && achievement_vnum='"+sqlEsc(title.achVnum)+"';");
		sqlQuery("INSERT INTO userTitle (`user_id`,`title`,`type`,`category`,`achievement_vnum`,`selected`) VALUES('"+this.id+"','"+sqlEsc(title.title)+"','"+sqlEsc(title.type)+"','"+sqlEsc(title.category)+"','"+sqlEsc(title.achVnum)+"','"+sqlEsc(0)+"');");
		accessTitles(this.id).push(title);
		var cVal = this.quest("HIDE_ACH_TITLE_MSG");
		var numLinkedTitles = ( title.achVnum != 0 ? this.getTitlesByAchievement(achVnum).length : 0 );
		if ( !cVal ) {
			getCharCols(this,constants.CL_OFF);
			this.send("You have earned the "+nrm+cyn+strTitle+nrm+" "+(title.type != "normal" ? title.type : "title")+"!"+nrm);
			mudLog(3, Math.max(103,this.invis),this.name+" has earned "+strType+" title: "+strTitle+".");
			if ( !this.countJS(625) ) {
				function quick(args) {
					var actor = args[0];
					getCharCols(actor);
					actor.send("Type "+cyn+"quick select"+nrm+" to select this title right now.");
				}
				setTimeout(1,quick,[this]);
				function afterWait(arg) {
					var actor = arg[0];
					if ( actor.isValid ) {
						actor.detach(625);
					}
				}
				this.attach(625);
				setTimeout(420,afterWait,[this]);
			}
		}
	}
}
	


