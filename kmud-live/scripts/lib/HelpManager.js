function HelpManager()
{
	this.helpFiles = {};
	
	this.loadHelpFilesFromDatabase();
}

HelpManager.prototype.createWebSocketMessageForHelpFile = function(helpFile, response)
{
	if(helpFile.id != null)
	{
		parentHelpFile = global.helpManager.getHelpFileById(helpFile.parentId);
	}
			
	var childrenHelpFiles = global.helpManager.getHelpFilesByParentId(helpFile.id);
	var childrenHelpFilesForMessage = [];
			
	childrenHelpFiles.forEach(function(childHelpFile) {
		childrenHelpFilesForMessage.push({name: childHelpFile.name, helpFileId: childHelpFile.id});
	});
		
	response.name = helpFile.name;
	response.syntax = helpFile.syntax;
	response.keywords = helpFile.keywords;
	response.description = helpFile.description;
	response.children = childrenHelpFilesForMessage;
	response.created = helpFile.createdDatetime.strftime("%b %d %Y %H:%M:%S");
	response.lastModified = helpFile.lastModifiedDatetime.strftime("%b %d %Y %H:%M:%S");
	response.parent = (parentHelpFile == null) ? (null) : ({helpFileId: parentHelpFile.id, name: parentHelpFile.name});
	response.helpFileId = helpFile.id;
};

HelpManager.prototype.addHelpFileInternally = function(helpFile)
{
	if(this.helpFiles[ helpFile.id ])
		mudLog(constants.BRF, 100, "Attempting to save help file internally when it already exists. ID: " + helpFile);
	else
		this.helpFiles[ helpFile.id ] = helpFile;
}

HelpManager.prototype.deleteHelpFileFromDatabase = function(helpFile)
{
	var sql = " DELETE"
			+ "   helpFile.*"
			+ " FROM"
			+ "   helpFile"
			+ " WHERE id = " + helpFile.id;
			
	sqlQuery(sql);
}

HelpManager.prototype.loadHelpFilesFromDatabase = function()
{
	var sql = " SELECT *"
	        + " FROM helpFile"
		    + " ORDER BY id";
	
	var resultSet = sqlQuery(sql);
	
	while(resultSet.hasNextRow)
	{
		var row = resultSet.getRow;
		
		var helpFile = this.createHelpFile();
		helpFile.id = parseInt(row.get("id"));
		helpFile.name = row.get("name");
		helpFile.syntax = row.get("syntax");
		helpFile.description = row.get("description");
		helpFile.parentId = row.get("parent_id");
		helpFile.keywords = row.get("keywords");
		helpFile.createdByUserId = row.getInteger("created_by_user_id");
		helpFile.createdDatetime = row.getTimestamp("created_datetime");
		helpFile.lastModifiedByUserId = row.getInteger("last_modified_by_user_id");
		helpFile.lastModifiedDatetime = row.getTimestamp("last_modified_datetime");
		
		this.helpFiles[ helpFile.id ] = helpFile;
	}
}

HelpManager.prototype.createHelpFile = function(parentId)
{
	var helpFile =
	{
		id: null,
		name: "New Help File",
		parentId: parentId
	};
	
	if(helpFile.parentId == undefined)
		helpFile.parentId = null;
	
	helpFile.isNew = function() {
		return this.id == null || this.id == undefined;
	}
	
	return helpFile;
}

HelpManager.prototype.saveHelpFileToDatabase = function(helpFile)
{
	if(helpFile.isNew())
	{
		var sql = " INSERT INTO helpFile("
				+ "   `name`,"
				+ "   `syntax`,"
				+ "   `description`,"
				+ "   `parent_id`,"
				+ "   `keywords`,"
				+ "   `created_by_user_id`,"
				+ "   `created_datetime`,"
				+ "   `last_modified_by_user_id`,"
				+ "   `last_modified_datetime`"
				+ ") VALUES("
				+ sqlEscapeQuoteString(helpFile.name) + ","
				+ sqlEscapeQuoteString(helpFile.syntax) + ","
				+ sqlEscapeQuoteString(helpFile.description) + ","
				+ helpFile.parentId + ","
				+ sqlEscapeQuoteString(helpFile.keywords) + ","
				+ helpFile.createdByUserId + ","
				+ sqlEncodeQuoteDate(helpFile.createdDatetime) + ","
				+ helpFile.lastModifiedByUserId + ","
				+ sqlEncodeQuoteDate(helpFile.lastModifiedDatetime)
				+ ");";
				
		sqlQuery(sql);
		
		helpFile.id = sqlInsertID();
	}
	else
	{
		var sql = " UPDATE helpFile SET"
				+ "   name = " + sqlEscapeQuoteString(helpFile.name) + ","
				+ "   syntax = " + sqlEscapeQuoteString(helpFile.syntax) + ","
				+ "   description = " + sqlEscapeQuoteString(helpFile.description) + ","
				+ "   parent_id = " + helpFile.parentId + ","
				+ "   keywords = " + sqlEscapeQuoteString(helpFile.keywords) + ","
				+ "   created_by_user_id = " + helpFile.createdByUserId + ","
				+ "   created_datetime = " + sqlEncodeQuoteDate(helpFile.createdDatetime) + ","
				+ "   last_modified_by_user_id = " + helpFile.lastModifiedByUserId + ","
				+ "   last_modified_datetime = " + sqlEncodeQuoteDate(helpFile.lastModifiedDatetime)
				+ " WHERE id = " + helpFile.id;
		
		sqlQuery(sql);
	}
}

HelpManager.prototype.reload = function()
{
	delete global.helpManager;
	global.helpManager = new HelpManager();
}

HelpManager.prototype.getCurrentFile = function(actor)
{
	var id = getSval(actor, 9, "help");
	return this.getHelpFileById(id);
}

HelpManager.prototype.setCurrentFile = function(actor, id)
{
	setSval(actor, 9, "help", id);
}

HelpManager.prototype.getHelpFileById = function(id)
{
	return this.helpFiles[ id ];
}

HelpManager.prototype.getHelpFileByName = function(name)
{
	for (var _autoKey in this.helpFiles) {
		var helpFile = this.helpFiles[_autoKey];
		if(!str_cmp(name, helpFile.name))
			return helpFile;
	}
	
	return null;
}

HelpManager.prototype.getHelpFilesByParentId = function(id)
{
	var children = [];
	for (var _autoKey in this.helpFiles) {
		var helpFile = this.helpFiles[_autoKey];
		if(helpFile.parentId == id)
			children.push(helpFile);
	}
	return children.sort(function(a, b) {
		var aName = a.name.toLowerCase(),
			bName = b.name.toLowerCase();
		if (aName < bName)
			return -1;
		if (aName > bName)
			return 1;
		return 0;
	});
}

HelpManager.prototype.getHelpPage = function(actor, file, hideFooter)
{
		getCharCols(actor);
		var parent = null;
		var pID = null;
		if (file) {
			parent = this.getHelpFileById(file.parentId);
			if (parent)
				pID = parent.id;
		}
		var singleFile = false;
		actor.send(" ");
		actor.send("Kinslayer Newbie Guide - " + cyn + (file ? file.name : "Table of Contents") + nrm);
		actor.send("________________________________________________________________________________");
		actor.send(" ");
		if (this.getHelpFilesByParentId(pID).length == 0) { // File has no children, it's the lowest you can go
			singleFile = true;
		}
		if (!file) { // File doesn't exist, return table of contents
			var helpTopics = this.getHelpFilesByParentId(null);
			for (var i = 0; i < helpTopics.length; i++) {
				actor.send("["+cyn+(i+1)+nrm+"] "+helpTopics[i].name);
			}
		}
		else {
			if (file.syntax) { // File contains a command with syntax
				actor.send("Command: "+file.syntax);
				actor.send(" ");
			}
			if (file.description) { // File has a description
				var desc = eval(file.description);
				actor.send(desc);
				actor.send(" ");
			}
			if (singleFile == false) { // File has children
				var children = this.getHelpFilesByParentId(file.id);
				for (var i = 0; i < children.length; i++) {
					actor.send("["+cyn+(i+1)+nrm+"] "+children[i].name);
				}
			}
		}
		
		if(hideFooter == null || hideFooter === false) {
			actor.send("________________________________________________________________________________");
			actor.send(" ");
			actor.send((singleFile == false ? "["+cyn+"#"+nrm+"] Select Topic           " : "") + "["+cyn+"B"+nrm+"] Back"+ (parent ? " to "+parent.name : "")+"        ["+cyn+"C"+nrm+"] Close Guide");
		}
}

HelpManager.prototype.getSearchedFile = function(term)
{
	term = term.toLowerCase();
	for (var _autoKey in this.helpFiles) {
		var helpFile = this.helpFiles[_autoKey];	
		if(helpFile.keywords != null && helpFile.keywords.length > 0)
		{// File has keywords

		
			if(helpFile.keywords.toLowerCase().split(/,\s*/).indexOf(term) != -1)
				return helpFile;
		}
	}
	return null;
}

HelpManager.prototype.getWorldMap = function(actor, bool)
{
	getCharCols(actor);
	var count = 0;
			//            X Y   name      vnum
	var name          = [2,1,"   ","   ",-1, bld];
	var name2         = [3,1," K "," Wo",-1, bld];
	var name3         = [4,1,"nsl","rld",-1, bld];
	var name4         = [5,1,"aye"," Ma",-1, bld];
	var name5         = [6,1,"r  ","p  ",-1, bld];
	var name6         = [7,1,"   ","   ",-1, bld];
	var hold          = [15,0,"SH ","   ",81, red];
	var stoneRoad     = [14,0," .-","/  ",10];
	var stoneRoadProp = [14,1,"*  ","   ",333, ( getManor(getRoom(33301)).ownerUserId == 0 ? bld : ( getManor(getRoom(33301)).race == 1 ? red : cyn ) )];
	var stoneRoad2    = [13,1,"  /","-' ",10];
	var keep          = [12,1,"   "," RK",80, red];
	var nKeep         = [12,0,"   ","   ",85, null, null, "  X"];
	var sKeep         = [12,2,"  |",""   ,20];
	var gapProp       = [12,3,"*"   ,"",332, ( getManor(getRoom(33201)).ownerUserId == 0 ? bld : ( getManor(getRoom(33201)).race == 1 ? red : cyn ) )];
	var sKeep2        = [12,2,""   ,"  |",18];
	var niamh         = [13,2,"_  "," `",66];
	var niamh3        = [13,2,""   ,"N"  ,181, bld];
	var niamh2        = [14,2,"   ","P  ",181, bld];
	var gap           = [12,3,"TG",""   ,206, bld];
	var niamhOgier    = [13,3,"__/" ,"   ",181];
	var nFD           = [12,3,""   ,"__/",193];
	var wFD           = [11,4,"_"  ,""   ,64];
	var nFM           = [10,4,"   ",".-'",64];
	var FD            = [11,4,"FD" ,"   ",17, cyn];
	var wFM           = [9,4,"   ","__" ,93];
	var nFM2          = [9,4,""   ,"_"  ,64];
	var wFM2          = [8,5,"  |-","  ",93];
	var tvRoad        = [8,5,""   ,"|"  ,19];
	var sTV           = [7,6,(actor.room.zoneVnum == 12 ? "" : "   "),"  .",82];
	var TV            = [8,6,""   ,"TV" ,(actor.room.zoneVnum == 223 ? 223 : 213), cyn];
	var tvRoad2       = [8,6,"  |","'",19];
	var tvProp        = [9,6,"*  ","   ",334, ( getManor(getRoom(33401)).ownerUserId == 0 ? bld : ( getManor(getRoom(33401)).race == 1 ? red : cyn ) )];
	var FM            = [9,5,"FM" ,"   ",103, cyn];
	var lowCaem       = [6,7," ,-",""   ,70];
	var FK2           = [5,7,""   ,"-"  ,100];
	var FK            = [5,7,"   ","FK",3, cyn];
	var caemRoad      = [6,7,""   ,"-+" ,3];
	var caem          = [6,7,"","C",1, cyn];
	var caem2         = [7,7,""   ,"M"  ,1, cyn];
	var lowCaem2      = [7,7,"-." ,""   ,70];
	var sTV2          = [7,7,"|","",82];
	var aRoad         = [7,7,""   ,"-|",214];
	var aRoad2        = [8,7,"   ","---",214];
	var AR            = [9,7,"   ","AR ",217, cyn];
	var lugRoad       = [5,8,",' ","|  ",4];
	var sCaem         = [6,8," `-",(actor.room.zoneVnum == 14 ? "" : "   "),3];
	var sCaem2        = [7,8,"|-'","|  ",31];
	var LG            = [5,9,"LG ",""   ,201, cyn];
	var sLG           = [5,9,""   ,"|  ",9];
	var nFarMadding   = [7,9,"|  ","|  ",230];
	var blackTower    = [8,9,"   ","   ",235, null, "X  ", null];
	var wFK           = [4,7,"   ","---",100];
	var WB            = [3,7,"  " ,"WB"   ,186, cyn];
	var wFK2          = [3,7," ","-",100];
	var nWB           = [3,6,"   ","   ",53, null, null, " X "];
	var wWB           = [2,7,"_  "," `-",101];
	var wWB2          = [1,7,"-._","   ",102];
	var BR            = [0,7," BR","  ",45, cyn];
	var nRoad         = [0,7,"","|",134];
	var nRoad2        = [0,8,"  |","  |",134];
	var EF            = [0,9,"  E","   ",132,cyn];
	var EF2           = [1,9,"F  ","   ",132,cyn];
	var leg1          = [1,12,"Typ","   ",0,bld];
	var leg2          = [2,12,"e M","   ",0,bld];
	var leg3          = [3,12,"AP ","   ",0,bld];
	var leg4          = [4,12,"LEG","   ",0,bld];
	var leg5          = [5,12,"END","   ",0,bld];
	var leg6          = [6,12," to","   ",0,bld];
	var leg7          = [7,12," to","   ",0,bld];
	var leg8          = [8,12,"ggl","   ",0,bld];
	var leg9          = [9,12,"e l","   ",0,bld];
	var leg10         = [10,12,"ege","   ",0,bld];
	var leg11         = [11,12,"nd ","   ",0,bld];
	var leg12         = [12,12,"on/","   ",0,bld];
	var leg13         = [13,12,"off","   ",0,bld];
	var leg14         = [14,12,".  ","   ",0,bld];
	var rooms = [name, name2, name3, name4, name5, name6, hold, stoneRoad, stoneRoadProp, stoneRoad2, keep, nKeep, sKeep, sKeep2, gapProp, gap, niamh, niamh3, niamh2, niamhOgier, nFD, wFD, nFM, FD, wFM, nFM2, wFM2, FM, tvRoad, sTV, TV, tvRoad2, tvProp, FK2, FK, caemRoad, lowCaem, caem, caem2, lowCaem2, sTV2, aRoad, aRoad2, AR, lugRoad, sCaem, sCaem2, LG, sLG, nFarMadding, blackTower, wFK, WB, wFK2, nWB, wWB, wWB2, BR, nRoad, nRoad2, EF, EF2, leg1, leg2, leg3, leg4, leg5, leg6, leg7, leg8, leg9, leg10, leg11, leg12, leg13, leg14];
	for (var _autoKey in rooms) {
		var r = rooms[_autoKey];
		//here.echo(actor.room.vnum);
		if (actor.room.zoneVnum == r[4])
		{
			var actor_room = [r[0],r[1]];
			//here.echo(actor_room);
		}
	}
	var aMob = getMobAtRoom(20800, 20804);
	var damaneZones = [];
	if(aMob){
		mudLog(2, "koradin test: pushing svals");
		damaneZones.push(parseInt(getSval(aMob, 20980, 1122)));
		damaneZones.push(parseInt(getSval(aMob, 20980, 1123)));
		damaneZones.push(parseInt(getSval(aMob, 20980, 1124)));
		damaneZones.push(parseInt(getSval(aMob, 20980, 1125)));
		damaneZones.push(parseInt(getSval(aMob, 20980, 1126)));
		damaneZones.push(parseInt(getSval(aMob, 20980, 1127)));
	}
	mudLog(2, "koradin test: zones: "+damaneZones.join(" "));
	var buffer = "";
	if (actor.quest("MapLegend") != 2 || bool == true)
		var limit = 13;
	else
		var limit = 9;
	for (var y = 0; y <= limit; y++)
	{
		var duplicateX = false;
		//Top line.
		for (var x = 0; x <= 15; ++x)
		{
			for (var _autoKey in rooms) {
				var r = rooms[_autoKey];
				if(arrContains(damaneZones,r[4])){
					mudLog(2, "koradin test: damane zone");
					var hlColor = bld+grn;
				}else if(r[5]){
					var hlColor = r[5];
				}else{
					var hlColor = nrm;
				}
				if (x == 7 && y == 6 && actor.room.zoneVnum == 12 && count < 1) {
					buffer += yel+bld+"  X"+nrm;
					count += 1;
				}
				if (actor.room.zoneVnum == r[4])
				{
					if (r[0] == x && r[1] == y) {
						var city = false;
						for (var i=0;i<3;i++) {
							if (r[2].charCodeAt(i) > 64 && r[2].charCodeAt(i) < 91) {
								city = true;
							}
						}
						if (city == false) {
							if (r[2] == "   " && r[3] == "   " && r[6] != null) {
								buffer += bld+yel+r[6]+nrm;
								duplicateX = true;
							}
							else
								buffer += bld+yel+r[2]+nrm;
						}
						else {
							for (var i=0;i<3;i++) {
								if (r[2].charCodeAt(i) > 64 && r[2].charCodeAt(i) < 91) {
									buffer += bld+yel+r[2].charAt(i)+nrm;
								}
								else
									buffer += r[2].charAt(i);
							}
						}
						count += 1;
						city = false;
					}
				}
				else if (r[0] == x && r[1] == y)
				{
					buffer += hlColor + r[2] + nrm;
					count += 1;
				}
			}
			if (count > 0)
				count = 0;
			else
				buffer += "   ";
		}
		if (bool == true) { // LEGEND IS ON
			buffer += "     ";
			if (y == 0)
				buffer += bld+"LEGEND"+nrm;
			else if (y == 1)
				buffer += bld+yel+"X "+nrm+" - You are here!";
			else if (y == 2)
				buffer += red+"Dark Cities"+nrm;
			else if (y == 3)
				buffer += red+"SH"+nrm+" - Stronghold";
			else if (y == 5)
				buffer += bld+"* "+nrm+" - Property";
			else if (y == 6)
				buffer += bld+"TG"+nrm+" - Tarwin's Gap";
			else if (y == 7)
				buffer += cyn+"Light Cities"+nrm;
			else if (y == 8)
				buffer += cyn+"FD"+nrm+" - Fal Dara";
			else if (y == 9)
				buffer += cyn+"TV"+nrm+" - Tar Valon";
			else if (y == 10)
				buffer += cyn+"AR"+nrm+" - Aringill";
			else if (y == 11)
				buffer += cyn+"LG"+nrm+" - Lugard";
			else if (y == 12)
				buffer += cyn+"BR"+nrm+" - Baerlon";
		}
		buffer += "\n";
		//Bottom line
		for (var x = 0; x <= 15; ++x)
		{
			// var coor = [];
			// coor.push(x);
			// coor.push(y);
			for (var _autoKey in rooms) {
				var r = rooms[_autoKey];
				if(arrContains(damaneZones,r[4])){
					mudLog(2, "koradin test: damane zone");
					var hlColor = bld+grn;
				}else if(r[5]){
					var hlColor = r[5];
				}else{
					var hlColor = nrm;
				}
				if (x == 6 && y == 8 && actor.room.zoneVnum == 14 && count < 1) {
					buffer += yel+bld+"X  "+nrm;
					count += 1;
				}
				if (actor.room.zoneVnum == r[4])
				{
					if (r[0] == x && r[1] == y) {
						var city = false;
						for (var i=0;i<3;i++) {
							if (r[3].charCodeAt(i) > 64 && r[3].charCodeAt(i) < 91) {
								city = true;
							}
						}
						if (city == false)  {
							if (r[2] == "   " && r[3] == "   " && duplicateX == false && r[7] != null)
								buffer += bld+yel+r[7]+nrm;
							else
								buffer += bld+yel+r[3]+nrm;
						}
						else {
							for (var i=0;i<3;i++) {
								if (r[3].charCodeAt(i) > 64 && r[3].charCodeAt(i) < 91) {
									buffer += bld+yel+r[3].charAt(i)+nrm;
								}
								else
									buffer += r[3].charAt(i);
							}
						}
						count += 1;
						city = false;
					}
				}
				else if (r[0] == x && r[1] == y)
				{
					buffer += hlColor + r[3] + nrm;
					count += 1;
				}
			}
			if (count > 0)
				count = 0;
			else
				buffer += "   ";
		}
		if (bool == true) { // LEGEND IS ON
			buffer += "     ";
			if (y == 2)
				buffer += red+"* "+nrm+" - Property";
			else if (y == 3)
				buffer += red+"RK"+nrm+" - Ruined Keep";
			else if (y == 4)
				buffer += bld+"Neutral Areas"+nrm;
			else if (y == 5)
				buffer += bld+"NP"+nrm+" - Niamh Pass";
			else if (y == 7)
				buffer += cyn+"* "+nrm+" - Property";
			else if (y == 8)
				buffer += cyn+"FM"+nrm+" - Fal Moran";
			else if (y == 9)
				buffer += cyn+"CM"+nrm+" - Caemlyn";
			else if (y == 10)
				buffer += cyn+"FK"+nrm+" - Four Kings";
			else if (y == 11)
				buffer += cyn+"WB"+nrm+" - Whitebridge";
			else if (y == 12)
				buffer += cyn+"EF"+nrm+" - Emond's Field";
		}
		if (y<limit)
			buffer += "\n";
	}
	return buffer;
}

JSCharacter.prototype.gsend = function(string) {
	getCharCols(this);
	this.send(cyn + bld + "The Guardian of Life tells you, '" + string + "'" + nrm);
}




























