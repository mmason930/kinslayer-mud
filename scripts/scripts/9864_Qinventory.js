var script9864 = function(self, actor, here, args, extra) {
	//Alder April 2010
	//Global Trigger...DO NOT EDIT
		if ( strn_cmp(getArgList(args)[0],"qinventory",2) == true ) {
			_block;
			getCharCols(actor,constants.CL_OFF);
			var inv = actor.getQuestInventory();
			actor.send(bld+"Quest Inventory:"+nrm);
			for (var i = 0, item; item = inv[i++];) {
				var amt = item.count;
				actor.send(cyn+item.name+(amt > 1 ? " ["+item.count+"]" : "")+nrm);
			}
			if (!inv.length) {
				actor.send(cyn+" Nothing."+nrm);
			}
		}
}