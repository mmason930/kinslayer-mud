var script3180 = function(self, actor, here, args, extra)
{
	// Alder
// 17 January 2011
// Recipe Mart Intro \\
if ( self.race != actor.race ) {
	_block;
	actor.send("What?");
	return;
}
getCharCols(actor);
var vArgs = getArgList(args);
var cmd = vArgs[0];
var myAll = getVendorRecipes(self.vnum); // Array of all recipes available with vendor
var buying = strn_cmp(cmd,"buy",3);
var viewing = strn_cmp(cmd,"browse",2);
/**IF BROWSING RECIPES**/
if ( viewing ) {
	_block;
	self.tell(actor,"Here are the recipes I have.");
	actor.send(strPadding("","-",88,"left"));
	var recipeCount = myAll.length;
	var padVal = 1;	// padVal allows for numbers to be aligned depending on highest digit
	if ( recipeCount < 10 )
		padVal = 2;
	else if ( recipeCount < 100 )
		padVal = 3;
	else
		padVal = 4;
	/*Show the recipes in a list*/
	for ( var i = 0; i < recipeCount; i++ ) {
		var recipe = myAll[i];
		var name = (recipe.results.length == 1 ? capFirstLetter(getObjProto(recipe.results[0].vnum).name) : recipe.name);		// Name of recipe (or single result)
		var type = grn+"MIXING"+nrm;		// Spelled out recipe type
		if ( !str_cmp( recipe.type, constants.RECIPE_TYPE_SMITHING ) ) {
			type = cyn+"SMITHING"+nrm;
		}
		else if ( !str_cmp( recipe.type, constants.RECIPE_TYPE_TAILORING ) ) {
			type = yel+"TAILORING"+nrm;
		}
		var learned = ( actor.isRecipeUnlocked(recipe) ? red+bld+"LEARNED"+nrm : "" );
		var price = recipe.price;
		var strCost = actor.numCopperToText(recipe.price,true,true);
		var arrCost = strCost.split(", ");
			var goldArr = arrCost[0].split(" ");	goldArr.shift();
				var goldStr = strPadding(goldArr[0]," ",4,"left") + " " + goldArr[1];
			var silverArr = arrCost[1].split(" ");
				var silverStr = strPadding(silverArr[0],"0",3,"left") + " " + silverArr[1];
			var copperArr = arrCost[2].split(" ");
				var copperStr = strPadding(copperArr[0],"0",3,"left") + " " + copperArr[1];
		var listPrice = (goldStr+", "+silverStr+", "+copperStr);
		actor.send( strPadding(cyn+(i+1)+nrm," ",padVal,"left")+") "+strPadding(name," ",36,"right")+"-  "+strPadding((learned.length ? learned : type)," ",10,"right")+" - "+listPrice+" "+"\n" );
	}
	actor.send(strPadding("","-",88,"left"));
	self.tell(actor,"Just tell me the tag number of the one you'd like to buy.");
}
/**IF BUYING RECIPE**/
else if ( buying ) {
	if ( !isNumber(vArgs[1]) ) {
		_noblock;
		return;
	}
	_block;
	// Invalid second argument
	if ( vArgs.length < 2 ) {
		self.tell(actor,"I'm afraid I don't stock that.");
		return;
	}
	var choice = vArgs[1];
	// Numerical argument is outside recipe array boundaries
	if ( choice > myAll.length ) {
		self.tell(actor,"I don't recall seeing that one in stock...");
		return;
	}
	var purchase = myAll[choice-1];
	// Player already knows selected recipe
	if ( actor.isRecipeUnlocked(purchase) ) {
		self.tell(actor,"Looks like you already know that one.");
		return;
	}
	// Insufficient funds
	if ( actor.gold < purchase.price ) {
		self.tell(actor,"Hate to break it to you, but you can't afford that.");
		return;
	}
	// Sufficient funds
	self.tell(actor,"That'll be"+actor.numCopperToText(purchase.price,false)+".");
	actor.gold -= purchase.price;
	act("You hand the coins to $N.",true,actor,null,self,constants.TO_CHAR);
	act("$n hands some coins to $N.",true,actor,null,self,constants.TO_ROOM);
	
	actor.unlockRecipe(purchase.vnum, purchase.type);
}

};

