let script636 = function(self, actor, here, args, extra) {
	//Alder
	//May 2010
	////GLOBAL WHOIS COMMAND\\\\
	_block;
	getCharCols(actor);
	let vArgs = getArgList(args);
	if ( vArgs.length !== 2 ) {
		actor.send("Correct syntax for the whois command: 'whois name'");
		return;
	}
	let disp = "There is no such player.";
	let input = vArgs.join("");
	for (let i = 0; i < input.length; i++) {
		let a = getAscii(input[i]);
		if ( a < 65 || a > 122 || ( a > 90 && a < 97 ) ) {
			actor.send(disp);
			return;
		}
	}
	let searchName = vArgs[1];
	let pData = sqlQuery("SELECT * FROM users WHERE username = " + sqlEscapeQuoteString(searchName));
	getCharCols(actor);
	if ( pData.hasNextRow ) {
		let row = pData.getRow;

		/**Retrieve relevant character information for whois**/
		let name = row.get("username");
		let title = " "+row.get("title");
		let arrTit = title.split("");
		let len = arrTit.length;
		for ( let i = 0 ; i < len; i++ ) {
			let ch = arrTit[i];
			if ( ch === '[' || ch === ']' || ch === '-' ) {
				arrTit[i] = "";
			}
		}
		title = arrTit.join("");
		let desc = row.get("description").trim();
		let id = row.get("user_id");
		let race = row.get("race");
		let vClass = row.get("chclass");
		let classRace = [" "+classNumToText(vClass)," "+raceNumToText(race)];
		let level = parseInt(row.get("level"));
		let tempClass = parseInt(row.get("chclass"));
		let prf = row.get("prf");
		let prf1 = parseInt(prf.split(" ")[ 0 ]);
		let isIncog = false;
		let incogIndicator = "";
		let createdDatetime = new Date( row.getInteger("birth") * 1000 );
		let lastLoginDatetime = row.getTimestamp("last_logon");

		if( isBitSet(prf1, 25) ) {
			isIncog = true;
			if ( actor.level > 100 ) {
				incogIndicator = bld+" (INCOGNITO)"+nrm;
			}
			// actor.send((actor.name == "Alder" ? "incog: "+isIncog : ""));
		}
		let checkClasses = (tempClass === constants.CLASS_DREADGUARD || tempClass === constants.CLASS_DREADLORD || tempClass === constants.CLASS_FADE || tempClass === constants.CLASS_BLADEMASTER || tempClass === constants.CLASS_GREYMAN || tempClass === constants.CLASS_OGIER);
		if ( checkClasses ) {
			classRace.pop();
		}
		let legend = "";
		let legNames = getLegends();
		if ( arrContains(legNames,name) ) {
			legend = " legendary ";
		}
		let sTitle = "";
		let sType = "";
		let pref = "";
		let norm = "";
		let suff = "";
		let clanName = "";
		let rank = "";
		let council = "";
		let levText = "";
		let clanInfo = [];
		let titleResult = sqlQuery("SELECT * FROM userTitle WHERE user_id='"+sqlEsc(id)+"' && selected=1;");
		if ( titleResult.hasNextRow ) {
			let tRow = titleResult.getRow;
			sTitle = tRow.get("title");
			sType = tRow.get("type");
			if ( sType > "" ) {
				pref = ( sType === "prefix" ? sTitle+" " : "" );
				suff = ( sType === "suffix" ? sTitle : "" );
			}
		}
		if ( !isIncog || actor.level >= 100 ) {
			let clanResult = sqlQuery("SELECT * FROM userClan WHERE user_id="+id+";");
			while ( clanResult.hasNextRow ) {
				let cRow = clanResult.getRow;
				let clanNum = cRow.get("clan_id");
				if( (typeof clanIsSecret != "undefined" && !clanIsSecret(clanNum)) || (typeof clanIsSecret == "undefined" && clanNum !== 15 && clanNum !== 13) ) {
					clanName = " "+clanNumToText(clanNum);
					rank = " "+getClanRankName(clanNum, cRow.get("rank"));
					council = parseInt(cRow.get("is_council"));
					council = ( council ? (clanNum >= 16 && clanNum <= 23 ? " Sitter" : " Councilman") : "" );
				}
				if ( council.length )
					rank = "";
				let cObj = {};
				cObj.name = clanName;
				cObj.rank = rank;
				cObj.council = council;
				clanInfo.push( cObj );
			}
			if ( clanInfo.length > 0 ) {
				classRace.pop();
				if ( clanInfo.length > 1 ) {
					for ( let i = 0; i < (clanInfo.length-1); i++ ) {
						let cObj = clanInfo[i];
						cObj.rank = "";
					}
				}
			}
			for ( let i = 0; i < clanInfo.length; i++ ) {
				let obj = clanInfo[i];
				clanInfo[i] = obj.name + "" + obj.rank + "" + obj.council + "";
			}
			if ( suff.length ) {
				classRace = [" "+suff];
			}

			levText = " is a"+legend;
			if ( level >= 100 ) {
				switch(level) {
					case 100:
						levText += "n Immortal of the Wheel";
						break;
					case 101:
						levText += " Builder of the Wheel";
						break;
					case 102:
						levText += "n Apprentice of the Wheel";
						break;
					case 103:
						levText += " Teller of the Wheel";
						break;
					case 104:
						levText += " Lord of the Wheel";
						break;
					case 105:
						levText += " Creator";
						break;
				}
				clanInfo = [];
				classRace = [];
			}
			else {
				levText += " level "+level;
			}
		}
		else {
			levText = " is a "+legend;
			if ( classRace.length > 1 ) {
				classRace.shift();
			}
			if ( suff.length ) {
				classRace = [" "+suff];
			}
		}
		classRace.reverse();
		disp = pref+name+title+levText+clanInfo.join(" and ")+classRace.join("")+"."+incogIndicator;

		if(desc.length > 0)
			disp += ("\n\n" + desc);

		disp += "\n";

		let arrDisp = getArgList(disp);
		disp = arrDisp.join(" ");
		let manorOwner = false;
		for (let _autoKey in global.manors) {
			let manor = global.manors[_autoKey];
			if (!str_cmp(searchName, getUserNameByUserId(manor.ownerUserId))) {
				manorOwner = true;
				disp += "\n"+bld+capFirstLetter(searchName)+" owns the "+manor.name+" near "+manor.area+"."+nrm;
			}
		}
		if (manorOwner === true)
			disp += "\n";
		disp += "\n" + cyn + name + " was created on " + createdDatetime.strftime("%B %d, %Y") + "." + nrm;

		if(level < 100 || actor.level >= level)
			disp += "\n" + cyn + name + " last logged in on " + lastLoginDatetime.strftime("%B %d, %Y") + "." + nrm;

	}
	else {
		let searchNames = sqlQuery("SELECT * FROM users WHERE username LIKE '"+sqlEsc(searchName)+"%' ORDER BY RAND();");
		if ( searchNames.hasNextRow ) {
			disp += "\n\nSome close matches: "+cyn;
			var names = [];
			while ( searchNames.hasNextRow ) {
				var row = searchNames.getRow;
				names.push(cyn+row.get("username")+nrm);
				if ( names.length > 35 ) {
					break;
				}
			}
			nameList = hangIndentStr(names.join(", "), 100, 20)+nrm;
			disp += nameList+nrm;
		}
	}
	actor.send(disp);









}