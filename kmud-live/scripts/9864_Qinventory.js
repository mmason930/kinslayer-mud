var script9864 = function(self, actor, here, args, extra)
{
	//Alder April 2010
//Global Trigger...DO NOT EDIT
	if ( strn_cmp(getArgList(args)[0],"qinventory",2) == true ) {
		_block;
		getCharCols(actor,constants.CL_OFF);
		var qInv = actor._questInv;
		actor.send(bld+"Quest Inventory:"+nrm);
		for each ( var item in qInv ) {
			var amt = item.itemAmount;
			actor.send(cyn+item.itemName+(amt > 1 ? " ["+item.itemAmount+"]" : "")+nrm);
		}
		if ( qInv.length == 0 ) {
			actor.send(cyn+" Nothing."+nrm);
		}
	}

};

