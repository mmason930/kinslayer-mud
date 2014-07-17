var script2072 = function(self, actor, here, args, extra) {
	//DRAW for harness
	//Koradin - April
	actor.send("Harnesses have been temporarily disabled. Please check back later.");
	// var arg_array = getArgList(args);
	// if (!strn_cmp(arg_array[0],"draw",3) || arg_array[2])
	// {
		// _noblock;
		// return;
	// }
	// if (!arg_array[1] || arrContains(self.namelist.split(" "),arg_array[1].toLowerCase()) == true)
	// {
		// for (i = 2001; i < 2010; i++)
		// {
			// if (!self.getPval(i.toString()))
			// {
				// var pval_num = i-10;
				// var drawn = parseInt( self.getPval( i.toString() ) );
				// break;
			// }
		// }
	// }
	// else if (parseInt(arg_array[1]) > 9 || parseInt(arg_array[1]) < 1)
	// {
		// actor.send("Your harness has no slot with that number.");
		// _noblock;
		// return;
	// }
	// else if (isNaN(parseInt(arg_array[1])))
	// {
		// _noblock;
		// return;
	// }
	// else
	// {
		// if (self.getPval(  ( parseInt(arg_array[1])+2010 ).toString()  ) != "none")
		// {	
			// var pval_num = parseInt(arg_array[1])+2000;
			// var drawn = parseInt( self.getPval( ( parseInt(arg_array[1])+2010 ).toString() ) );
		// }
	// }
	// if (!drawn)
	// {
		// actor.send("The harness is empty.")
		// _noblock;
		// return;
	// }
	// if (actor.eq(constants.WEAR_WIELD))
	// {
		// actor.send("You're already wielding a weapon.");
		// _block;
		// return;
	// }
	// _block;
	// actor.send("You quickly draw " + getObjProto(drawn).name + " from " + self.name + ".");
	// act("$n quickly draws " + getObjProto(drawn).name + " from " + self.name + ".",true,actor,null,null,constants.TO_ROOM);
	// actor.loadObj(drawn);
	// actor.comm("wield " + getObjProto(drawn).namelist.split(" ")[0]);
	// self.setPval(pval_num.toString(),(pval_num-2000).toString()+". < Empty Slot >");
	// self.setPval( (pval_num+10).toString(), "none");
}