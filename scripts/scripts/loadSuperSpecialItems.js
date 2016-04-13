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
		4584	// damage artifact
    ];
    
    // Pick a random item from the rare list. If there's already too many of them in the game then try again 3 times
	var counter = 0;
	for (counter = 0; counter < 3; counter++) {
		var index = random(0, rareLoadList.length-1);
		var proto = getObjProto(rareLoadList[index]);
		if (proto.count < proto.max) {
			self.loadObj(proto.vnum);
			break;
		}
	}
	
    // Pick 3 items from the other list and load 
    for (var i=0; i < 3; i++) {
        index = random(0, otherLoadList.length-1);
        self.loadObj(otherLoadList[index]);
    }
    
}