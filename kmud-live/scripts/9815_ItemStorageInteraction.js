var script9815 = function(self, actor, here, args, extra)
{
	// Alder
// June 2011
/****************************************
 *       Item Storage Interaction       *
 ****************************************/
if ( actor.getPval("storageSlots") == null || actor.race != self.race ) {
	return;
}
var vArgs = getArgList(args);
var storage = strn_cmp(vArgs[0],"storage",3);
var account = strn_cmp(vArgs[0],"account",4);
var collect = strn_cmp(vArgs[0],"collect",4);
var offer 	= strn_cmp(vArgs[0],"offer",3);
// Getting information about storage system
if ( storage ) {
	_block;
	self.tell(actor,"Managing your storage account is simple: just give me the item you want to deposit for safekeeping, and I charge you the gold up front.");
	self.tell(actor,"To view the items currently stored under your name, just use the -account- command at any banker.");
	self.tell(actor,"If you'd like to collect one of your items, simply type: collect <item #>.");
	self.tell(actor,"You can also offer an item for storage to see how much it will cost. Just type: offer <item name>.");
	return;
}
// Viewing items currently stored
if ( account ) {
	_block;
	var account = actor.getStoredItems();
	getCharCols(actor);
	actor.send("+----------------+");
	actor.send("|  "+bld+"Stored Items"+nrm+"  |");
	actor.send(""+strPadding("","-",50,"right"));
	var numItems = account.length;
	if ( numItems < 10 )
		padVal = 2;
	else if ( numItems < 100 )
		padVal = 3;
	for ( var i = 0; i < numItems; i++ ) {
		actor.send(strPadding(grn+(i+1)+nrm," ",padVal,"left")+") "+account[i].name);
		account[i].extract();
	}
	var slots = actor.countEmptyStorageSlots();
	var s = "s";
	if ( slots == 1 ) {
		s = "";
	}
	actor.send(cyn+"\n You have "+intToText(slots)+" available storage slot"+s+"."+nrm);
	actor.send(strPadding("","-",50,"right"));
	self.tell(actor, "To take an item out of storage, type: collect <item #>.");
	return;
}
// Collecting a stored item
if ( collect ) {
	_block;
	var itemNumber = parseInt(vArgs[1]);
	// Arg is NaN
	if ( !isNumber(itemNumber) || !vArgs[1] ) {
		self.tell(actor, "Indicate which item you'd like to collect by the number displayed beside it.");
		return;
	}
	var account = actor.getStoredItems();
	// Arg is out of bounds
	if ( itemNumber < 0 || itemNumber > account.length ) {
		self.tell(actor, "I didn't see that item anywhere, did you mean to say something else?");
		return;
	}
	var item = account[itemNumber-1];
	item.moveToChar(self);
	for each ( var obj in account ) {
		if ( obj.id != item.id ) {
			obj.extract();
		}
	}
	self.tell(actor, "I'll get that right out for you.");
	waitpulse 5;
	act("$n gives you $p.",true,self,item,actor,constants.TO_VICT);
	act("$n gives $N $p.",true,self,item,actor,constants.TO_NOTVICT);
	item.moveToChar(actor);
	actor.setPval("storageSlots",actor.countEmptyStorageSlots()+1,true);
	self.tell(actor, "There's your item. Come back any time.");
	actor.comm("save");
	return;
}
// Assessing the cost to store a certain item in inventory
if ( offer ) {
	_block;
	var alias = vArgs[1];
	if ( !alias ) {
		self.tell(actor, "You must offer an item from your inventory to get a storage price.");
		return;
	}
	var item = getObjInListVis(actor,alias,actor.inventory);
	if ( !item ) {
		self.tell(actor, "You don't appear to have one of those.");
		return;
	}
	self.tell(actor, "Let me take a look...");
	var cost = getStoragePrice(item);
	var str = "";
	if ( cost == -1 ) {
		str = "That is not an item.";
	}
	else if ( cost == -2 ) {
		str = "You must first remove the contents of "+item.name+" before I can give you a price.";
	}
	else if ( cost == -3 ) {
		str = "You cannot store "+item.name+".";
	}
	else {
		str = "It will cost "+cost+" copper to store "+item.name+".";
	}
	self.tell(actor,str);
	return;
}
	
	

};

