var script20910 = function(self, actor, here, args, extra) {
	var vArgs = getArgList(args);
	getCharCols(actor);
	if (!vArgs[1]) {
		actor.send("Examine what?");
		_block;
		return;
	}
	vArgs[1] = vArgs[1].toLowerCase();
	var listView = false;
	if(vArgs[2]){
		listView = true;
	}
	var exaArr = [];
	if(!listView){
		for (var _autoKey in actor.inventory) {//load items carried
			var thingster = actor.inventory[_autoKey];
			exaArr.push([thingster, "carried"]);
		}
		for (i=0; i<22; i++) {
			if (actor.eq(i))
				exaArr.push([actor.eq(i), "equipped"]); //load items equipped
		}
		for (var _autoKey in actor.room.items) {//load items on ground
			var thingie = actor.room.items[_autoKey];
			exaArr.push([thingie, "on ground"]);
		}
	}
	for (var _autoKey in actor.room.people) {
		var person = actor.room.people[_autoKey]; //load PC eq
		var playCheck = true;
		if(listView){ playCheck = (person.name.toLowerCase() == vArgs[2].toLowerCase()); }
		if (person.vnum == -1 && person != actor && playCheck) {
			for (i=0; i<22; i++) {
				if (person.eq(i))
					exaArr.push([person.eq(i), "worn by "+person.name]);
			}
		}
	}
	for (var _autoKey in actor.room.people) {
		var person = actor.room.people[_autoKey]; //load Mob eq
		var mobCheck = true;
		if(listView){ mobCheck = arrContains(person.namelist.split(" "), vArgs[2].toLowerCase()); }
		// actor.send("listView: "+listView+" and mobCheck: "+mobCheck+" for "+person.name);
		if (person.vnum > 0 && mobCheck) {
			for (i=0; i<22; i++) {
				if (person.eq(i)) {
                    actor.send("eq exists");
					if (!person.eq(i).extraFlags(constants.ITEM_INVISIBLE) && !person.eq(i).extraFlags(constants.ITEM_NODROP) && !person.eq(i).extraFlags(constants.ITEM_NO_SHOW))
						exaArr.push([person.eq(i), "worn by "+person.name]);
				}
			}
		}
	}
	var viewArr = [];
	for (i=0; i<exaArr.length; i++) {
		viewArr.push(exaArr[i][0]);
	}
	
	var obj = getObjInListVis(actor, vArgs[1], viewArr, false);
	if (!obj) {
		if(listView){
			actor.send("You don't see that here.");
			_block;
		}else{
			_noblock;
		}
		return;
	}
	if(global.global2014Util){ // examining pedestals for global event
		//sendKoradin("pedestals engaged");
		var pedestal = global.global2014Util.objectIdToPedestalMap[ obj.id ];
		if(pedestal){
			actor.send(" ");
			if(pedestal.race == constants.RACE_TROLLOC){
				var ld = "DARK";
			}else{
				var ld = "LIGHT";
			}
			actor.send("This pedestal belongs to the "+ld+".");
			actor.send("This pedestal currently has "+pedestal.getHitPoints()+" out of "+pedestal.getMaxHitPoints()+" hit points.");
			_block;
			return;
		}
	}
	if (isName("corpse", obj.namelist)) {
		_noblock;
		return;
	}
	if (isName("quiverforbows", obj.namelist)) {
		_noblock;
		return;
	}
	if (obj.countJS(2900) > 0) {
		_noblock;
		return;
	}
	var objLocation = "";
	var baseObjLoc = "";
	for (i=exaArr.length-1; i>-1; i--) {
		if (obj == exaArr[i][0])
			 baseObjLoc = exaArr[i][1];
	}
	objLocation = " ("+baseObjLoc+")";
	var color = cyn;
	if (obj.extraFlags(15) == true) // RARE check
		color = mag;
	if (isName("masterweapon", obj.namelist)) // Master weapon check
		color = grn;
	
	for (var _autoKey in actor.room.people) {
	
		var peep = actor.room.people[_autoKey];
		if (isName(peep.name, baseObjLoc) && peep != actor) {
			if (actor.fighting) {
				actor.send("You're too busy to examine that right now!");
				_block;
				return;
			}
			else if (peep.fighting) {
				actor.send(capFirstLetter(peep.name)+" is moving too fast - you can't examine that right now.");
				_block;
				return;
			}
			var newName = obj.name.split(" ");
			newName.splice(0,1);
			newName = newName.join(" ");
			peep.send(actor.name+" examines your "+newName+".");
		}
	}
	actor.send("You're examining "+color+obj.name+nrm+objLocation+".");
	if(obj.getPval("made_by")){ actor.send(yel+"This item was created by "+obj.getPval("made_by")+"."+nrm); }
	if (obj.clan > 0) {
		var msgCol = nrm;
		if (!actor.inClan(obj.clan))
			msgCol = red;
		actor.send(msgCol+"This item belongs to the "+clanNumToText(obj.clan)+"."+nrm);
	}
	if (isName("pickformining", obj.namelist) || (obj.vnum > 4584 && obj.vnum < 4604 && isName("ore", obj.namelist)) ) {
		actor.send(yel+"This item can be used for Mining."+nrm);
	}
	if (obj.vnum == 22111 || (obj.vnum > 4535 && obj.vnum < 4583 && isName("bar", obj.namelist))) {
		actor.send(yel+"This item can be used for Smithing."+nrm);
	}
	if (obj.vnum == 22116 || (obj.vnum > 4909 && obj.vnum < 4983)) {
		actor.send(yel+"This item can be used for Tailoring."+nrm);
	}
	if (obj.vnum == 2030)
		actor.send(yel+"This item can be used for Mixing."+nrm);
	if (obj.vnum > 20984 && obj.vnum < 20999) {
		actor.send(yel+"This item can be used for Mixing."+nrm);
	}
	var jsCount = 0;
	if (obj.countJS(20808) > 0) //bow
		jsCount += 1;
	if (obj.countJS(20808) > 0) //bow
		jsCount += 1;
	if (obj.countJS(20808) > 0) //bow
		jsCount += 1;
	if (obj.countJS(20849) > 0) //decay
		jsCount += 1;
	if (obj.countJS(2033) > 0) { //vial
		actor.send(cyn+bld+"This mixture can be quaffed or poured into a drinking container."+nrm);
	}
	else if ((obj.countJS() > jsCount) || obj.affects[0] || obj.type == constants.ITEM_ANGREAL)
		actor.send(cyn+bld+"This item has hidden properties!"+nrm);
	//actor.send("\n");
	baseObjLoc = baseObjLoc.split(" ");
	_block;
	if (obj.extraDescription != "undefined" && obj.type != constants.ITEM_WEAPON && obj.type != constants.ITEM_ARMOR)
		actor.send(obj.extraDescription);
	if (obj.type == constants.ITEM_LIGHT) {
		if (baseObjLoc[0] == "worn") {
			var vicName = baseObjLoc.splice(0,2);
			vicName = baseObjLoc.join(" ");
			actor.send("You attempt to test the light source, but "+vicName+" won't let you.");
		}
		else {
			var hours = obj.value(2);
			if (hours > 3)
				var msg = "very well lit.";
			else if (hours > 1)
				var msg = "fading.";
			else if (hours > 0)
				var msg = "about to go out!";
			else if (hours == 0)
				var msg = "burnt out.";
			else
				var msg = "glowing brighter than ever.";
			actor.send(cyn+bld+"When you test the light source, you find it is "+msg+nrm);
		}
		return;
	}
	else if (obj.type == constants.ITEM_FOUNTAIN || obj.type == constants.ITEM_DRINKCON) {
		if (baseObjLoc[0] == "worn") {
			var vicName = baseObjLoc.splice(0,2);
			vicName = baseObjLoc.join(" ");
			actor.send("You attempt to peek inside, but "+vicName+" won't let you.");
		}
		else {
			var maxDrinks = obj.value(0);
			var curDrinks = obj.value(1);
			var drLeft = Math.floor((curDrinks/maxDrinks)*100);
			var liq = liquidNumToText(obj.value(2));
			if (drLeft > 94) {
				var msg = "full of "+liq+".";
			}
			else if (drLeft > 64)
				var msg = "more than half full of "+liq+".";
			else if (drLeft > 34)
				var msg = "about half full of "+liq+".";
			else if (drLeft > 0)
				var msg = "less than half full of "+liq+".";
			else
				var msg = "empty."
			actor.send("When you look inside, you see:");
			actor.send("It's "+msg);
		}
		return;
	}
	else if (obj.type == constants.ITEM_CONTAINER) {
		if (baseObjLoc[0] == "worn") {
			var vicName = baseObjLoc.splice(0,2);
			vicName = baseObjLoc.join(" ");
			actor.send("You attempt to peek inside, but "+vicName+" won't let you.");
		}
		else {
		
			if(obj.isClosed)
			{
				actor.send("It is closed.");
			}
			else
			{
				var contents = obj.contents;
				var finalArr = [];
				for (i=0;i<contents.length;i++) {
					finalArr.push([contents[i],1]);
				}
				for (j=0;j<finalArr.length;j++) {
					for (k=0;k<j+1;k++) {
						if (finalArr[j][0].name == finalArr[k][0].name && finalArr[j][0] != finalArr[k][0]) {
							finalArr[k][1] += 1;
							finalArr.splice(j,1);
							j--;
						}
					}
				}
				actor.send("When you look inside, you see:");
				if (finalArr.length == 0) {
					actor.send("Nothing.")
					return;
				}
				for (var _autoKey in finalArr) {
					var content = finalArr[_autoKey];
					var quantity = "";
					if (content[1] > 1)
						quantity = " ["+content[1]+"]";
					actor.send(content[0].name+quantity);
				}
			}
		}
		return;
	}
	if (obj.extraFlags(12) == true && obj.extraFlags(16) == false && msgCol == red) { //NO SHOW on, TRADES off, actor out of clan
		return;
	}
	if (color == grn && !isName(actor.name, obj.namelist)) {
		actor.send("It looks strange to you...");
		return;
	}
	//actor.send(red+"The EXAMINE command is currently being updated! Check back soon.\n"+nrm);
	if (obj.type == constants.ITEM_ARMOR) {
		if (obj.absorb > 10 && obj.parry <= 0) {
			var objType = "abs";
			var viewPrac = Math.floor((actor.strength+actor.constitution)/39);
		}
		else if (obj.absorb < 10 && obj.dodge >= 0 && obj.parry >= 0) {
			var objType = "dodge";
			var viewPrac = Math.floor(actor.dexterity/20);
		}
		else {
			var objType = "combo";
			var viewPrac = Math.floor((actor.strength+actor.constitution+actor.dexterity)/58);
		}
		actor.send("It appears to be "+objType+" equipment.\n");
		var objWeight = Math.floor(obj.weight*100)/100;
		var pd = "pound";
		if (objWeight != 1)
			pd += "s";
		actor.send("   Weight:       "+color+objWeight+nrm+" "+pd);
		actor.send("   Parry Rating: "+color+obj.parry+nrm);
		actor.send("   Dodge Rating: "+color+obj.dodge+nrm);
		actor.send("   Abs Rating:   "+color+obj.absorb+nrm);
	}
	else if (obj.type == constants.ITEM_WEAPON) {
	   var aAn = " ";
	   var lowDmg = obj.value(constants.VALUE_WEAPON_DAM_LOW);
	   var highDmg = obj.value(constants.VALUE_WEAPON_DAM_HIGH);
	   if (obj.value(0) == 0) {
	      var objType = "long blade";
	      var viewPrac = actor.getSkill(getSkillVnum("Long Blades"));
	   }
	   else if (obj.value(0) == 1) {
	      var objType = "short blade";
	      var viewPrac = actor.getSkill(getSkillVnum("Short Blades"));
	   }
	   else if (obj.value(0) == 2) {
	      var objType = "club";
	      var viewPrac = actor.getSkill(getSkillVnum("Clubs"));
	   }
	   else if (obj.value(0) == 3) {
	      var objType = "staff";
	      var viewPrac = actor.getSkill(getSkillVnum("Staves"));
	   }
	   else if (obj.value(0) == 4) {
	      var objType = "spear";
	      var viewPrac = actor.getSkill(getSkillVnum("Spears"));
	   }
	   else if (obj.value(0) == 5) {
	      var objType = "axe";
	      aAn = "n ";
	      var viewPrac = actor.getSkill(getSkillVnum("Axes"));
	   }
	   else if (obj.value(0) == 6) {
	      var objType = "chain";
	      var viewPrac = actor.getSkill(getSkillVnum("Chains"));
	   }
	   else if (obj.value(0) == 7) {
	      var objType = "bow";
	      var viewPrac = actor.getSkill(getSkillVnum("Bows"));
	      lowDmg += obj.weight;
	      highDmg += obj.weight;
	      lowDmg = Math.floor(lowDmg);
	      highDmg = Math.floor(highDmg);
	   }
	   else {
	      var objType = "lance";
	      var viewPrac = actor.getSkill(getSkillVnum("Lance"));
	   }
	   if (obj.vnum < 2650 && obj.vnum > 2599 && obj.vnum != 2609) {
	      objType = "projectile";
	      lowDmg *= 10;
	      highDmg *= 10;
	   }
	   var oneTwo = "one";
	   if (obj.extraFlags(8) == true)
	      oneTwo = "two";
	   actor.send("It is a "+oneTwo+"-handed "+objType+".\n");
	   var objWeight = Math.floor(obj.weight*100)/100;
	   var pd = "lb";
	   if (objWeight != 1)
	      pd += "s";
	   actor.send("   Weight:          "+color+objWeight+nrm+" "+pd);
	   actor.send("   Offensive Bonus: "+color+obj.offensive+nrm);
	   actor.send("   Parry Bonus:     "+color+obj.parry+nrm);
	   if (viewPrac < 99) {
	      if (objType == "projectile")
	         highDmg /= 3;
	      highDmg = dmgRangeToText(highDmg);
	   }
	   if (viewPrac < 95) {
	      if (objType == "projectile")
	         lowDmg /= 3;
	      lowDmg = dmgRangeToText(lowDmg);
	   }
	   actor.send("   Damage Range:    "+color+lowDmg+"-"+highDmg+nrm);
	   /**var bashPrac = actor.getSkill(getSkillVnum("Bash"));
	   var objBash = obj.value(4);
	   if (objBash > 0) {
	      if (bashPrac < 99) {
	         objBash /= 3;
	         objBash = dmgRangeToText(objBash);
	      }
	      actor.send("   Bash Rating:     "+color+objBash+nrm);
	   }**/
	   var stabPrac = actor.getSkill(getSkillVnum("Backstab"));
	   var objStabLand = obj.value(7);
	   if (objStabLand > 0) {
	      var stabLowDmg = obj.value(5);
	      var stabHighDmg = obj.value(6);
	      if (stabPrac < 99) {
	         objStabLand /= 3;
	         objStabLand = dmgRangeToText(objStabLand);
	         stabHighDmg /= 13;
	         stabHighDmg = dmgRangeToText(stabHighDmg);
	      }
	      if (stabPrac < 95) {
	         stabLowDmg /= 13;
	         stabLowDmg = dmgRangeToText(stabLowDmg);
	      }
	      actor.send("   Backstab Rating: "+color+objStabLand+nrm);
	      actor.send("   Backstab Range:  "+color+stabLowDmg+"-"+stabHighDmg+nrm);
	   }
	   var chargePrac = actor.getSkill(getSkillVnum("Charge"));
       var skewerPrac = actor.getSkill(getSkillVnum("Skewer"));
	   var objChargeLand = obj.value(10);
	   if (objChargeLand > 0) {
	      var chgLowDmg = obj.value(8);
	      var chgHighDmg = obj.value(9);
	      if (chargePrac < 99 && skewerPrac < 99) {
	         objChargeLand /= 3;
	         objChargeLand = dmgRangeToText(objChargeLand);
	         chgHighDmg /= 13;
	         chgHighDmg = dmgRangeToText(chgHighDmg);
	      }
	      if (chargePrac < 95 && skewerPrac < 95) {
	         chgLowDmg /= 13;
	         chgLowDmg = dmgRangeToText(chgLowDmg);
	      }
	      actor.send("   Charge Rating:   "+color+objChargeLand+nrm);
	      actor.send("   Charge Range:    "+color+chgLowDmg+"-"+chgHighDmg+nrm);
	   }
	}
	
	
	
	
	
	
	
	
	
}