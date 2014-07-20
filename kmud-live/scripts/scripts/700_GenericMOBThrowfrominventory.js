var script700 = function(self, actor, here, args, extra) {
	var vict = actor;
	
	// Loop through inventory
	for (var _autoKey in self.inventory) {
		var item = self.inventory[_autoKey];
		var item_type = item.value(constants.VALUE_WEAPON_TYPE);
		
		// If mob has a throwable item in inventory, continue
		if ((item_type == constants.WEAPON_SHORT_BLADE ||
			item_type == constants.WEAPON_AXE ||
			item_type == constants.WEAPON_SPEAR) &&
			!item.extraFlags(constants.ITEM_TWO_HANDED)
		) {
			// If mob is wielding a weapon, remove it and wear the projectile
			var currentWeapon = null;
			if (self.eq(constants.WEAR_WIELD)) {
				currentWeapon = self.eq(constants.WEAR_WIELD);
				var currentNamelist = currentWeapon.namelist.split(" ");
				self.comm("remove " + currentNamelist[0]);
			}
			var namelist = item.namelist.split(" ");
			self.comm("wear " + namelist[0]);
			
			// Have the mob rewear their weapon after throwing
			if (currentWeapon) {
				function rewearWeapon(vArgs) {
					self.comm("wear " + vArgs[0]);
				}
				setTimeout(35, rewearWeapon, currentNamelist);
			}
	
			namelist = vict.namelist.split(" ");		
			
			// Throw the projectile
			self.comm("throw " + namelist[0]);
			
			break;
		}
	}
	
}