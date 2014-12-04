var script20638 = function(self, actor, here, args, extra) {
	var argArray = getArgList(args);
	_block;
	if(argArray[1] == "weighted_random") {
		//		   0%		5%	   10%	   15%	   20%	   25%	   30%	   35%		40%		45%		50%		55%		60%		65%		70%		75%		80%		85%		90%		95%	   100%
		var c = [0.0000, .00380, .01475, .03222, .05570, .08475, .11895, .15798, .20155, .24931, .30210, .36040, .42265, .48113, .57143, .66667, .75000, .82353, .88889, .94737, 1.0000];
		var n = 0;
		var cIndex = argArray[2];
		var numTrials = argArray[3];
		var success = false;
		var numSuccess = 0;
		var numTries = 0;
		var seqMiss = 0;
		var maxMiss = 0;
		var seqHit = 0;
		var maxHit = 0;
		var hitMod = 0;
		var seqMisses = new Array(30);
		for (var i=0; i < 30; i++){seqMisses[i]=0;}
		// P(N)=C*N
		here.echo("Pseudo Random Distribution Test, desired P = ." + cIndex + ", number of trials = " + numTrials);
		for( var i = 0; i < numTrials; i++ ) {
			while(!success) {
				n++;
				if( random(0, 100) / 100 < c[Math.floor((cIndex / 5))]*(n - hitMod)) {
					success = true;
					numSuccess++;
					seqHit++;
					if(seqHit > 1)
						hitMod++;
				}
				else {
					seqMiss++;
					if(seqHit > maxHit)
						maxHit = seqHit;
					seqHit = 0;
					hitMod = 0;
				}
				numTries++;
				//here.echo("Attempt: " + n + ", Chance: " + c[cIndex / 5 - 1]*n + ", Success: " + success);
			}
			if(seqMiss > maxMiss)
				maxMiss = seqMiss;
			seqMisses[seqMiss]++;
			seqMiss = 0;
			success = false;
			n = 0;
		}
		here.echo("Successes: " + numSuccess + "  Total attempts: " + numTries + "  Chance of success: " + numSuccess/numTries);
		here.echo("Max sequential misses: " + maxMiss + "  Max sequential hits: " + maxHit);
		for( var i = 0; i < 30; i++ ) {
			if(seqMisses[i] > 0)
				here.echo(i + " misses in a row: " + seqMisses[i]);
		}
	}
	if(argArray[1] == "legends2011") {
		var year = "2011";
		if(argArray[2] == "list"){
			if( !argArray[3] ) {
				actor.send("Correct format is fogeltest legends2010 list <Number to List>");
				return;
			}
			var query_text = "SELECT pk.*,u.username,u.race,u.chclass,u.level,COUNT(pk.killer_id) AS kill_count, SUM(pk.wp_transfer) AS wp_total, (SUM(pk.wp_transfer)/COUNT(pk.killer_id)) AS avg FROM player_kills pk, users u WHERE pk.time_of_death >= '"+year+"-01-01 00:00:00' AND pk.time_of_death <= '"+year+"-12-31 23:59:59' AND u.user_id=pk.killer_id GROUP BY pk.killer_id ";
			var order_and_desc = "ORDER BY wp_total DESC LIMIT " + argArray[3] + ";";
			//var order_and_desc = "ORDER BY kill_count DESC LIMIT " + argArray[3] + ";";
			//var order_and_desc = "ORDER BY avg DESC LIMIT " + argArray[3] + ";";
			var legends = sqlQuery(query_text + order_and_desc);
			var count = 1;
			while(legends.hasNextRow){
				var row = legends.getRow;
				var name = row.get("username");
				var chclass = row.get("chclass");
				var level = row.get("level");
				var race = row.get("race");
				var wp = row.get("wp_total");
				//var wp = row.get("kill_count");
				//var wp = row.get("avg");
				
				actor.send( count + ") " + name + " the " + raceNumToText(race) + " " + classNumToText(chclass) + " - " + wp );
				count++;
				
			}
		}
		else if(argArray[2] == "totals"){
			var query_text = "SELECT pk.* FROM player_kills pk WHERE pk.time_of_death >= '"+year+"-01-01 00:00:00' AND pk.time_of_death <= '"+year+"-12-31 23:59:59'";
			var query_2_text = "SELECT pk.* FROM player_kills pk WHERE pk.time_of_death >= '"+year+"-01-01 00:00:00' AND pk.time_of_death <= '"+year+"-12-31 23:59:59' GROUP BY pk.killer_id";
			var query_3_text = "SELECT pk.* FROM player_kills pk WHERE pk.time_of_death >= '"+year+"-01-01 00:00:00' AND pk.time_of_death <= '"+year+"-12-31 23:59:59' GROUP BY pk.victim_id";
			var race_query_text = "SELECT pk.*, u.username, u.race, COUNT(pk.killer_id) AS kill_count, SUM(pk.wp_transfer) as wp_total FROM player_kills pk, users u WHERE pk.time_of_death >= '2011-01-01 00:00:00' AND pk.time_of_death <= '2011-12-31 23:59:59' AND u.user_id=pk.killer_id GROUP BY u.race;";
			var total_pks = sqlQuery(query_text);
			var total_wps = 0;
			while( total_pks.hasNextRow ){
				var row = total_pks.getRow;
				total_wps += parseInt(row.get("wp_transfer"));
			}
			var query_2 = sqlQuery(query_2_text);
			var query_3 = sqlQuery(query_3_text);
			var query_4 = sqlQuery(race_query_text);
			actor.send( "Total Number of Player Kills: " + total_pks.numRows );
			actor.send( "Total Amount of Weave Points Exchanged: " + total_wps);
			actor.send( "Total Number of Characters With Player Kills: " + query_2.numRows );
			actor.send( "Total Number of Characters With Player Deaths: " + query_3.numRows );
			while( query_4.hasNextRow ){
				var row = query_4.getRow;
				actor.send( "Total " + capFirstLetter(raceNumToText(row.get("race"))) + " Weave Point Gain: " + row.get("wp_total"));
				actor.send( "Total " + capFirstLetter(raceNumToText(row.get("race"))) + " Player Kills: " + row.get("kill_count"));
			}
		}
	}
	if( argArray[1] == "smelting" ) {
		var mat = "tin";
		var totalWeight = 100;
		var result = getSmeltingResult(mat, totalWeight);
		for (var _autoKey in here.items) {
			var thing = here.items[_autoKey];
			for (var _autoKey in thing.namelist.split(" ")) {
				var name = thing.namelist.split(" ")[_autoKey];
				if (name == "quenching") {
					var barrel = thing;
					break;
				}
			}
			wait 5;
			if (barrel) {
				barrel.loadObj(result);
				here.echo("The bar of "+mat + " cools enough to drop into the quenching barrel with a loud HISS!");
			}
		}
	}
	if( argArray[1] == "arenaStatsByName" ) {
		var character = argArray[2];
		var characterId = sqlQuery("SELECT user_id FROM users WHERE username = '" + character + "' LIMIT 1");
		var userId = parseInt(characterId.getRow.get("user_id"));
		var queryText = "SELECT * FROM pvals WHERE sKey= 'arenaStats' AND owner_id = '" + userId + "'";
		var queryResult = sqlQuery(queryText);
		while( queryResult.hasNextRow ){
			var row = queryResult.getRow;
			here.echo(row.get("value"));
		}
	}
	if( argArray[1] == "arenaStats" ) {
		var queryText = "SELECT u.username, u.last_logon, p.* FROM pvals p JOIN users u ON u.user_id = p.owner_id WHERE p.sKey = 'arenaStats' AND p.value != '0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~' AND p.value != '0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~0'"; //AND u.last_logon > '2011-08-18 16:30:00' AND u.last_logon <= '2011-08-18 22:30:00' ";
		var queryResult = sqlQuery(queryText);
		while( queryResult.hasNextRow ) {
			var row = queryResult.getRow;
			here.echo("Owner: " + row.get("username") + "    Value: " + row.get("value"));
		}
	}
	/*if( argArray[1] == "clearArena" ) {
		var queryText = "DELETE FROM pvals WHERE sKey = 'arenaStats' AND value != '0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~' AND value != '0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~0' AND owner_id != '38' AND owner_id != '35280' AND owner_id != '6091' AND owner_id != '35600' AND owner_id != '3020' AND owner_id !='33929' AND owner_id !='35617'";
		var queryResult = sqlQuery(queryText);
		var deleteTitlesText = "DELETE FROM userTitle WHERE (title = 'Conscript' OR title = 'Scout' OR title = 'Soldier' OR title = 'Second Squadman' OR title = 'Squadman' OR title = 'File Leader') AND user_id != '38' AND user_id != '35280' AND user_id != '6091' AND user_id != '35600' AND user_id != '3020' AND user_id !='33929' AND user_id !='35617'";
		var deleteTitlesResult = sqlQuery(deleteTitlesText);
		while( queryResult.hasNextRow ) {
			var row = queryResult.getRow;
			here.echo("Owner: " + row.get("owner_id") + "    Value: " + row.get("value"));
		}
		while( deleteTitlesResult.hasNextRow ) {
			var row = deleteTitlesResult.getRow;
			here.echo("Owner: " + row.get("user_id") + "    Title: " + row.get("title"));
		}
	}*/
	if( argArray[1] == "inactive_rares" ) {
        var query = '';
        if (argArray[2] == "list") {
            query = "SELECT s.username, s.last_login, s.vnum, s.object_id FROM "
                            + "(SELECT u.username, MAX(ul.login_datetime) AS last_login, o.vnum, o.id AS object_id FROM users u JOIN userLogin ul ON u.user_id = ul.user_id JOIN objects o ON u.user_id = o.holder_id "
                            + "WHERE (o.vnum='1300' OR o.vnum='10994' OR o.vnum='2405' OR o.vnum='1322' OR o.vnum='2609' OR o.vnum='1911' OR o.vnum='1708' "
                            + " OR o.vnum='1401' OR o.vnum='1400' OR o.vnum='1299' OR o.vnum='1416' OR (o.vnum >= '21792' AND o.vnum <= '21799') OR o.vnum='611' "
                            + " OR o.vnum='1100' OR o.vnum='2501' OR o.vnum='2500' OR o.vnum='21476' OR o.vnum='1016') "
                        + "GROUP BY username) AS s WHERE s.last_login <= DATE_SUB(CURDATE(), INTERVAL 30 DAY)";
        } else if (argArray[2] == "purge") {
            query =   " UPDATE"
                        + "  objects,"
                        + "  users,"
                        + "  obj_protos"
                        + " SET"
                        + "  objects.holder_id='34246',"
                        + "  objects.top_level_holder_id='34246',"
                        + "  objects.holder_type='P',"
                        + "  objects.top_level_holder_type='P',"
                        + "  objects.pos = -1"
                        + " WHERE objects.holder_type='P' "
                        + " AND objects.holder_id=users.user_id "
                        + " AND obj_protos.vnum=objects.vnum "
                        + " AND obj_protos.extra_flags & (1 << 15) "
                        + " AND users.last_logon <= DATE_SUB(CURDATE(), INTERVAL 30 DAY)";
        } else {
            actor.send("Proper syntax: fogeltest inactive_rares list|purge");
            return;
        }
		var result = sqlQuery(query);
		
		while( result.hasNextRow ) {
			var row = result.getRow;
			actor.send(row.get('username') + " " + row.get('last_logon') + " " + row.get('vnum')+ " " + row.get('id'));
		}
	}
	if( argArray[1] == "find_rares" ) {
		var allQuery = "SELECT obj_protos.sdesc, obj_protos.vnum " +
						"FROM obj_protos " +
						"WHERE obj_protos.extra_flags & (1 << 15) " +
						"ORDER BY obj_protos.vnum ASC";		
		var allResult = sqlQuery(allQuery);
		
		var existsQuery = "SELECT users.username, objects.vnum, obj_protos.sdesc " +
							"FROM users " +
							"LEFT JOIN objects " +
							"ON users.user_id = objects.top_level_holder_id " +
							"LEFT JOIN obj_protos " +
							"ON objects.vnum = obj_protos.vnum " +
							"WHERE objects.top_level_holder_type = 'P' " +
							"AND obj_protos.extra_flags & (1 << 15)" +
							"AND users.level < 100 " +
							"ORDER BY objects.vnum ASC";
			
		var existsResult = sqlQuery(existsQuery);
	
	
		if( argArray[2] == "list_all" ) {	
			var buffer = "";
			buffer += "+-------------------------------------------------------------------------------+--------------+\n";
			buffer += "| Object Name                                                                   | Object Vnum  |\n";
			buffer += "+-------------------------------------------------------------------------------+--------------+\n";
			while( allResult.hasNextRow ) {
				var row = allResult.getRow;
				buffer += ("| " + 
							strPadding(row.get('sdesc'), " ", 65, "right") + 
							"              | " + 
							strPadding(row.get('vnum'), " ", 10, "right") + 
							"    |\n");
				//actor.send(row.get('username') + " " + row.get('sdesc') + " " + row.get('vnum'));
			}
			buffer += "+-------------------------------------------------------------------------------+--------------+\n";
			actor.send(buffer);
		} else if( argArray[2] == "list_exists" ) {		
			var buffer = "";
			buffer += "+----------------+-------------------------------------------------------------------------------+--------------+\n";
			buffer += "| Username       | Object Name                                                                   | Object Vnum  |\n";
			buffer += "+----------------+-------------------------------------------------------------------------------+--------------+\n";
			while( existsResult.hasNextRow ) {
				var row = existsResult.getRow;
				buffer += ("| " + 
							strPadding(row.get('username'), " ", 10, "right") + 
							"      | " + 
							strPadding(row.get('sdesc'), " ", 65, "right") + 
							"              | " + 
							strPadding(row.get('vnum'), " ", 10, "right") + 
							"    |\n");
				//actor.send(row.get('username') + " " + row.get('sdesc') + " " + row.get('vnum'));
			}
			buffer += "+----------------+-------------------------------------------------------------------------------+--------------+\n";
			actor.send(buffer);
		} else if( argArray[2] == "list_missing" ) {
			var inGame = new Array();
			var notInGame = new Array();
			while (existsResult.hasNextRow) {
				var row = existsResult.getRow;
				inGame[row.get('vnum')] = 1;
			}
			while (allResult.hasNextRow) {
				var row = allResult.getRow;
				if (!inGame[row.get('vnum')]) {
					notInGame[row.get('vnum')] = row.get('sdesc');
				}
			}		
			var buffer = "";
			buffer += "+-------------------------------------------------------------------------------+--------------+\n";
			buffer += "| Object Name                                                                   | Object Vnum  |\n";
			buffer += "+-------------------------------------------------------------------------------+--------------+\n";
			for (obj in notInGame) {
				buffer += ("| " + 
				strPadding(notInGame[obj], " ", 65, "right") + 
				"              | " + 
				strPadding(obj, " ", 10, "right") + 
				"    |\n");
				if (argArray[3] == "and_create") {
					actor.loadObj(obj);
				}
			}
			buffer += "+-------------------------------------------------------------------------------+--------------+\n";
			actor.send(buffer);
			
		}
	}
	if( argArray[1] == "audit_qps" ) {
		var victim = argArray[2];
		var sum = 0;
		var query = "SELECT qps.user_id, "
					+ "qps.amount, "
					+ "qps.created_datetime, "
					+ "qps.issued_by_user_type, "
					+ "qps.issued_by_user_id, "
					+ "qps.reason, "
					+ "u.username "
					+ "FROM clanQuestPointTransaction qps "
					+ "LEFT JOIN users u "
					+ "ON qps.user_id = u.user_id "
					+ "WHERE u.username = '" + victim + "'";
		var res = sqlQuery(query);
		var buffer = "";
		buffer += "+----------------+----------------------------------------------------------------+--------+\n";
		buffer += "| Username       | Reason                                                         | Amount |\n";
		buffer += "+----------------+----------------------------------------------------------------+--------+\n";
		while( res.hasNextRow ) {
			var row = res.getRow;
			buffer += ("| " + 
						strPadding(row.get('username'), " ", 10, "right") + 
						"      | " + 
						strPadding(row.get('reason'), " ", 50, "right") + 
						"              | " + 
						strPadding(row.get('amount'), " ", 5, "left") + 
						"   |\n");
			sum += Number(row.get('amount'));
		}
		buffer += "+----------------+----------------------------------------------------------------+--------+\n";
		buffer += "+                +                                                                + Total  +\n";
		buffer += "+----------------+----------------------------------------------------------------+--------+\n";
		buffer += "+                +                                                                +" + strPadding(sum, " ", 5, "left") + "    +\n";
		actor.send(buffer);
	}
	if (argArray[1] == "list_weaves") {
	    var sql = "SELECT	* " +
				"FROM	weaveAttribute " +
	            "WHERE  attribute_name IN ('Air', 'Earth', 'Fire', 'Water', 'Spirit', 'DmgLow', 'DmgHigh', 'Disabled', 'Vnum')";
		var res = sqlQuery(sql);    
	    var weaves = new Array();
	    while (res.hasNextRow) {
	        row = res.getRow;
	        var name = row.get('name');
	        var attr = row.get('attribute_name');
	        var val = row.get('attribute_value');
	        if (weaves[weaves.length-1] == undefined || weaves[weaves.length-1]['name'] != name) {
	            var weave = new Array();
	        } else {
	            var weave = weaves.pop();
	        }
	        weave['name'] = name;
	        weave[attr] = val;
	        weaves.push(weave);
		}
		var buffer = "";
		buffer += "+------+---------------------+-----+-------+------+-------+--------+-------------------+----------+\n";
		buffer += "| Vnum | Weave name          | Air | Earth | Fire | Water | Spirit | Min Dmg | Max Dmg | Disabled |\n";
		buffer += "+------+---------------------+-----+-------+------+-------+--------+-------------------+----------+\n";
		for(var i = 0; i < weaves.length; i++) {
	        var weave = weaves[i];
	        if (weave['Disabled'] && weave['Disabled'] == 'Yes' && argArray[2] != 'showDisabled') {
	            continue;
	        }
	        if (weave['Vnum'] > 100) {
	            continue;
	        }
	        var vnum = '';
	        var air = '';
	        var earth = '';
	        var fire = '';
	        var water = '';
	        var spirit = '';
	        var dmgLow = '';
	        var dmgHigh = '';
	        var disabled = '';
	
	        if (weave['Vnum']) {
	            vnum = weave['Vnum'];
	        }
	        buffer += "| " + strPadding(vnum, " ", 5, "right");
	        buffer += " | ";
	        buffer += strPadding(weave['name'], " ", 20, "right") + 
	                    " | ";
	        if (weave['Air']) {
	            air = weave['Air'];
	        }
	        buffer += strPadding(air, " ", 4, "right");
	        buffer += " | ";
	        if (weave['Earth']) {
	            earth = weave['Earth'];
	        }
	        buffer += strPadding(earth, " ", 6, "right");
	        buffer += " | ";
	        if (weave['Fire']) {
	            fire = weave['Fire'];
	        }
	        buffer += strPadding(fire, " ", 5, "right");
	        buffer += " | ";
	        if (weave['Water']) {
	            water = weave['Water'];
	        }
	        buffer += strPadding(water, " ", 6, "right");
	        buffer += " | ";
	        if (weave['Spirit']) {
	            spirit = weave['Spirit'];
	        }
	        buffer += strPadding(spirit, " ", 7, "right");
	        buffer += " | ";
	        if (weave['DmgLow']) {
	            dmgLow = weave['DmgLow'];
	        }
	        buffer += strPadding(dmgLow, " ", 8, "right");
	        buffer += " | ";
	        if (weave['DmgHigh']) {
	            dmgHigh = weave['DmgHigh'];
	        }
	        buffer += strPadding(dmgHigh, " ", 8, "right");
	        buffer += " | ";
	        if (weave['Disabled']) {
	            disabled = weave['Disabled'];
	        }
	        buffer += strPadding(disabled, " ", 9, "right");
	        buffer += " | ";
	        buffer += "\n+------+---------------------+-----+-------+------+-------+--------+-------------------+----------+\n";
		}
	    actor.send(buffer);
	
	}
	
	if (argArray[1] == "list_smithable") {
		itemClass = argArray[2];
		itemType = argArray[3];
		searchTerms = '';
		for (i=2;i < argArray.length; i++) {
			searchTerms += " AND obj_protos.sdesc LIKE '%" + argArray[i] + "%'";
		}
		//itemMat = argArray[4];
		var query = "SELECT obj_protos.* " +
					"FROM obj_protos " +
					"WHERE 1 "  + searchTerms +
					"ORDER BY obj_protos.vnum ASC";
		var res = sqlQuery(query);
		
		var buffer = "";
		buffer += "+-------+-------------------------+----+----+----+--------+\n";
		buffer += "| Vnum  | Object Name             | OB | PB | DB | Weight |\n";
		buffer += "+-------+-------------------------+----+----+----+--------+\n";
		while (res.hasNextRow) {
	        row = res.getRow;
			buffer += ("| " + 
							strPadding(row.get('vnum'), " ", 7, "right") + 
							"| " + 
							strPadding(row.get('sdesc'), " ", 25, "right") + 
							"| " +
							strPadding(row.get('offensive'), " ", 4, "right") + 
							"| " +
							strPadding(row.get('parry'), " ", 4, "right") + 
							"| " +
							strPadding(row.get('dodge'), " ", 4, "right") + 
							"| " +
							strPadding(row.get('weight'), " ", 8, "right") + 
							"|\n");
		}
		actor.send(buffer);
	
	}
	
	if (argArray[1] == "wait_test") {
		here.echo("test 1/3");
        waitpulse 30;
        here.echo("test 2/3");
        waitpulse 60;
        here.echo("test 3/3");
	}
}