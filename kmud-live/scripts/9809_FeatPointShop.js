var script9809 = function(self, actor, here, args, extra)
{
	// Alder
// February 2011
/****************************************
 *         Feat points shop             *
 ****************************************/
if ( self.race != actor.race ) {
	_noblock;
	return;
}
var vArgs = getArgList(args);
var browse = strn_cmp(vArgs[0],"browse",2);
var buy = strn_cmp(vArgs[0],"buy",3);
var itemList = [];	// Array of items available for purchase
// Generic code for retool purchases
		var retoolCode = "";
		retoolCode += "var retool = new Object();";
		retoolCode += 'retool.name = "";';
		retoolCode += 'retool.roomDesc = "";';
		retoolCode += 'retool.aliases = "";';
		retoolCode += 'actor.featRetool = retool;';
		retoolCode += 'self.tell(actor, "I just need to know how you want to change your item. The three attributes are: name, aliases, roomDesc.");';
		retoolCode += 'self.tell(actor, "To set the information for an attribute, just tell me the attribute followed by the information.");';
		retoolCode += 'self.tell(actor, "Here\'s an example: roomDesc A fancy new item sits here.");';
		retoolCode += 'self.tell(actor, "Once your information is set, give me the item you want to retool and I will charge you and return it. If you rent before retooling an item, your retool information will be lost. You may only attempt to retool one type of item at a time. If you mess up one of your attributes, just tell me the information again so I can get it right.");';
/**************************************************************
 * Each item available for purchase has several properties:   *
 * name: string representation of the item                    *
 * cost: number of feat points required to purchase           *
 * code: string of JS code to execute upon purchase of item   *
 **************************************************************/
/**
	Bracket title
				   **/
var newItem = new Object();
	newItem.name = "Custom bracket title";
	newItem.cost = 8;
	newItem.code = "self.loadObj(9810); self.comm('give placard '+actor.name); self.tell(actor,'-Write- your title on that and hand it back to me.'); self.tell(actor,\"If you mess up just -write- over what's already written on the placard.\");";
	itemList.push( newItem );
/**
	Prefix title
				   **/
// var newItem = new Object();
	// newItem.name = "Custom prefix title";
	// newItem.cost = 15;
	// newItem.code = "self.loadObj(9809); self.comm('give placard '+actor.name); self.tell(actor,'-Write- your title on that and hand it back to me.');";
	// itemList.push( newItem );
/**
	Storage Account
				   **/
if ( actor.getPval("storageSlots") == null ) {
	var newItem = new Object();
		newItem.name = "Item Storage Permit";
		newItem.cost = 45;
		newItem.code = "actor.setPval('storageSlots',5,true); self.tell(actor, \"You can now store up to \"+intToText(actor.getPval('storageSlots'))+\" items with your new storage permit. See your local banker about -storage- and he'll establish your account.\");";
		itemList.push( newItem );
}
/**
	Retool HEAD
				   **/
var newItem = new Object();
	newItem.name = "Head-piece retool";
	newItem.cost = 11;
	newItem.charge = false;
	newCode = retoolCode;
	newCode += 'retool.type = constants.ITEM_WEAR_HEAD;';
	newCode += 'retool.cost = '+newItem.cost+';';
	newItem.code = newCode;
	itemList.push( newItem );
/**
	Retool BODY
				   **/
var newItem = new Object();
	newItem.name = "Body-piece retool";
	newItem.cost = 12;
	newItem.charge = false;
	newCode = retoolCode;
	newCode += 'retool.type = constants.ITEM_WEAR_BODY;';
	newCode += 'retool.cost = '+newItem.cost+';';
	newItem.code = newCode;
	itemList.push( newItem );
/**
	Retool LEGS
				   **/
var newItem = new Object();
	newItem.name = "Leg-piece retool";
	newItem.cost = 10;
	newItem.charge = false;
	newCode = retoolCode;
	newCode += 'retool.type = constants.ITEM_WEAR_LEGS;';
	newCode += 'retool.cost = '+newItem.cost+';';
	newItem.code = newCode;
	itemList.push( newItem );
/**
	Retool FEET
				   **/
var newItem = new Object();
	newItem.name = "Footwear retool";
	newItem.cost = 9;
	newItem.charge = false;
	newCode = retoolCode;
	newCode += 'retool.type = constants.ITEM_WEAR_FEET;';
	newCode += 'retool.cost = '+newItem.cost+';';
	newItem.code = newCode;
	itemList.push( newItem );
/**
	Retool HANDS
				   **/
var newItem = new Object();
	newItem.name = "Glove retool";
	newItem.cost = 9;
	newItem.charge = false;
	newCode = retoolCode;
	newCode += 'retool.type = constants.ITEM_WEAR_HANDS;';
	newCode += 'retool.cost = '+newItem.cost+';';
	newItem.code = newCode;
	itemList.push( newItem );
/**
	Retool WAIST
				   **/
var newItem = new Object();
	newItem.name = "Belt retool";
	newItem.cost = 8;
	newItem.charge = false;
	newCode = retoolCode;
	newCode += 'retool.type = constants.ITEM_WEAR_WAIST;';
	newCode += 'retool.cost = '+newItem.cost+';';
	newItem.code = newCode;
	itemList.push( newItem );
/**
	Retool SHOULDERS
				   **/
var newItem = new Object();
	newItem.name = "Shoulder retool";
	newItem.cost = 8;
	newItem.charge = false;
	newCode = retoolCode;
	newCode += 'retool.type = constants.ITEM_WEAR_SHOULDERS;';
	newCode += 'retool.cost = '+newItem.cost+';';
	newItem.code = newCode;
	itemList.push( newItem );
/**
	Retool ARMS
				   **/
var newItem = new Object();
	newItem.name = "Arm-piece retool";
	newItem.cost = 8;
	newItem.charge = false;
	newCode = retoolCode;
	newCode += 'retool.type = constants.ITEM_WEAR_ARMS;';
	newCode += 'retool.cost = '+newItem.cost+';';
	newItem.code = newCode;
	itemList.push( newItem );
/**
	Retool SHIELD
				   **/
var newItem = new Object();
	newItem.name = "Shield retool";
	newItem.cost = 18;
	newItem.charge = false;
	newCode = retoolCode;
	newCode += 'retool.type = constants.ITEM_WEAR_SHIELD;';
	newCode += 'retool.cost = '+newItem.cost+';';
	newItem.code = newCode;
	itemList.push( newItem );
/**
	Retool WEAPON
				   **/
var newItem = new Object();
	newItem.name = "Weapon retool";
	newItem.cost = 25;
	newItem.charge = false;
	newCode = retoolCode;
	newCode += 'retool.type = constants.ITEM_WEAR_WIELD;';
	newCode += 'retool.cost = '+newItem.cost+';';
	newItem.code = newCode;
	itemList.push( newItem );
/**
	Next Item
				   **/
function itemSort(a, b) {
	return a.cost > b.cost;
}
itemList.sort(itemSort);
// If browsing stock
if ( browse ) {
	_block;
	self.tell(actor,"You can -buy- any of these items with your feat points.");
	getCharCols(actor);
	actor.send("+--------------------+");
	actor.send("|  "+bld+"Feat Points Shop"+nrm+"  |");
	actor.send(""+strPadding("","-",50,"right"));
	var numItems = itemList.length;
	if ( numItems < 10 )
		padVal = 2;
	else if ( numItems < 100 )
		padVal = 3;
	else
		padVal = 4;
	for ( var i = 0; i < numItems; i++ ) {
		var item = itemList[i];
		actor.send(strPadding(cyn+(i+1)+nrm," ",padVal,"left") + ") " + bld+strPadding(item.name," ",23,"right")+nrm+"--  " + (actor.featPoints < item.cost ? red : grn) + item.cost+nrm+ " feat points");
	}
	actor.send(strPadding("","-",50,"right"));
	actor.send("Your feat points: "+grn+actor.featPoints+nrm+"\n");
	self.tell(actor,"Make sure to indicate the item number of your purchase.");
}
// If buying item
if ( buy ) {
	_block;
	if ( vArgs.length > 1 ) {
		var choice = vArgs[1];
		if ( !isNumber(choice) ) {
			self.tell(actor,"You need to tell me the item number you want to buy.");
			return;
		}
		if ( choice < 1 || choice > itemList.length ) {
			self.tell(actor,"I don't see that one in stock...");
			return;
		}
		var item = itemList[choice-1];
		if ( item.cost > actor.featPoints ) {
			self.tell(actor,"You haven't collected enough feat points to buy that.");
			return;
		}
		self.tell(actor,"Great! "+capFirstLetter(item.name)+" is all yours.");
		actor.addFeatPoints( item.charge == false && item.charge != undefined ? 0 : -item.cost );
		eval(item.code);
		return;
	}
	else {
		self.tell(actor,"Well WHICH one are you gonna buy? You've gotta tell me the number.");
		return
	}
}

};

