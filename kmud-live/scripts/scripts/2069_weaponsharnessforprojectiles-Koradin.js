var script2069 = function(self, actor, here, args, extra) {
	//projectiles harness
	//Koradin - April 2010
	actor.send("Harnesses have been temporarily disabled. Please check back later.");
	// var arg_array = getArgList(args);
	// if (!strn_cmp(arg_array[0],"sheath",3) || (arg_array[1] != undefined && !arrContains(self.namelist.split(" "),arg_array[1].toLowerCase())))
	// {
		// _noblock;
		// return;
	// }
	// var wielded = actor.eq(constants.WEAR_WIELD);
	// var knife = constants.WEAPON_SHORT_BLADE;
	// var spear = constants.WEAPON_SPEAR;
	// var axe = constants.WEAPON_AXE;
	// if (wielded)
	// {
		// if (wielded.type == constants.ITEM_WEAPON)
		// {
			// var wielded_type = wielded.value(constants.VALUE_WEAPON_TYPE);
			// if (!wielded.extraFlags(constants.ITEM_TWO_HANDED))
			// {
				// if (wielded_type == spear || wielded_type == knife || wielded_type == axe)
					// var end_item = wielded;
			// }
		// }
	// }
	// else if (!wielded || !end_item)
	// {
		// for each(var item in actor.inventory)
		// {
			// if (item.type == constants.ITEM_WEAPON)
			// {
				// var item_type = item.value(constants.VALUE_WEAPON_TYPE);
				// if (!item.extraFlags(constants.ITEM_TWO_HANDED))
				// {
					// if (item_type == spear || item_type == knife || item_type == axe)
					// {
						// var end_item = item;
						// break;
					// }
				// }
			// }
		// }
	// }
	// if (!end_item)
	// {
		// _noblock;
		// return;
	// }
	// for (i = 2009; i > 2000; i--)
	// {
		// if (!self.getPval(i.toString()))
			// var open_slot = i;
	// }
	// if (!open_slot)
	// {
		// actor.send("Your harness is full already.");
		// _noblock;
		// return;
	// }
	// _block;
	// actor.send("You slide " + end_item.name + " into " + self.name + ".");
	// act("$n slides " + getObjProto(end_item.vnum).name + " into " + self.name + ".",true,actor,null,null,constants.TO_ROOM);
	// self.setPval(open_slot.toString(),open_slot-2000+". "+getObjProto(end_item.vnum).name,true);
	// end_item.extract();
}