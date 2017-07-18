var loadSuperSpecialItems = function(self, actor, here, args, extra) {
    // List of rare items that could load
    var rareLoadList = [
        1300,    // a heron-marked longsword
        1322,   // a heron-marked blade from the Age of Legends
        2609,   // a jade-embedded cinquedia paired with a sturdy wooden sheath
        1911,   // a reflective onyx shield
        1708,   // an ornate Two Rivers longbow
        1401,   // a long bladed hunting knife
        1400,   // a perfectly clear glass dagger
        1299,   // an intricately carved longsword
        611,    // a golden heron marked belt
        1100,   // a yew spear with a fire blackened tip
        21476,  // a bear statuette
        1016,   // a berserker's pendant
        2504,   // a hero's talisman from the Age of Legends
        10618,  // a strange black helm with a double horn crest
        10012,  // a bright green orb
        2501,   // a thin strip of embroidered cloth
        2500,   // a figure of an armored soldier raising his sword to the heavens
		21792,  // a set of Sharan-marked boots
        21793,  // a set of Sharan-marked vambraces
        21794,  // a plumed ebony burgonet with Sharan markings
        21795,  // a pair of Sharan-marked steel-reinforced ebony gauntlets
        21796,  // a pair of Sharan-marked ebony greaves
        21797,  // a Sharan-marked, steel-reinforced ebony leather belt
        21798,  // Sharan-marked, ebony pauldrons
		21799,  // a Sharan-emblazoned cuirass
		10994,  // Cal'bekkar, the stained battle-maul of legends past
		1215,   // an ancient shocklance
		1321,   // a tainted black sword
		1414,   // the tainted blade of Shadar Logoth
		2405    // Rhilor, the tainted chain
    ];
    
    // List of other items that could load
    var otherLoadList = [
        2708,   // a vial filled with a bright blue liquid
        2718,   // a bubbling vial, cold to the touch
        2723,   // a fizzing orange vial
        2728,   // a sparkling white vial
        2607,   // a platinum throwing knife
        2606,   // a golden throwing knife
        2627,   // a platinum throwing spear
        2626,   // a golden throwing spear
        2648,   // a platinum throwing axe
        2647,   // a gold throwing axe
        2108,   // a bundle of soldier's rations (x5 to give higher chance)
        2108,   // a bundle of soldier's rations
        2108,   // a bundle of soldier's rations
        2108,   // a bundle of soldier's rations
        2108,   // a bundle of soldier's rations
        2109,   // a stick of dried rations (x5 to give higher chance)
        2109,   // a stick of dried rations
        2109,   // a stick of dried rations
        2109,   // a stick of dried rations
        2109,   // a stick of dried rations
		2023,   // a large water skin
		2023,   // a large water skin
		2023,   // a large water skin
		2023,   // a large water skin
		2023,   // a large water skin
        590,    // a bunch of fireworks
        22801,  // an enormous teddy bear
        22810,  // a toy sword
        4585,   // a chunk of obsidian ore
        4589,   // a chunk of platinum ore
        4591,   // a chunk of gold ore
        4590,   // a chunk of silver ore
        2039,   // a lightstick made of crystal
        4933,   // a large animal fur
        4936,   // a large animal pelt
        4939,   // a large pile of scales
        4924,   // a bolt of velvet
        4921,   // a bolt of silk
        4918,   // a bolt of wool
		4567,	// mv sapping artifact
		4568,	// stunning artifact
		4584,	// damage artifact
		20985,  // a few strings of sunburst root
  		20986,  // some blackwasp nettles
  		20987,  // a blisterleaf plant
  		20988,  // some blue goatflower petals
  		20989,  // a boneknit plant
  		20990,  // some crimsonthorn root
  		20991,  // some five-finger root
  		20992,  // some flatwort leaves
  		20993,  // an uprooted forkroot plant
 		20994,  // a handful of goatstongue
 		20995,  // a handful of greenwort
 		20996,  // a grey fennel plant
 		20997,  // a handful of fresh ivy leaves
 		20998,  // an uprooted sheepstongue plant
		2114,   // a hunk of beef
		21305  // a seasoned steak
    ];
    
	// Pick a random item. If there's already too many of them in the game, just go down the list until we find one we can use
	var index = random(0, rareLoadList.length-1);
	var proto = getObjProto(rareLoadList[index]);
	var canLoad = false;
	if (proto.count >= proto.max) {
		var counter = 0;
		for (counter; counter < rareLoadList.length, counter++) {
			proto = getObjProto(rareLoadList[counter]);
			if (proto.count < proto.max) {
				canLoad = true;
				break;
			}
		}
	} else {
		canLoad = true;
	}
	
	var numOtherLoads = 3;
	
	if (canLoad) {
		self.loadObj(proto.vnum);
	} else {
		// If we can't load a rare, give the player a couple bonus items.
		numOtherLoads = 5;
	}
    
	
    // Pick 3 items from the other list and load 
    for (var i=0; i < numOtherLoads; i++) {
        index = random(0, otherLoadList.length-1);
        self.loadObj(otherLoadList[index]);
    }
    
}
