//Smithing lib fxns
//Koradin - May 2010
function getSmithableArmorSlots( actor )
{
	var results = [];
	var skill = actor.getSkill(getSkillVnum('Smithing'));
	//name[0], unlock level[1], mat reqs[2], don't use[3], skill level[4], don't use[5], coal req[6], names[7]
	var boots = ["Boots",        10,6, "               ", 1,"a pair of",5,["boots","war boots","combat boots"] ];
	var hands = ["Gauntlets",    15,6, "           ",     2,"a pair of",5,["gauntlets", "war gloves"]];
	var belt = ["Belt",          25,6, "                ",3,"a",        5,["belt","girdle","war belt"]];
	var shoulders = ["Spaulders",35,8, "           ",     4,"a pair of",6,["spaulders","pauldrons","shoulder plates"]];
	var hat = ["Helmet",         35,8, "              ",  5,"a",        6,["helm","half-helm","helmet","war helm","visored helmet"]];
	var legs = ["Greaves",       45,10, "             ",   6,"a pair of",7,["greaves","leg plates"]];
	var arms = ["Vambraces",     45,10, "           ",     7,"a pair of",7,["vambraces","arm plates"]];
	var plate = ["Breastplate",  55,12,"         ",       8,"a",        8,["plate","breastplate","cuirass"]];
	var shield = ["Shield",      65,12,"              ",  9,"a",        9,["buckler","shield"],["aegis","tower shield","kite shield"]];
	var slots = [boots,hands,belt,shoulders,hat,legs,arms,plate,shield];
	
	for each ( var slot in slots )
	{
		if (skill >= slot[1])
			results.push(slot);
	}
	return results;
}
function getTailoringArmorSlots( actor )
{
		var results = [];
		var skill = actor.getSkill(getSkillVnum('Tailoring'));
		//name[0], unlock level[1], mat reqs[2], don't use[3], skill level[4], don't use[5], thread req[6], names for dodge[7], names for combo[8]
		var boots = ["Boots",         10,6, "               ", 1,"a pair of",5,["moccasins","shoes","slippers","sandals"],["boots","riding boots"] ];
		var hands = ["Gloves",        15,6, "           ",     2,"a pair of",5,["gloves","riding gloves"],["half-gauntlets","gauntlets","gloves"] ];
		var belt = ["Belt",           25,6, "                ",3,"a",        5,["belt","sash"],["belt"] ];
		var shoulders = ["Pads",      35,8,"           ",      4,"a pair of",6,["pads","coverings","shoulder pads"],["spaulders","ailettes"] ];
		var hat = ["Hat",             45,8, "              ",  5,"a",        6,["cap","hat","hood","headband"],["half-helm","hood","helmet","helm","coif"] ];
		var legs = ["Leggings",       55,10, "             ",  6,"a pair of",7,["leggings","pants","breeches","stockings"],["leggings","leg plates","greaves"] ];
		var arms = ["Sleeves",        65,10, "           ",    7,"a pair of",7,["sleeves","coverings","sleeves"],["sleeves","vambraces","arm plates"] ];
		var plate = ["Vest",          75,12,"         ",       8,"a",        8,["shirt","vest","blouse","brassiere","tunic","corset"],["tunic","vest","breastplate","shirt","hauberk"] ];
		var shield = ["Cloak",        90,12,"              ",  9,"a",        9,["cloak","robe","cape","gown"],["coat","jacket","cloak","mantle"] ];
		var slots = [boots,hands,belt,shoulders,hat,legs,arms,plate,shield];
	
		for each ( var slot in slots )
		{
			if (skill >= slot[1])
				results.push(slot);
		}
		return results;
}
function getSmithableWeapons( actor )
{
	var results = [];
	var skill = actor.getSkill(getSkillVnum('Smithing'));
//name[0], unlock level[1], mat req[2], don't use[3], skill level[4], don't use[5], coal req[6], 1h names[7], 2h/throwing axe names[8]
	var prodagger = ["Small Projectile",10,3,"    ",           1,"a",2,["throwing knife","dart","throwing star","war quoit","chakram"]];
	var dagger = ["Short Blade",      20,6,"         ",        1,"a",5,["short blade","dagger","knife","baselard","poniard","stiletto","dirk","kukri","machete","hunting knife","belt knife"]];
	var prospear = ["Large Projectile", 15,4,"    ",           2,"a",2,["throwing spear","javelin"],["tomahawk","throwing axe"]];
	var spear = ["Spear",             30,16,"               ",  3,"a",6,["spear","javelin"],["partisan","double-forked spear"]];
	var lance = ["Lance",             30,16,"               ",  4,"a",6,["lance","war lance"],["war lance","lance"]];
	var chain = ["Chain",             40,16,"               ",  5,"a",7,["chain whip","ball-and-chain","chain scythe"],["slungshot","double-weighted chain","ball-and-chain"]];
	var staff = ["Staff",             40,14,"               ",  6,"a",6,["staff","stave","cane","war staff"],["quarterstaff","trident","war staff"]];
	var axe = ["Axe",                 50,18,"                 ",7,"a",8,["axe","war hatchet","war axe","half-moon axe"],["battle axe","double-bladed battle axe","double-bladed axe","war voulge","spiked double-scythe"]];
	var sword = ["Long Blade",        50,18,"          ",       8,"a",8,["long blade","longsword","war sword","scimitar","sword","cutlass"],["double-bladed war sword","claymore","broadsword"]];
	var club = ["Club",               60,20,"                ",9,"a",8,["club","spiked mace","cudgel","hammer"],["truncheon","mallet","war club","war hammer"]];
	var weapons = [prodagger,prospear,dagger,spear,lance,staff,chain,axe,sword,club];
	for each (var weapon in weapons)
	{
		if (skill >= weapon[1])
			results.push(weapon);
	}
	return results;
}
function getSmithableTypes( actor )
{
	var results = [];
	var skill = actor.getSkill(getSkillVnum('Smithing'));
	var Tin = ["Tin",10,2,"               "];
	var Aluminum = ["Aluminum",20,5,"          "];
	var Copper = ["Copper",30,3,"            "];
	var Iron = ["Iron",40,3,"              "];
	var Steel = ["Steel",50,7,"             "];
	var Bronze = ["Bronze",60,7,"            "];
	var Silver = ["Silver",70,8,"            "];
	var Gold = ["Gold",80,7,"              "];
	var Platinum = ["Platinum",90,9,"          "];
	var Obsidian = ["Obsidian",99,9,"          "];
	var types = [Tin,Aluminum,Copper,Iron,Steel,Bronze,Silver,Gold,Platinum,Obsidian]
	for each (var type in types)
	{
		if (skill >= type[1])
			results.push(type);
	}
	return results;
}
function getTailoringTypes( actor, kind )
{
	if (kind == "dodge")
	{
		var results = [];
		var skill = actor.getSkill(getSkillVnum('Tailoring'));
		var Tin = ["Skin",10,2,"            "];
		var Aluminum = ["Hide",25,5,"            "];
		var Copper = ["Cloth",45,3,"           "];
		var Iron = ["Linen",60,3,"           "];
		var Steel = ["Wool",70,7,"            "];
		var Bronze = ["Silk",80,7,"            "];
		var Silver = ["Velvet",90,8,"          "];
		var Gold = ["Fur",99,7,"             "];
		var types = [Tin,Aluminum,Copper,Iron,Steel,Bronze,Silver,Gold]
		for each (var type in types)
		{
			if (skill >= type[1])
				results.push(type);
		}
		return results;
	}
	else if (kind == "combo")
	{
		var results = [];
		var skill = actor.getSkill(getSkillVnum('Tailoring'));
		var Tin = ["Light Leather",10,2,"         "];
		var Aluminum = ["Leather",25,5,"               "];
		var Copper = ["Quilted Leather",45,3,"       "];
		var Iron = ["Padded Leather",60,3,"        "];
		var Steel = ["Studded Leather",70,7,"       "];
		var Bronze = ["Chainmail",80,7,"             "];
		var Silver = ["Scale",90,8,"                 "];
		var Gold = ["Brigandine",99,7,"            "];
		var types = [Tin,Aluminum,Copper,Iron,Steel,Bronze,Silver,Gold]
		for each (var type in types)
		{
			if (skill >= type[1])
				results.push(type);
		}
		return results;
	}
	return null;
}
function getCondition( actor, type )
{
	if (type == "abs")
	{
		var skill = actor.getSkill(getSkillVnum('Smithing'));
		var cond1 = ["radiant","you stand back and admire your expert work.",0,8];
		var cond2 = ["shimmering","your eyes light up, a grin splitting your face.",0,7];
		var cond3 = ["mighty","a smile appears on your face.",0,6];
		var cond4 = ["resilient","you nod in approval, examining your work.",0,5];
		var cond5 = ["tempered","you nod to yourself, examining your work.",0,4];
		var cond6 = ["durable","you sigh and wipe your brow.",0,3];
		var cond7 = ["thin","you sigh heavily and wipe your brow.",0,2];
		var cond8 = ["flimsy","disappointment flashes across your face.",0,1];
		var cond9 = ["misshapen","you mutter in disgust.",0,0];
	}
	else if (type == "combo")
	{
		var skill = actor.getSkill(getSkillVnum('Tailoring'));
		var cond1 = ["exquisite","you stand back and admire your expert work.",0,8];
		var cond2 = ["gleaming","your eyes light up, a grin splitting your face.",0,7];
		var cond3 = ["hardened","a smile appears on your face.",0,6];
		var cond4 = ["solid","you nod in approval, examining your work.",0,5];
		var cond5 = ["thick","you nod to yourself, examining your work.",0,4];
		var cond6 = ["mottled","you sigh and wipe your brow.",0,3];
		var cond7 = ["scratched","you sigh heavily and wipe your brow.",0,2];
		var cond8 = ["flimsy","disappointment flashes across your face.",0,1];
		var cond9 = ["cracked","you mutter in disgust.",0,0];
	}
	else if (type == "dodge")
	{
		var skill = actor.getSkill(getSkillVnum('Tailoring'));
		var cond1 = ["luxurious","you stand back and admire your expert work.",0,8];
		var cond2 = ["magnificent","your eyes light up, a grin splitting your face.",0,7];
		var cond3 = ["plush","a smile appears on your face.",0,6];
		var cond4 = ["supple","you nod in approval, examining your work.",0,5];
		var cond5 = ["comfortable","you nod to yourself, examining your work.",0,4];
		var cond6 = ["rough","you sigh and wipe your brow.",0,3];
		var cond7 = ["scratchy","you sigh heavily and wipe your brow.",0,2];
		var cond8 = ["patched","disappointment flashes across your face.",0,1];
		var cond9 = ["tattered","you mutter in disgust.",0,0];
	}
	else if (type == "weapon")
	{
		var skill = actor.getSkill(getSkillVnum('Smithing'));
		var cond1 = ["lethal","you stand back and admire your expert work.",0,8];
		var cond2 = ["wicked","your eyes light up, a grin splitting your face.",0,7];
		var cond3 = ["threatening","a smile appears on your face.",0,6];
		var cond4 = ["powerful","you nod in approval, examining your work.",0,5];
		var cond5 = ["strong","you nod to yourself, examining your work.",0,4];
		var cond6 = ["mediocre","you sigh and wipe your brow.",0,3];
		var cond7 = ["mundane","you sigh heavily and wipe your brow.",0,2];
		var cond8 = ["wimpy","disappointment flashes across your face.",0,1];
		var cond9 = ["harmless","you mutter in disgust.",0,0];
	}
	else
		return null;
	if (skill < 10)
		return null;
	var conditionFound = false;
	var results = [null,cond9,cond8,cond7,cond6,cond5,cond4,cond3,cond2,cond1];
	var skill_tier = Math.floor(skill/10);
	var skill_num = skill - (skill_tier*10);
	for (j = 1; j < results.length; j++)
	{
		 if (skill_tier == j /*&& j < 6*/)
		{
			if (j > 5)
			{
				if (j == 6)
				{
					results[3][2] += 50;
					results[4][2] += 300;
					results[5][2] += 500;
					results[6][2] += 100;
					results[7][2] += 50;
				}
				else if (j == 7)
				{
					results[4][2] += 225;
					results[5][2] += 500;
					results[6][2] += 200;
					results[7][2] += 50;
					results[8][2] += 25;
				}
				else if (j == 8)
				{
					results[4][2] += 150;
					results[5][2] += 475;
					results[6][2] += 250;
					results[7][2] += 75;
					results[8][2] += 50;
				}
				else if (j == 9)
				{
					results[4][2] += 50;
					results[5][2] += 450;
					results[6][2] += 300;
					results[7][2] += 100;
					results[8][2] += 75;
					results[9][2] += 25;
				}
			}
			else
			{
			if (results[j-2])
				results[j-2][2] += 150;
			else
			{
				if (results[j-1])
					results[j-1][2] += 150;
				else
					results[j][2] += 150;
			}
			if (results[j-1])
				results[j-1][2] += 300;
			else
				results[j][2] += 300;
			results[j][2] += 400;
			if (results[j+2])
			{
				results[j+2][2] += 50;
				results[j+1][2] += 100;
			}
			else
			{
				if (results[j+1])
				{
					results[j+1][2] += 50;
					results[j][2] += 100;
				}
				else
				{
					results[j][2] += 50;
					results[j-1][2] += 100;
				}
			}
			}
			for (k = 1; k < (skill_num+1); k++)
			{
				if (j < 6)
				{
				if (results[j-2] && results[j-2][2] > 0)
				{
					results[j-2][2] -= 3;
					results[j][2] += 3;
				}
				if (results[j-1] && results[j-1][2] > 0)
				{
					results[j-1][2] -= 15;
					results[j][2] += 15;
				}
				if (results[j+2])
				{
					results[j][2] -= 3;
					results[j+1][2] += 2;
					results[j+2][2] += 1;
				}
				else if (results[j+1])
				{
					results[j-1][2] -= 10;
					results[j][2] += 7;
					results[j+1][2] += 3;
				}
				}
				else
				{
					if (results[3][2] > 0)
						results[3][2] -= 25;
					else if (results[4][2] > 0)
						results[4][2] -= 25;
					else if (results[5][2] > 0)
						results[5][2] -= 25;
					if (results[j+2])
					{
						results[j+2][2] += 2;
						results[j+1][2] += 7;
						results[j][2] += 16;
					}
					else if (results[j+1])
					{
						results[j+1][2] += 2;
						results[j][2] += 7;
						results[j-1][2] += 16;
					}
					else
					{
						results[j][2] += 2;
						results[j-1][2] += 7;
						results[j-2][2] += 16;
					}
				}
			}
			conditionFound = true;
			break;
		}
	}
	if (conditionFound == false)
		results[1][2] += 1000;
	var chance = 0;
	var roll = random(0,1000);
	var aChance = [];
	for (l = 1; l < results.length; l++)
	{
		//actor.room.echo(results[l]);
		if (results[l][2] > 0)
		{
			chance += results[l][2];
			aChance.push([results[l],chance]);
		}
	}
	for (m = 0; m < aChance.length; m++)
	{
		//actor.room.echo(obj);
		if (roll < aChance[m][1])
			return aChance[m][0];
	}
	
	return null;
}
function awardPracs( actor, type, material, item )
{
	getCharCols(actor);
	if (!type)
		return null;
	if (material)
		material = material.toLowerCase();
	var skill = actor.getSkill(getSkillVnum(type));
	var skill_level = Math.floor(skill/10);
	if (type == "Smithing")
	{
		for each (var arm in getSmithableArmorSlots(actor))
		{
			if (item == arm[0])
				var item_level = arm[4];
		}
		if (!item_level)
		{
			for each (var weap in getSmithableWeapons(actor))
			{
				if (item == weap[0])
					var item_level = weap[4];
			}
		}
		if (!item_level)
			return null;
		for each (var mat in getSmithableTypes(actor))
		{
			if (material == mat[0].toLowerCase())
				var mat_level = Math.floor(mat[1]/10);
		}
		 if (!mat_level)
			 return null;
		var prac_chance_count = 0;
		if ( item_level >= skill_level )
			prac_chance_count += 1;
		else
			prac_chance_count = prac_chance_count - (skill_level - (item_level + random(0,2)));
		if ( mat_level >= skill_level )
			prac_chance_count += 1;
		else
			prac_chance_count = prac_chance_count - (skill_level - mat_level);
		if ( skill_level < 4 )
		{
			var prac_award = (prac_chance_count + 1);
			var random_roll = Math.floor(skill/10);
		}
		else if ( skill_level < 7 )
		{
			var prac_award = prac_chance_count;
			var random_roll = Math.floor(skill/7);
		}
		else
		{
			var prac_award = (prac_chance_count - 1);
			var random_roll = Math.floor(skill/5);
		}
		//actor.send(prac_award);
		if ( random(1,random_roll) < 3 )
			prac_award -= 1;
		if ( prac_award < 0 )	
			prac_award = 0;
	}
	else if (type == "Mining")
	{
		var coal = [20,"coal"];
		var tin = [20,"tin"];
		var alum = [20,"aluminum"];
		var cop = [30,"copper"];
		var iron = [40,"iron"];
		var silv = [50,"silver"];
		var gold = [60,"gold"];
		var plat = [70,"platinum"];
		var sid = [80,"obsidian"];
		var diam = [90,"diamond"];
		var ores = [coal,tin,alum,cop,iron,silv,gold,plat,sid,diam];
		if (skill_level < 4)
			var prac_award = 3;
		else if (skill_level < 7)
			var prac_award = 2;
		else
			var prac_award = 1;
			
		for each ( var mType in ores )
		{
			if (material == mType[1])
				var item_type = mType;
		}
		if (!item_type || item_type == undefined)
			item_type = coal;
		if (skill > (item_type[0]+random(0,30)))
			prac_award -= ( skill_level - (item_type[0]/10) );
		if (prac_award < 0)
			prac_award = 0;
		if (item == "randomMining")
			prac_award = 1;
	}
	else if (type == "Herbalism")
	{
		if (skill_level < 4)
			var prac_award = 3;
		else if (skill_level < 7)
			var prac_award = 2;
		else
			var prac_award = 1;
		if (item == 20989)
			prac_award += 1;
		if (random(1,skill) > 2)
			prac_award -= random(1,3);
		if (prac_award < 0)
			prac_award = 0;
	}
	else if (type == "Skinning")
	{
		if (skill_level < 4)
			var prac_award = 3;
		else if (skill_level < 7)
			var prac_award = 2;
		else
			var prac_award = 1;
		if (material == "skin")
		{
			if (skill > 39)
				prac_award -= 2;
		}
		else if (material == "hide")
		{
			if (skill > 59)
				prac_award -= 1;
		}
		if (item == "small" && random(1,3) == 1)
			prac_award -= 1;
		else if (item == "medium" && random(1,4) == 1)
			prac_award -= 1;
		else if (item == "large" && random(1,5) == 1)
			prac_award -= 1;
		if (random(1,skill) > 2)
			prac_award -= random(1,3);
		if (prac_award < 0)
			prac_award = 0;
	}
	else if (type == "Tailoring")
	{
		for each (var arm in getTailoringArmorSlots(actor))
		{
			if (item == arm[0])
				var item_level = arm[4];
		}
		if (!item_level)
			return null;
		for each (var mat in getTailoringTypes(actor,"dodge"))
		{
			if (material == mat[0].toLowerCase())
				var mat_level = Math.floor(mat[1]/10);
		}
		if (!mat_level)
		{
			for each (var mt in getTailoringTypes(actor,"combo"))
			{
				if (material == mt[0].toLowerCase())
					var mat_level = Math.floor(mt[1]/10);
			}
		}
		if (!mat_level)
			return null;
		var prac_chance_count = 0;
		if ( item_level >= skill_level )
			prac_chance_count += 1;
		else
			prac_chance_count = prac_chance_count - (skill_level - (item_level + random(0,2)));
		//sendKoradin(prac_chance_count);
		if ( mat_level >= skill_level )
			prac_chance_count += 1;
		else
			prac_chance_count = prac_chance_count - (skill_level - (mat_level + random(0,2)));
		//sendKoradin(prac_chance_count);
		if ( skill_level < 4 )
		{
			var prac_award = (prac_chance_count + 1);
			var random_roll = Math.floor(skill/10);
		}
		else if ( skill_level < 7 )
		{
			var prac_award = prac_chance_count;
			var random_roll = Math.floor(skill/7);
		}
		else
		{
			var prac_award = (prac_chance_count - 1);
			var random_roll = Math.floor(skill/5);
		}
		//actor.send(prac_award);
		//sendKoradin(prac_award);
		if ( random(1,random_roll) < 3 )
			prac_award -= 1;
		if ( prac_award < 0 )	
			prac_award = 0;
		//sendKoradin(prac_award);
	}
	else if (type == "Fishing")
	{
		if (skill_level < 4)
			var prac_award = 3;
		else if (skill_level < 7)
			var prac_award = 2;
		else
			var prac_award = 1;
	}
	else if (type == "Cooking")
	{
		if (skill_level < 4)
			var prac_award = 3;
		else if (skill_level < 7)
			var prac_award = 2;
		else
			var prac_award = 1;
	}
	else if (type == "Mixing")
	{
		if (skill_level < 4)
			var prac_award = 3;
		else if (skill_level < 7)
			var prac_award = 2;
		else
			var prac_award = 1;
		if (material == "fail")
			prac_award -= random(1,3);
		if (random(1,100) < skill)
			prac_award -= random(1,3);
		if ( (skill - item.skillRequired) > 14 )
			prac_award -= 3;
		else if ( (skill - item.skillRequired) > 9 )
			prac_award -= 2;
		else if ( (skill - item.skillRequired) > 4 )
			prac_award -= 1;
		if ( (item.skillRequired - skill) > 9 )
			prac_award -= 3;
		if (prac_award < 0)
			prac_award = 0;
	}
	
	if (type == "Mining")
		var title = "Miner";
	else if (type == "Herbalism")
		var title = "Herbalist";
	else if (type == "Smithing")
		var title = "Blacksmith";
	else if (type == "Fishing")
		var title = "Angler";
	else if (type == "Skinning")
		var title = "Carver";
	else if (type == "Tailoring")
		var title = "Tailor"
	else if (type == "Mixing")
		var title = "Alchemist";
	else if (type == "Cooking")
		var title = "Chef";
	//actor.send(actor.getSkill(getSkillVnum(type)) + prac_award);
	if ( (actor.getSkill(getSkillVnum(type)) + prac_award) > 99 || prac_award == 0)
		return null;
	else
	{
		sendKoradin(actor.name+" just raised "+type+" ("+actor.getSkill(getSkillVnum(type))+" to "+(actor.getSkill(getSkillVnum(type))+prac_award)+")");
		actor.setSkill(getSkillVnum(type), actor.getSkill(getSkillVnum(type))+prac_award);
		actor.send(bld+"Your skill in '"+nrm+cyn+type+nrm+bld+"' has increased!"+nrm);
		actor.comm("save");
		//actor.send(actor.getSkill(getSkillVnum(type)));
		//actor.send(prac_award);
		if (actor.getSkill(getSkillVnum(type)) == 99)
		{
			actor.send("You are now learned in "+type+".");
			actor.qval("artisan_"+type,1);
			actor.unlockTitle("Artisan "+title,"normal","trades");
			if (getNumberOfArtisanTrades(actor) > 4)
				actor.unlockAchievement(20801);
		}
		if (actor.getSkill(getSkillVnum(type)) > 74)
			actor.unlockTitle("Master "+title,"normal","trades");
		if (actor.getSkill(getSkillVnum(type)) > 39)
			actor.unlockTitle("Journeyman "+title,"normal","trades");
		if (actor.getSkill(getSkillVnum(type)) > 9)
			actor.unlockTitle("Apprentice "+title,"normal","trades");
	}
	
	return null;
}
function getSmeltingResults( strType, intWeight ) {
	if (!strType || !intWeight)
		return null;
	if (!str_cmp(strType, "bronze"))
		var weightRoll = random(10,16);
	else if (!str_cmp(strType, "steel"))
		var weightRoll = random(8,12);
	else if (!str_cmp(strType, "coal"))
		var weightRoll = random(3,6);
	else
		var weightRoll = random(4,8);
	if (intWeight < weightRoll)
		return 0;
	if (!str_cmp(strType, "tin"))
		var bars = [4581,4582];
	if (!str_cmp(strType, "aluminum"))
		var bars = [4577,4578];
	if (!str_cmp(strType, "copper"))
		var bars = [4579,4580];
	if (!str_cmp(strType, "iron"))
		var bars = [4575,4576];
	if (!str_cmp(strType, "steel"))
		var bars = [4565,4566];
	if (!str_cmp(strType, "bronze"))
		var bars = [4536,4537];
	if (!str_cmp(strType, "silver"))
		var bars = [4573,4560];
	if (!str_cmp(strType, "gold"))
		var bars = [4563,4564];
	if (!str_cmp(strType, "platinum"))
		var bars = [4553,4552];
	if (!str_cmp(strType, "obsidian"))
		var bars = [4550,4551];
	if (!str_cmp(strType, "coal"))
		var bars = [4571];
	var totalWeight = intWeight;
	var results = [];
	while (totalWeight > 0) {
		if (totalWeight > weightRoll) {
			if (bars[1])
				results.push(bars[1]);
			else
				results.push(bars[0]);
		}
		else {
			results.push(bars[0]);
		}
		totalWeight -= weightRoll;
	}
	return results;
}
function getPreparingResults( strType, intWeight ) {
	if (!strType || !intWeight)
		return null;
	if (!str_cmp(strType, "brigandine"))
		var weightRoll = random(13,19);
	else if (!str_cmp(strType, "scale"))
		var weightRoll = random(7,13);
	else if (!str_cmp(strType, "chainmail") || !str_cmp(strType, "studded") || !str_cmp(strType, "padded") || !str_cmp(strType, "quilted"))
		var weightRoll = random(10,15);
	else
		var weightRoll = random(4,8);
	if (intWeight < weightRoll)
		return 0;
	if (!str_cmp(strType,"skin"))
		var barVnums = [4951,4952];
	else if (!str_cmp(strType,"hide"))
		var barVnums = [4953,4954];
	else if (!str_cmp(strType,"cloth"))
		var barVnums = [4955,4956];
	else if (!str_cmp(strType,"linen"))
		var barVnums = [4957,4958];
	else if (!str_cmp(strType,"wool"))
		var barVnums = [4959,4960];
	else if (!str_cmp(strType,"silk"))
		var barVnums = [4961,4962];
	else if (!str_cmp(strType,"velvet"))
		var barVnums = [4963,4964];
	else if (!str_cmp(strType,"fur"))
		var barVnums = [4965,4966];
	else if (!str_cmp(strType,"light"))
		var barVnums = [4967,4968];
	else if (!str_cmp(strType,"leather"))
		var barVnums = [4969,4970];
	else if (!str_cmp(strType,"quilted"))
		var barVnums = [4971,4972];
	else if (!str_cmp(strType,"padded"))
		var barVnums = [4973,4974];
	else if (!str_cmp(strType,"studded"))
		var barVnums = [4975,4976];
	else if (!str_cmp(strType,"chainmail"))
		var barVnums = [4977,4978];
	else if (!str_cmp(strType,"scale"))
		var barVnums = [4979,4980];
	else if (!str_cmp(strType,"brigandine"))
		var barVnums = [4981,4982];
	var totalWeight = intWeight;
	var results = [];
	while (totalWeight > 0) {
		if (totalWeight > weightRoll) {
			if (barVnums[1])
				results.push(barVnums[1]);
			else
				results.push(barVnums[0]);
		}
		else {
			results.push(barVnums[0]);
		}
		totalWeight -= weightRoll;
	}
	return results;
}
function getCostMultiplier(strName) {
	if (!strName)
		return 0;
	var multi = 1;
	if (isName("obsidian", strName) || isName("fur", strName) || isName("brigandine", strName))
		multi = 4.02;
	else if (isName("platinum", strName) || isName("velvet", strName) || isName("scale", strName))
		multi = 2.51;
	else if (isName("gold", strName) || isName("silk", strName) || isName("chainmail", strName))
		multi = 1.03;
	else if (isName("silver", strName) || isName("wool", strName) || isName("studded", strName))
		multi = 0.48;
	else
		multi = 0.24;
	var condArr = [ "harmless tattered cracked misshapen", "wimpy patched flimsy", "mundane scratchy scratched thin", "mediocre rough mottled durable", "strong comfortable thick tempered", "powerful supple solid resilient", "threatening plush hardened mighty", "wicked magnificent gleaming shimmering", "lethal luxurious exquisite radiant" ];
	var name = strName.split(" ");
	var cond = 0;
	for (k=0; k<condArr.length; k++) {
		for each (var word in name) {
			if (isName(word, condArr[k])) {
				cond = 9-k;
				//sendKoradin(cond);
			}
		}
	}
	if(cond==0){cond = 1;}
	multi = multi/cond;
	if (isName("bar", strName) || isName("square", strName))
		multi = 1.1;
	//sendKoradin(multi);
	return multi;
}










