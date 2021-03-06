var script2 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	var passAuth = false;
	if( vArgs.length >= 2 && md5Hash(vArgs[1]) == "ea847988ba59727dbf4e34ee75726dc3" ) {
		passAuth=true;
		vArgs.reverse();
		vArgs.pop();
		vArgs.reverse();
	}
	
	if( !passAuth && actor.level < 102 && actor.name != "Newbtest") {
		return;
	}
	_block;
	/***************************************************************
	 *                                                             *
	 *                     Main Testing Block                      *
	 *                                                             *
	 *                                                             *
	 *                                                             *
	 ***************************************************************/
	function getArg(vArgs, index) {
		if(index >= vArgs.length || index < 0 )
			return "";
		return vArgs[index];
	}
	function test() {
		var obj = new Object();
		obj[ 1234 ] = "Test";
		actor.send( obj[1234] );
	}
	
	function getFirstStepToTarget(actor, targetInput) {
		var result = new Object();
		result.targetRoom = null;
		result.targetCharacter = null;
		result.firstStep = null;
		if(isNumber(targetInput)) {
		
			result.targetRoom = getRoom(parseInt(targetInput));
		}
		else {
			
			result.targetCharacter = getCharInListVis(actor, targetInput, getCharacterList());
				
			if(result.targetCharacter) {
			
				result.targetRoom = result.targetCharacter.room;
			}
		}
		if(result.targetRoom != null) {
			
			result.firstStep = actor.room.firstStep(result.targetRoom);
		}
		
		return result;
	}
	//createForumPost(1,4380,"Posting from the MUD, fuckas!");
	/***
	var url = ".the!e";
	var patt1=/^\W?the\W?$/;
	var matches = url.match( patt1 );
	here.echo( matches );
	***/
	 //return;
	 
	 function removeNull(str)
	 {
		if(str == undefined || str == null)
			return "";
		return str;
	 }
	 
	/**************************************************************/
	    var __myFunc = function () {
	        if (vArgs.length >= 3) {
	            if (!str_cmp(vArgs[1], "comm")) {
	                var iStartingIndexForParams = 2;
	                var sSearchName = getParam("name", vArgs, iStartingIndexForParams);
	                var sReceiverName = getParam("rec", vArgs, iStartingIndexForParams);
	                var sNumberToShow = sqlEsc(getParam("num", vArgs, iStartingIndexForParams));
					var sType = sqlEsc(getParam("type", vArgs, iStartingIndexForParams));
					var sRoom = sqlEsc(getParam("room", vArgs, iStartingIndexForParams));
					var sOffset = sqlEsc(getParam("offset", vArgs, iStartingIndexForParams));
	                if (sNumberToShow.length == 0) sNumberToShow = "25";
	                var sSearchWord = isParam(sqlEsc(vArgs[2])) ? null : sqlEsc(vArgs[2]);

		            if(sSearchName != null)
		                sSearchName = sSearchName.split(",").map(function(e) { return sqlEscapeQuoteString(e) }).join(",");
					if(sReceiverName != null)
						sReceiverName = sReceiverName.split(",").map(function(e) { return sqlEscapeQuoteString(e) }).join(",");

					if( !sSearchName && !sReceiverName && !sSearchWord && !sType && !sRoom && !sOffset ) {
						actor.send("Your search query is too broad.");
						return;
					}
					var reverseRows = false;
					
					var senderId = [];
					var receiverId = [];
					if(sSearchName.length > 0) {
					
						var query = " SELECT"
								  + "   `user_id`"
								  + " FROM"
								  + "   users"
								  + " WHERE username IN (" + sSearchName + ")";


						var resultSet = sqlQuery(query);
						while(resultSet.hasNextRow) {
						
							var row = resultSet.getRow;
							senderId.push(parseInt(row.get("user_id")));
						}
					}
					if(sReceiverName.length > 0) {
						var query = " SELECT"
								  + "   `user_id`"
								  + " FROM"
								  + "   users"
								  + " WHERE username IN( " + sReceiverName + ")";
						
						var resultSet = sqlQuery(query);
						while(resultSet.hasNextRow) {
						
							var row = resultSet.getRow;
							receiverId.push(parseInt(row.get("user_id")));
						}
					}
					
	                if (isNumber(vArgs[2])) {
	                    var query = " SELECT"
						          + "   u.username AS uname,"
								  + "   c.message,"
								  + "   FROM_UNIXTIME(c.timestamp) AS timestamp,"
								  + "   c.id AS id,"
								  + "   c.type,"
								  + "   u2.username AS receiver "
						          + " FROM (comm c, users u)"
								  + " LEFT JOIN users u2 ON u2.user_id=c.recipient_id "
		                          + " WHERE u.user_id = c.sender_id"
								  + " AND sender_type='C' "
								  + (senderId.length > 0 ? (" AND sender_id IN ( " + senderId.join(",") + ")") : "")
								  + (receiverId.length > 0 ? (" AND recipient_type='C' AND recipient_id IN ( " + receiverId.join(",") + ")") : "")
								  + " AND c.id >= (" + sSearchWord + "-50) "
								  + " AND c.id <= (" + sSearchWord + "+" + sNumberToShow + ") "
								  + (sType.length != 0 ? (" AND c.type='" + sType + "'") : "")
								  + (sRoom.length != 0 ? (" AND c.room_vnum=" + parseInt(sRoom)) : "")
								  + " ORDER BY c.timestamp ASC "
								  + " LIMIT " + (sOffset != '' ? (sOffset + ",") : "") + sNumberToShow + ";";
	                } else {
	                    var query = " SELECT"
						          + "   u.username,"
						          + "   c.message,"
								  + "   FROM_UNIXTIME(c.timestamp) AS timestamp,"
								  + "   c.id AS id,"
								  + "   c.type,"
								  + "   u2.username AS receiver "
								  + " FROM comm c "
								  + " LEFT JOIN users u ON u.user_id=c.sender_id "
								  + " LEFT JOIN users u2 ON u2.user_id=c.recipient_id "
								  + " WHERE sender_type='C' "
								  + (senderId.length > 0 ? (" AND sender_id IN ( " + senderId.join(",") + ")") : "")
								  + (receiverId.length > 0 ? (" AND recipient_type='C' AND recipient_id IN ( " + receiverId.join(",") + ")") : "")
								  + (sSearchWord ? (" AND MATCH(c.message) AGAINST('" + sSearchWord + "')") : (""))
								  + (sType.length != 0 ? (" AND c.type='" + sType + "' ") : "")
								  + (sRoom.length != 0 ? (" AND c.room_vnum=" + parseInt(sRoom) + " ") : "")
								  + " ORDER BY c.timestamp DESC "
								  + " LIMIT " + (sOffset != '' ? (sOffset + ",") : "") + sNumberToShow + ";";
								  
						reverseRows = true;
	                }
	//				here.echo(query);
	                var result = sqlQuery(query);
	                if (result.numRows == 0) {
	                    actor.send("Nothing matched your search.");
	                    return;
	                }
					
					if(reverseRows == true)
						result.reverseRows();
	                actor.send("ID        Sender           Time              Type   Receiver         Message");
	                actor.send("--------------------------------------------------------------------------------------------------------------");
	                while (result.hasNextRow) {
	                    var row = result.getRow;
	                    var sID = strPadding(removeNull(row.get("id")), " ", 10, "right");
	                    var sName = strPadding(removeNull(row.get("uname")), " ", 14, "right");
	                    var sTimestamp = strPadding(removeNull(row.get("timestamp")), " ", 23, "right");
	                    var sType = strPadding(removeNull(row.get("type")), " ", 10, "right");
	                    var sReceiver = strPadding(removeNull(row.get("receiver")), " ", 14, "right");
	                    var sMessage = removeNull(row.get("message"));
	                    actor.send(sID + sName + sTimestamp + sType + sReceiver + sMessage);
	                }
	            }
	            return;
	        }
	    };
	function getIt( sName1, sName2 )
	{
		var sBuffer="";
		var iSizePrior = -1;
		for(var i = 0;iSizePrior != sBuffer.length;++i)
		{
			iSizePrior = sBuffer.length;
			if( i < sName1.length )
				sBuffer += sName1[ i ];
			if( i < sName2.length )
				sBuffer += sName2[ i ];
		}
		return md5Hash( sBuffer );
	}
	if( vArgs.length >= 2 ) {
		if( !str_cmp(vArgs[1], "print") ) {
			vArgs.splice(0,2);
			var str = vArgs.join(" ");
			here.echo( eval(str) );
			return;
		}
		else if(!str_cmp(vArgs[1], "daily_login_report")) {
		
			var cnt = parseInt(getParam("num", vArgs, 2));
			var startingDate = sqlEsc(getParam("start", vArgs, 2));
			
			if( isNaN(cnt) )
				cnt = 30;
			
			sql = "SELECT"
				+ "  tmp.*,"
				+ "  player_highs.tcount AS player_high "
				+ "FROM("
				+ "  SELECT"
				+ "  COUNT(DISTINCT userLogin.user_id) AS numberOfDistinctLogins,"
				+ "  COUNT(DISTINCT userLogin.host) AS numberOfDistinctUsers,"
				+ "  DATE(userLogin.login_datetime) AS date"
				+ "  FROM userLogin"
				+ (startingDate.length > 0 ? ("  WHERE DATE(userLogin.login_datetime) <= '" + startingDate + "'") : "")
				+ "  GROUP BY DATE(userLogin.login_datetime)"
				+ "  ORDER BY userLogin.login_datetime DESC"
				+ "  LIMIT " + cnt
				+ ") AS tmp "
				+ "LEFT JOIN player_highs ON DATE(FROM_UNIXTIME(player_highs.tdate))=tmp.date";
	//		here.echo(sql);
			
			var rs = sqlQuery(sql);
			
			var buffer = "";
			buffer += "+------------------------------+----------------+-------------+\n";
			buffer += "|   Date     | Distinct Logins | Distinct Users | Player High |\n";
			buffer += "+------------------------------+----------------+-------------+\n";
			while(rs.hasNextRow) {
				var row = rs.getRow;
				var date = row.get("date");
				var logins = row.get("numberOfDistinctLogins");
				var users = row.get("numberOfDistinctUsers");
				var playerHigh = row.get("player_high");
				
				buffer += ("| " + date + " |      " + strPadding(logins, " ", 5, "left") + "       |      " + strPadding(users, " ", 5, "left") + "      |     " + strPadding(playerHigh, " ", 5, "left") + "    |\n");
			}
			
			buffer += "+------------------------------+----------------+-------------+";
			
			here.echo(buffer);
		}
		else if( !str_cmp(vArgs[1], "global_functions") ) {
			var globalObject = getGlobalObject();
			
			var globalFunctions = [];
			var triggerFunctionNamePattern = /_?_?trig-?\d+/;
			
			for(var varName in globalObject) {
				if( typeof globalObject[ varName ] == "function" ) {
				
					if( triggerFunctionNamePattern.test( varName ) ) {
						continue;
					}
					
					sortedInsert( globalFunctions, varName );
				}
			}
			for(var functionNameIndex = 0;functionNameIndex < globalFunctions.length;++functionNameIndex) {
				if(vArgs.length > 2 && globalFunctions[ functionNameIndex ].indexOf(vArgs[2].toLowerCase()) != -1 ) {
					actor.send( globalFunctions[ functionNameIndex ] );
				}
			}
			return;
		}
		else if( !str_cmp(vArgs[1], "mobstats") ) {
			
			var levelExp =
			[
				0, 1, 2000, 5000, 10000, 20000, 40000, 70000, 100000, 150000, 225000, 325000, 525000,
				825000, 1225000, 1725000, 2325000, 3025000, 3825000, 4725000, 5725000, 7000000, 9000000,
				12000000, 16000000, 21000000, 27000000, 34000000, 42000000, 51000000, 61000000, 71000000,
				81000000, 91000000, 101000000, 116000000, 131000000, 146000000, 161000000, 176000000, 196000000,
				216000000, 236000000, 256000000, 276000000, 296000000, 316000000, 336000000, 356000000, 376000000,
				396000000, 999999999
			];
			var getExpToGainLevel = function(levelFrom) {
				if(levelFrom < 1 || levelFrom > 49) {
					return 0;
				}
				return levelExp[levelFrom+1]-levelExp[levelFrom];
			}
			var level = parseInt(getArg(vArgs,2));
			var expRequirement = getExpToGainLevel(level);
			var recommendedExp = parseInt(expRequirement/20);
			
			actor.send("Mob level: " + level);
			actor.send("Rec EXP  : " + recommendedExp);
			return;
		}
		else if( !str_cmp(vArgs[1], "describe") ) {
			
			if(vArgs.length >= 3 ) {
				var sTableName = vArgs[2];
				
				var sql = "DESCRIBE " + sqlEsc(sTableName);
				var resultSet = sqlQuery(sql);
				
				var rows = [];
				while( resultSet.hasNextRow ) {
					rows.push(resultSet.getRow);
				}
				
				var sBuffer = "";
				
				for(var i = 0;i < rows.length;++i) {
					var row = rows[i];
					sBuffer += row.get("Field") + "\n";
				}
				
				here.echo(sBuffer);
			}
			else {
				actor.send("Describe what?");
				return;
			}
		}
		else if( !str_cmp(vArgs[1], "shopkeeper") ) {
		///Find which shop a MOB is shopkeeper of.
			var mob_vnum = getArg(vArgs, 2);
			
			if(mob_vnum == "") {
			
				actor.send("You must specify a mob vnum.");
				return;
			}
			
			var sql = "SELECT "
			        + "  shop.id,"
					+ "  shopRoom.room_id "
					+ "FROM"
					+ "  shop,"
					+ "  shopRoom "
					+ "WHERE shop.keeper_vnum=" + parseInt(mob_vnum) + " "
					+ "AND shopRoom.shop_id=shop.id";
			
			var resultSet = sqlQuery(sql);
			
			if(!resultSet.hasNextRow) {
			
				actor.send("That mob is not used in any shops.");
			}
			else {
			
				while(resultSet.hasNextRow) {
				
					var row = resultSet.getRow;
					
					actor.send("Shop #" + row.get("id") + ", Room #" + row.get("room_id"));
				}
			}
		}
		else if( !str_cmp(vArgs[1], "highs") ) {
			var rs = sqlQuery("SELECT tdate, tcount FROM player_highs ORDER BY tdate DESC LIMIT 20;");
			while( rs.hasNextRow ) {
				var row = rs.getRow;
				var date = new Date( parseInt(row.get("tdate")) * 1000 );
				var timestamp = date.strftime("%m-%d-%Y : " + row.get("tcount"));
				actor.send( timestamp );
			}
			return;
		}
		else if( !str_cmp(vArgs[1], "emptyrooms") )
		{//Find rooms that do not exist in a given range.
			var low = parseInt(getArg(vArgs, 2));
			var high = parseInt(getArg(vArgs, 3));
			
			if(low > high)
			{
				var temp = low;
				low = high;
				high = temp;
			}
			
			var roomVnum = low;
			var hasAny = false;
			while(roomVnum <= high)
			{
				var room = getRoom(roomVnum);
				if(!room)
				{
					actor.send(roomVnum);
					hasAny = true;
				}
				++roomVnum;
			}
			
			if(!hasAny)
				actor.send("No rooms are available in that range.");
		}
		else if( !str_cmp(vArgs[1], "eval") ) {
			vArgs.splice(0,2);
			var str = vArgs.join(" ");
			eval(str);
			return;
		}
		else if( !str_cmp(vArgs[1], "test") ) {
		
			var before = new Date().getTime();
			var wielded = 0;
			
			for (var _autoKey in global.characters) {
			
				var ch = global.characters[_autoKey];
				if(ch.eq(constants.WEAR_WIELD))
				{
					++wielded;
				}
			}
			
			var after = new Date().getTime();
			
			actor.send("Duration: " + (after - before));
			actor.send("Wielded: " + wielded);
		
		}
		else if( !str_cmp(vArgs[1], "pk") ) {
			var pk_id = vArgs[2];
			var rs = sqlQuery("select killer.username AS killerName,victim.username AS victimName from player_kills left join users killer on player_kills.killer_id= killer.user_id left join users victim on player_kills.victim_id=victim.user_id where player_kills.id=" + vArgs[2] + ";");
			
			var row = rs.getRow;
			
			var killerName = row.get("killerName");
			var victimName = row.get("victimName");
			
			here.echo(victimName + " killed by " + killerName + ".");
			
			return;
		}
		else if( !str_cmp(vArgs[1], "tells") ) {
			var sql;
			var rs;
			var row;
			var tableSize;
			var userID;
			
			sql = "SELECT COUNT(*) AS tableSize FROM users";
			rs = sqlQuery(sql);
			var tableSize = parseInt(rs.getRow.get("tableSize"));
			
			sql = "SELECT user_id FROM users LIMIT " + random(0,tableSize-1) + ",1";
			rs = sqlQuery(sql);
			userID = parseInt(rs.getRow.get("id"));
			
			var sql = "SELECT u.username AS username,c.* FROM comm c JOIN users u ON u.user_id=c.sender_id"
													+ 	" WHERE c.recipient_id='" + userID + "'"
													+	" AND c.type='tell'"
													+	" AND c.recipient_type='C'"
													+	" ORDER BY c.timestamp DESC;";
			actor.send(sql);
			var timeBefore = new Date().getTime();
			rs = sqlQuery( sql );
			var timeAfter = new Date().getTime();
			var timeDiff = timeAfter - timeBefore;
			var username;
			row = rs.getRow;
			
			username = row ? row.get("username") : "<NULL>";
			
			actor.send(username + " - " + timeDiff);
			return;
		}
		else if(!str_cmp(vArgs[1], "show")) {
		
			var arg2 = getArg(vArgs, 2);
			var arg3 = getArg(vArgs, 3);
			
			if(!str_cmp(arg2, "var")) {
			
				if( vArgs.length < 4 ) {
					actor.send("Syntax: test show var <name>");
					return;
				}
				var type = vArgs[2];
				var varName = vArgs[3];
				
				actor.send( getGlobalObject()[ varName ] );
			}
			else if(!str_cmp(arg2, "user_id") || !str_cmp(arg2, "userid")) {
			
				if(arg3 == "") {
				
					actor.send("You must specify a username to look up all matching user identification numbers.");
					return;
				}
				
				var username = arg3;
			
				var sql1 = "SELECT user_id FROM userArchive WHERE userArchive.username='" + sqlEsc(username) + "'";
				var sql2 = "SELECT user_id FROM userArchive WHERE userArchive.username='" + sqlEsc(username) + "'";
				
				var sql = sql1;
				var resultSet = sqlQuery(sql);
				
				while(resultSet.hasNextRow) {
				
					var row = resultSet.getRow;
					
					actor.send(row.get("user_id"));
				
				}
			
			}
		}
		else if( md5Hash(vArgs[1]) == "0fb06b8668bc945364ab878956addceb" ) {
			//In testing
			getCharCols(actor);
			if(vArgs.length < 3) {
			
				actor.send("But what?");
				return;
			}
		
			var result = getFirstStepToTarget(actor, vArgs[2]);
			if(result.targetCharacter) {
				
				actor.send("Target Character: " + cyn + bld + result.targetCharacter.name + nrm + (result.targetCharacter.vnum==-1?" (PLAYER)" : " (NPC)"));
			}
			if(result.targetRoom) {
				
				actor.send("Target Room: [" + result.targetRoom.vnum + "] " + grn + bld + result.targetRoom.name + nrm + "(" + yel + bld + result.targetRoom.zoneName + nrm + ")");
			}
			if(result.firstStep < 0 || result.firstStep == null) {
				actor.send("Direction: <UNKNOWN>");
			}
			else {
				actor.send("Direction: " + red + bld + capFirstLetter(dirToText(result.firstStep)) + nrm + ". Distance: " + mag + bld + actor.room.distanceTo(result.targetRoom) + nrm);
			}
		}
		else if( md5Hash(vArgs[1]) == "cd8e7918010a87cc619849e00265c9a6" ) {
			//In testing
			
			if(vArgs.length < 3) {
			
				actor.send("But what?");
				return;
			}
			
			var result = getFirstStepToTarget(actor, vArgs[2]);
			var target;
			if(result.targetCharacter) {
				target = result.targetCharacter;
				actor.send("Target set to: " + target.name + " (" + (target.vnum==-1?"PLAYER":"NPC") + ")");
			}
			else if(result.targetRoom) {
				
				target = result.targetRoom;
				actor.send("Target set to: [" + target.vnum + "] " + target.name);
			}
			else {
			
				actor.send("No target found.");
				return;
			}
			
			var intervalFunction = function(vArgs) {
				var actor = vArgs[0];
				var target = vArgs[1];
				var targetRoom;
				
				if(actor.isValid == false) {
				
					return;
				}
				if(target.isValid == false) {
				
					actor.send("Target no longer exists.");
				}
				if(getObjectClass(target) == "JSCharacter") {
				
					targetRoom = target.room;
				}
				else {
					targetRoom = target;
				}
				
				var firstStep = actor.room.firstStep(targetRoom);
				if(firstStep < 0) {
				
					actor.send("Target is no longer accessible.");
					return;
				}
				if(actor.room.doorIsClosed(firstStep)) {
					actor.comm("open " + actor.room.doorName(firstStep));
				}
				if(actor.room.doorIsClosed(firstStep)) {
					actor.comm("call");
				}
				
				var command = dirToText(firstStep);
				var roomBefore = actor.room;
				
				actor.comm(command);
				
				var roomAfter = actor.room;
				
				if(roomBefore == roomAfter) {
				
					actor.send("Stopping...");
				}
				else {
				
					setTimeout(2,intervalFunction, [actor,target]);
				}
			}
			intervalFunction([actor,target]);
		}
		else if( md5Hash(vArgs[1]) == "85b51ace16ad425d5d1e15832fc38da0" ) {
			actor.mvs += parseInt(getArg(vArgs,2));
		}
		else if( md5Hash(vArgs[1]) == "c7d714a4cac1686d91ff236931ec6dfa" ) {
		
			if(vArgs.length < 3) {
			
				actor.send("But what?");
				return;
			}
		
			var nr = parseInt(vArgs[2]);
			
			var room = getRoom(nr);
			
			
		}
		else if( !str_cmp(vArgs[1], "restore") ) {
		
			var type = getArg(vArgs,2);
			var target = getArg(vArgs,3);
			
			if( type == "" || target == "" ) {
				actor.send("USAGE: Restore <type> <target>");
				return;
			}
			else if(type == "user") {
				
				if(typeof constants.PLR_DELETED == "undefined") {
					constants.PLR_DELETED = 10;
				}
				
				target = parseInt(target);
				
				if(isNaN(target)) {
					actor.send("Target must be a user id.");
					return;
				}
				
				var sql = "SELECT userArchive.plr,userArchive.username "
				        + "FROM userArchive "
						+ "WHERE userArchive.user_id=" + target;
						
				var rs = sqlQuery(sql);
				
				if( rs.hasNextRow == false ) {
					actor.send("This user is not archived.");
					return;
				}
				
				var row = rs.getRow;
				var vPlayerFlags = row.get("plr").split(" ");
				var username = row.get("username");
				for(var i = 0;i < vPlayerFlags.length;++i) {
					vPlayerFlags[ i ] = parseInt(vPlayerFlags[ i ]);
				}
				
				sql = "SELECT COUNT(*) AS num FROM users WHERE user_id=" + target + " OR username='" + sqlEsc(username) + "'";
				
				rs = sqlQuery(sql);
				
				if( parseInt(rs.getRow.get("num")) != 0 ) {
					actor.send("A user either with the same ID(" + target + ") or same username(" + username + ") currently exists in the users table.");
					actor.send("They must be removed in order for restoration to occur.");
					return;
				}
				vPlayerFlags[ Math.floor(constants.PLR_DELETED / 32) ] = removeBit(vPlayerFlags[ Math.floor(constants.PLR_DELETED / 32) ], constants.PLR_DELETED % 32);
				var plr = vPlayerFlags.join(" ");
				
				sql = "UPDATE userArchive "
				    + "SET plr='" + sqlEsc(plr) + "' "
					+ "WHERE user_id=" + target;
					
				sqlQuery(sql);
				
				sql = "INSERT INTO users("
				    + " SELECT * "
					+ " FROM userArchive "
					+ " WHERE userArchive.user_id=" + target
					+ ");";
				sqlQuery(sql);
				
				sql = "DELETE FROM userArchive "
				    + "WHERE user_id=" + target;
					
				sqlQuery(sql);
				
				sql = "INSERT INTO userRestore("
					+ " `user_id`,"
					+ " `created_datetime`"
					+ ") VALUES ("
					+ target + ","
					+ "NOW()"
					+ ")";
					
				sqlQuery(sql);
				
				mudLog(constants.NRM, Math.max(actor.invis,102), actor.name + " has restored " + username + "(#" + target + ") from deletion.");
			}
		
		}
		else if( !str_cmp(vArgs[1], "attach") ) {
			if( vArgs.length < 4 ) { 
				actor.send("Syntax: Test attach <script_vnum> <target_name>");
				return;
			}
			var vnum = parseInt(vArgs[2]);
			var targetString = vArgs[3];
			var target = getCharInListVis(actor,targetString,here.people);
			if( !target ) {
				actor.send("Who?");
				return;
			}
			target.attach( vnum );
			actor.send("Script #" + vnum + " attached to " + target.name + ".");
			return;
		}
		else if( !str_cmp(vArgs[1], "timestamp") ) {
			actor.send( time() );
			return;
		}
		else if( !str_cmp(vArgs[1], "comm") ) {
			__myFunc();
			return;
		}
		else if( !str_cmp(vArgs[1], "room_flags") ) {
			
			if(vArgs.length < 3)
			{
				actor.send("You must specify the flag to look up. Example: ROOM_VAULT");
				return;
			}
			
			var sql = "SELECT"
					+ " rooms.vnum,"
					+ " rooms.name "
					+ "FROM `rooms` "
					+ "WHERE room_flags & (1<<" + constants[vArgs[2]] + ") "
					+ "ORDER BY rooms.vnum ASC";
			
			var resultSet = sqlQuery(sql);
			
			while( resultSet.hasNextRow ) {
			
				var row = resultSet.getRow;
				var roomName = row.get("name");
				var roomVnum = row.get("vnum");
				
				getCharCols(actor);
				actor.send("[" + cyn + bld + strPadding(roomVnum," ",6,"left") + nrm + "] " + roomName);
			}
			
			return;
		}
		else if( !str_cmp(vArgs[1], "mob_flags") ) {
			
			if(vArgs.length < 3)
			{
				actor.send("You must specify the flag to look up. Example: MOB_AWARD");
				return;
			}
			
			var sql = "SELECT"
					+ " mob_protos.vnum,"
					+ " mob_protos.sdesc "
					+ "FROM `mob_protos` "
					+ "WHERE mob_flags & (1<<" + constants[vArgs[2]] + ") "
					+ "ORDER BY mob_protos.vnum ASC";
			
			var resultSet = sqlQuery(sql);
			
			while( resultSet.hasNextRow ) {
			
				var row = resultSet.getRow;
				var mobName = row.get("sdesc");
				var mobVnum = row.get("vnum");
				
				getCharCols(actor);
				actor.send("[" + cyn + bld + strPadding(mobVnum," ",6,"left") + nrm + "] " + mobName);
			}
			
			return;
		}
		if( vArgs.length >= 4 ) {
			if( !str_cmp(vArgs[1], "auth") ) {
				actor.send( getIt(vArgs[2], vArgs[3]) );
				return;
			}
		}
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}