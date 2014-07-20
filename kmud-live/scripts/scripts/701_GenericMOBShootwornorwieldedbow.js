var script701 = function(self, actor, here, args, extra) {
	var ch = self;
	
	// See if bow is in inventory
	var bowInInventory = false;
	for (var _autoKey in self.inventory) {
		var item = self.inventory[_autoKey];
		if (item.value(constants.VALUE_WEAPON_TYPE) == constants.WEAPON_BOW) {
			bowInInventory = true;
		}
	}
	
	if ((ch.eq(constants.WEAR_BACK) && ch.eq(constants.WEAR_BACK).value(constants.VALUE_WEAPON_TYPE) == constants.WEAPON_BOW) ||
		(ch.eq(constants.WEAR_WIELD) && ch.eq(constants.WEAR_WIELD).value(constants.VALUE_WEAPON_TYPE) == constants.WEAPON_BOW) ||
		bowInInventory
	) {
		// If character is fighting, shoot current target
		if (ch.fighting) {
			smobShoot(ch, '', '');
		} else {
			// Else try to shoot a player in an adjacent room
			for (i=0; i<here.neighbors.length; i++) {
				room = here.neighbors[i];
				if (room) {
					for (var _autoKey in room.people) {
						var person = room.people[_autoKey];
						// Shoot the first player in the room
						if (person.vnum == -1) {
							smobShoot(ch, person, here.firstStep(room));
							break;
						}
					}
				}
			}
		}
	}
	
	function smobShoot(ch, vict, direction) {
		if (!(ch.eq(constants.WEAR_WIELD).value(constants.VALUE_WEAPON_TYPE) == constants.WEAPON_BOW)) {
			ch.comm("remove bow");
			// If mob is wielding a weapon, remove it and wear the projectile
			var currentWeapon = null;
			if (self.eq(constants.WEAR_WIELD)) {
				currentWeapon = self.eq(constants.WEAR_WIELD);
				var currentNamelist = currentWeapon.namelist.split(" ");
				ch.comm("remove " + currentNamelist[0]);
				if (self.eq(constants.WEAR_SHIELD)) {
					ch.comm("remove shield");
				}
			}
			
			// Have the mob rewear their weapon after shooting
			if (currentWeapon) {
				function rewearWeapon(vArgs) {
					ch.comm("remove bow");
					ch.comm("wear " + vArgs[0]);
					ch.comm("wear shield");
					ch.comm("wear bow");
				}
				setTimeout(50, rewearWeapon, currentNamelist);
			}
		}
		if (!ch.eq(constants.WEAR_WIELD)) {
			ch.comm("wield bow");
		}
		var victim = "";
		if (vict) {
			var namelist = vict.namelist.split(" ");
			victim = vict ? " " + namelist[0] + " " : "";
		}
		var dir = direction ? dirToText(direction) : "";
		
		ch.comm("shoot" + victim + dir);
	}
}