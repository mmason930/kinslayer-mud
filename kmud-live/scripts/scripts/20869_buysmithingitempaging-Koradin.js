var script20869 = function(self, actor, here, args, extra) {
	for each (var person in actor.room.people) {
		for each (var pName in person.namelist.split(" ")) {
			if (pName == "dude") {
				var apprentice = person;
				break;
			}
		}
	}
	if (!apprentice) {
		actor.detach(20869);
		actor.send("The transaction is canceled because the apprentice has disappeared!");
		return;
	}
	var pack = apprentice.eq(constants.WEAR_BACK);
	if (!pack || pack.contents.length == 0) {
		actor.send("The transaction is canceled because the apprentice has given back all the items!");
		actor.detach(20869);
		return;
	}
	var itemList = pack.contents;
	getCharCols(actor);
	var vArgs = getArgList(args);
	if (strn_cmp(vArgs[0],"cancel",1)) {
		_block;
		actor.send("You cancel the transaction.");
		actor.detach(20869);
		return;
	}
	if(strn_cmp(vArgs[0],"examine",1)){
		_noblock;
		return;
	}
	_block;
	for (i=0; i< itemList.length; i++) {
		if (vArgs[0].toLowerCase() == "b"+(i+1)) {
			if (actor.gold < Math.floor(itemList[i].cost*getCostMultiplier(itemList[i].name))) {
				actor.send(red+capFirstLetter(apprentice.name) + " tells you 'You can't afford that!'"+nrm);
				actor.send(" ");
				actor.detach(20869);
				return;
			}
			else {
				actor.gold -= Math.floor(itemList[i].cost*getCostMultiplier(itemList[i].name));
				apprentice.say("Excellent choice, "+actor.name+"!");
				apprentice.comm("get "+itemList[i].namelist.split(" ")[0]+" pack");
				while (apprentice.inventory[0] != itemList[i])
					apprentice.comm("get "+itemList[i].namelist.split(" ")[0]+" pack");
				apprentice.inventory[0].moveToChar(actor);
				actor.send(capFirstLetter(apprentice.name)+ " gives you "+itemList[i].name+".");
				act("$n gives $N "+itemList[i].name+".",true,apprentice,null,actor,constants.TO_NOTVICT);
				apprentice.comm("put all pack");
				actor.detach(20869);
				return;
			}
		}else if(vArgs[0].toLowerCase() == "v"+(i+1)) {
			var obj = itemList[i];
			var baseObjLoc = "";
			var color = cyn;
			actor.send("You're examining "+color+obj.name+nrm+".");
			
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
			   var objChargeLand = obj.value(10);
			   if (objChargeLand > 0) {
				  var chgLowDmg = obj.value(8);
				  var chgHighDmg = obj.value(9);
				  if (chargePrac < 99) {
					 objChargeLand /= 3;
					 objChargeLand = dmgRangeToText(objChargeLand);
					 chgHighDmg /= 13;
					 chgHighDmg = dmgRangeToText(chgHighDmg);
				  }
				  if (chargePrac < 95) {
					 chgLowDmg /= 13;
					 chgLowDmg = dmgRangeToText(chgLowDmg);
				  }
				  actor.send("   Charge Rating:   "+color+objChargeLand+nrm);
				  actor.send("   Charge Range:    "+color+chgLowDmg+"-"+chgHighDmg+nrm);
			   }
			}
			actor.detach(20869);
			return;
		}
	}
	actor.send("That is not a valid choice.");
	actor.send("Choose another option, or type "+cyn+"C"+nrm+" to cancel the transaction.");
	return;
	
	
}