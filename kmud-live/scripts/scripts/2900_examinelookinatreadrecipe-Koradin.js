var script2900 = function(self, actor, here, args, extra) {
	var argArray = getArgList(args);
	getCharCols(actor, constants.CL_SPARSE);
	if (!argArray[1] || argArray[3])
	{
		_noblock;
		return;
	}
	var rType = self.getPval("recType");
	if (!rType && !isName(argArray[1],self.namelist) && !isName(argArray[2],self.namelist)) {
		actor.send("As you attempt to read the parchment, it falls apart in your hands.");
		self.moveToRoom(getRoom(20899));
		self.extract();
		_block;
		return;
	}
	var recVnum = parseInt(self.getPval("recVnum"));
	var recipe = recVnum = getRecipeByVnum(recVnum, rType);
	var sTest = self.getPval("exaView");
	if ( (!recipe || !sTest) && !isName(argArray[1],self.namelist) && !isName(argArray[2],self.namelist) )
	{
		actor.send("The writing has become so smudged you can't make it out!");
		_block;
		return;
	}
	if (sTest == "tL")
		var newView = "yel+     ________+nrm+ ________\n+yel+    |        |+nrm+        |\n+yel+    |        |+nrm+        |\n+yel+    |   +  Mix+|+nrm+        |    The top left corner of a recipe for:\n+yel+    |________|+nrm+        |\n    |                 |\n    |                 |    +getObjProto(recVnum).name+\n    |                 |\n    |_________________|\n";
	else if (sTest == "tR")
		var newView = "     ________+yel+ ________+nrm+\n    |        +yel+|        |+nrm+\n    |        +yel+|        |+nrm+\n    |        +yel+|+ing +    |+nrm+    The top right corner of a recipe for:\n    |        +yel+|________|+nrm+\n    |                 |\n    |                 |    +getObjProto(recVnum).name+\n    |                 |\n    |_________________|\n";
	else if (sTest == "bL")
		var newView = "     _________________\n    |                 |\n    |                 |\n    |                 |    The bottom left corner of a recipe for:\n    |+yel+________+nrm+         |\n+yel+    |        |+nrm+        |\n+yel+    |     Rec|+nrm+        |    +getObjProto(recVnum).name+\n+yel+    |        |+nrm+        |\n+yel+    |________|+nrm+________|\n";
	else if (sTest == "bR")
		var newView = "     _________________\n    |                 |\n    |                 |\n    |                 |    The bottom right corner of a recipe for:\n    |        +yel+ ________+nrm+|\n    |        +yel+|        |+nrm+\n    |        +yel+|ipe     |+nrm+    +getObjProto(recVnum).name+\n    |        +yel+|        |+nrm+\n    |________+yel+|________|+nrm+\n";
	else if (sTest == "bL~bR")
		var newView = "     _________________\n    |                 |\n    |                 |\n    |                 |    The bottom half of a recipe for:\n    |+yel+_________________+nrm+|\n+yel+    |                 |\n+yel+    |      Recipe     |    +getObjProto(recVnum).name+\n+yel+    |                 |\n+yel+    |_________________|+nrm+\n";
	else if (sTest == "bL~tL")
		var newView = "     +yel+________ +nrm+________\n    +yel+|        |+nrm+        |\n    +yel+|        |+nrm+        |\n    +yel+|   +  Mix+|+nrm+        |    The left half of a recipe for:\n    +yel+|        |+nrm+        |\n+yel+    |        |+nrm+        |\n+yel+    |     Rec|+nrm+        |    +getObjProto(recVnum).name+\n+yel+    |        |+nrm+        |\n+yel+    |________|+nrm+________|\n";
	else if (sTest == "bL~tR")
		var newView = "     ________ +yel+________+nrm+\n    |        +yel+|        |+nrm+\n    |        +yel+|        |+nrm+\n    |        +yel+|+ing +    |+nrm+    Half of a recipe for:\n    |+yel+________|________|\n+yel+    |        |+nrm+        |\n+yel+    |     Rec|+nrm+        |    +getObjProto(recVnum).name+\n+yel+    |        |+nrm+        |\n+yel+    |________|+nrm+________|\n";
	else if (sTest == "bR~tL")
		var newView = "yel+     ________+nrm+ ________\n+yel+    |        |+nrm+        |\n+yel+    |        |+nrm+        |\n+yel+    |   +  Mix+|+nrm+        |    Half of a recipe for:\n+yel+    |________|________+nrm+|\n    |        +yel+|        |+nrm+\n    |        +yel+|ipe     |+nrm+    +getObjProto(recVnum).name+\n    |        +yel+|        |+nrm+\n    |________+yel+|________|+nrm+\n";
	else if (sTest == "bR~tR")
		var newView = "     ________+yel+ ________+nrm+\n    |        +yel+|        |+nrm+\n    |        +yel+|        |+nrm+\n    |        +yel+|+ing +    |+nrm+    The right half of a recipe for:\n    |        +yel+|        |+nrm+\n    |        +yel+|        |+nrm+\n    |        +yel+|ipe     |+nrm+    +getObjProto(recVnum).name+\n    |        +yel+|        |+nrm+\n    |________+yel+|________|+nrm+\n";
	else if (sTest == "tL~tR")
		var newView = "yel+     _________________\n+yel+    |                 |\n+yel+    |                 |\n+yel+    |   +  Mix+ing +     |+nrm+    The top half of a recipe for:\n+yel+    |_________________|+nrm+\n    |                 |\n    |                 |    +getObjProto(recVnum).name+\n    |                 |\n    |_________________|\n";
	else if (sTest == "bL~bR~tL")
		var newView = "     +yel+________+nrm+ ________\n    +yel+|        |+nrm+        |\n    +yel+|        |+nrm+        |\n    +yel+|   +  Mix+| +nrm+       |    All but the top right corner of a recipe for:\n    +yel+|        |________+nrm+|\n+yel+    |                 |\n+yel+    |      Recipe     |    +getObjProto(recVnum).name+\n+yel+    |                 |\n+yel+    |_________________|+nrm+\n";
	else if (sTest == "bL~bR~tR")
		var newView = "     ________ +yel+________+nrm+\n    |        +yel+|        |+nrm+\n    |        +yel+|        |+nrm+\n    |        +yel+|+ing +    |    +nrm+All but the top left corner of a recipe for:\n    |+yel+________|        |+nrm+\n+yel+    |                 |\n+yel+    |      Recipe     |    +getObjProto(recVnum).name+\n+yel+    |                 |\n+yel+    |_________________|+nrm+\n";
	else if (sTest == "bL~tL~tR")
		var newView = "yel+     _________________\n+yel+    |                 |\n+yel+    |                 |\n+yel+    |    +  Mix+ing +    |+nrm+    All but the bottom right corner of a recipe for:\n+yel+    |         ________|+nrm+\n    +yel+|        |+nrm+        |\n    +yel+|     Rec|+nrm+        |    +getObjProto(recVnum).name+\n    +yel+|        |+nrm+        |\n    +yel+|________|+nrm+________|\n";
	else if (sTest == "bR~tL~tR")
		var newView = "yel+     _________________\n+yel+    |                 |\n+yel+    |                 |\n+yel+    |    +  Mix+ing +    |+nrm+    All but the bottom left corner of a recipe for:\n+yel+    |________         |+nrm+\n    |        +yel+|        |+nrm+\n    |        +yel+|ipe     | +nrm+   +getObjProto(recVnum).name+\n    |        +yel+|        |+nrm+\n    |________+yel+|________|+nrm+\n";
	else if (sTest == "bL~bR~tL~tR")
		var newView = "yel+     _________________\n+yel+    |                 |\n+yel+    |                 |\n+yel+    |    +  Mix+ing +    |+nrm+    The recipe for:\n+yel+    |                 |+nrm+\n    +yel+|                 |+nrm+\n    +yel+|      Recipe     | +nrm+   +getObjProto(recVnum).name+\n    +yel+|                 |+nrm+\n    +yel+|_________________|+nrm+\n";
	var rString = "";
	newView = newView.split("+");
	for (i=0;i<newView.length;i++)
	{
		if (newView[i] == "cyn")
			newView[i] = cyn;
		if (newView[i] == "nrm")
			newView[i] = nrm;
		if (newView[i] == "yel") {
			if (rType == "M")
				newView[i] = yel;
			else if (rType == "S")
				newView[i] = mag;
			else if (rType == "T")
				newView[i] = grn;
		}
		if (newView[i] == "  Mix") {
			if (rType == "M")
				newView[i] = "  Mix";
			else if (rType == "S")
				newView[i] = " Smit";
			else if (rType == "T")
				newView[i] = "Tailo";
		}
		if (newView[i] == "ing ") {
			if (rType == "M")
				newView[i] = "ing ";
			else if (rType == "S")
				newView[i] = "hing";
			else if (rType == "T")
				newView[i] = "ring";
		}
		if (newView[i] == "getObjProto(recVnum).name")
			newView[i] = cyn+recipe.name+nrm;
	}
	for (i=0;i<newView.length;i++)
		rString += newView[i];
	if (strn_cmp(argArray[0],"examine",3) || strn_cmp(argArray[0],"read",3) || (strn_cmp(argArray[0],"look",1) && !argArray[2]))
	{
		if (self.carriedBy == actor)
		{
			var item = getObjInListVis(actor,argArray[1],actor.inventory);
			if (item != self)
			{
				_noblock;
				return;
			}
			_block;
			actor.send("When you look at the parchment, you see:");
			actor.send(" ");
			actor.send(rString);
			return;
		}
		else
		{
			var item = getObjInListVis(actor,argArray[1],actor.room.items);
			if (item != self)
			{
				_noblock;
				return;
			}
			_block;
			actor.send("When you look at the parchment, you see:");
			actor.send(" ");
			actor.send(rString);
			return;
		}
	}
	else if (strn_cmp(argArray[0],"look",1))
	{
		if (!argArray[2])
		{
			_noblock;
			return;
		}
		else if (strn_cmp(argArray[1],"in",1) || strn_cmp(argArray[1],"at",1))
		{
			if (self.carriedBy == actor)
			{
				var item = getObjInListVis(actor,argArray[2],actor.inventory);
				if (item != self)
				{
					_noblock;
					return;
				}
				_block;
				actor.send("When you look at the parchment, you see:");
				actor.send(" ");
				actor.send(rString);
				return;
			}
			else
			{
				var item = getObjInListVis(actor,argArray[2],actor.room.items);
				if (item != self)
				{
					_noblock;
					return;
				}	
				_block;
				actor.send("When you look at the parchment, you see:");
				actor.send(" ");
				actor.send(rString);
				return;
			}
		}
		else
		{
			_noblock;
			return;
		}
	}
	
}