//Boot OLC
var vOLC = [];
/**************************************************************************
 *                                                                        *
 * bootReceditOLC()                                                       *
 * Arguments: <None>                                                      *
 * Purpose:                                                               *
 * ~~~ By: Galnor 01/22/2010                                              *
 *                                                                        *
 **************************************************************************/
function bootReceditOLC()
{
	var oConfig = new Object();
	var mode;
	oConfig.type = "recedit";
	oConfig.modes = [];
	
	/**** Main Menu ****/
	mode = new Object();
	mode.mode = "MODE_MAIN";
	mode.parser = function(actor,fLetter,vArgs) {
		
		if( fLetter == "1" ) {
			actor.getOLC().recipeType = constants.RECIPE_TYPE_MIXING;
			actor.getOLC().switchToMode("MODE_BROWSE_RECIPES");
		}
		else if( fLetter == "2" ) {
			actor.getOLC().recipeType = constants.RECIPE_TYPE_SMITHING;
			actor.getOLC().switchToMode("MODE_BROWSE_RECIPES");
		}
		else if( fLetter == "3" ) {
			actor.getOLC().recipeType = constants.RECIPE_TYPE_TAILORING;
			actor.getOLC().switchToMode("MODE_BROWSE_RECIPES");
		}
		else if( fLetter == "Q" ) {
			actor.cleanupOLC();
		}
		else {
			actor.send("Invalid option.\nTry again: ");
		}
	}
	mode.display = function(actor) {
		delete actor.getOLC().recipeType;
		actor.send(bld + grn + "1" + nrm + ") Mixing Recipes");
		actor.send(bld + grn + "2" + nrm + ") Smithing Recipes");
		actor.send(bld + grn + "3" + nrm + ") Tailoring Recipes");
		actor.send("");
		actor.send(bld + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push( mode );
	
	/**** Mode Browse Recepes ****/
	mode = new Object();
	mode.mode = "MODE_BROWSE_RECIPES";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( isNumber(vArgs[0]) ) {
			var iTargetRecipeIndex = parseInt(vArgs[0]);
			var recipe = getRecipeByVnum( iTargetRecipeIndex, actor.getOLC().recipeType );
			if( !recipe )
				actor.send("The recipe you are trying to select is out of range.");
			else {
				actor.getOLC().recipe = recipe;
				actor.getOLC().switchToMode( "MODE_EDIT_RECIPE" );
			}
		}
		else if( fLetter == "A" ) {
			actor.getOLC().switchToMode( "MODE_ADD_VNUM");
		}
		else if( fLetter == "D" ) {
			actor.getOLC().switchToMode( "MODE_DELETE_VNUM");
		}
		else if( fLetter == "Q" ) {
			delete actor.getOLC().recipeType;
			actor.getOLC().switchToMode( "MODE_MAIN" );
		}
		else {
			actor.send("Invalid option.\nTry again: ");
		}
	};
	mode.display = function(actor)
	{
		getCharCols(actor);
		/*** Grab the recipes from OLC ***/
		var pBuffer = "";
		var vContainer = getRecipeContainer( actor.getOLC().recipeType );
		for(var recipeIndex = 0;recipeIndex < vContainer.length;++recipeIndex) {
			var recipe = vContainer[ recipeIndex ];
			var quan = 0;
			var recipeVnums = [];
			for (var _autoKey in recipe.ingredients) {
				var ing = recipe.ingredients[_autoKey];
				quan += ing.quantity;
				recipeVnums.push(ing.vnum);
			}
			pBuffer += bld + cyn + recipe.vnum + nrm + ") " + yel + recipe.name + nrm + " " + /*quan+" " + recipe.skillRequired+ " " + recipeVnums+ */"\n";
		}
		actor.send("~~~Recipe Editor~~~\n");
		actor.send( pBuffer );
		actor.send(bld + grn + "A" + nrm + ") Add Recipe");
		actor.send(bld + grn + "D" + nrm + ") Delete Recipe");
		actor.send(bld + grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push( mode );
	
	/**** Add Recipe Vnum ****/
	mode = new Object();
	mode.mode = "MODE_ADD_VNUM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		var vnum = parseInt(vArgs[0]);
		if( vnum <= 0 || !isNumber(vnum) )
			actor.send("Vnum must be above zero.");
		else if( getRecipeByVnum( vArgs[0], actor.getOLC().recipeType ) )
			actor.send("A recipe with that vnum already exists.");
		else {
			actor.getOLC().recipe = new Recipe(vnum,actor.getOLC().recipeType,"",[],[]);
			actor.getOLC().switchToMode( "MODE_EDIT_RECIPE");
		}
	};
	mode.display = function(actor) {
		actor.send("Enter the vnum for this new Recipe: ");
	}
	oConfig.modes.push( mode );
	
	/*** Delete Recipe ***/
	mode = new Object();
	mode.mode = "MODE_DELETE_VNUM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( !isNumber(vArgs[0]) ) {
			actor.send("Input must be numeric, or 'Q' to quit.\nTry again: ");
		}
		deleteRecipeByVnum( parseInt(vArgs[0]),actor.getOLC().recipeType );
		actor.getOLC().switchToMode( "MODE_BROWSE_RECIPES" );
	};
	mode.display = function(actor) {
		actor.send("Enter the vnum of the recipe you wish to delete, or 'Q' to quit: ");
	}
	oConfig.modes.push( mode );
	
	/*** Edit Recipe ***/
	mode = new Object();
	mode.mode = "MODE_EDIT_RECIPE";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( fLetter == "1" ) {
			actor.getOLC().switchToMode( "MODE_RECIPE_NAME" );
		}
		else if( fLetter == "2" ) {
			actor.getOLC().switchToMode( "MODE_RECIPE_SKILL_REQUIRED");
		}
		else if( fLetter == "3" ) {
			actor.getOLC().switchToMode( "MODE_RECIPE_SUCCESS_MESSAGE_ACTOR");
		}
		else if( fLetter == "4" ) {
			actor.getOLC().switchToMode( "MODE_RECIPE_SUCCESS_MESSAGE_ROOM");
		}
		else if( fLetter == "5" ) {
			actor.getOLC().switchToMode( "MODE_RECIPE_FAILURE_MESSAGE_ACTOR");
		}
		else if( fLetter == "6" ) {
			actor.getOLC().switchToMode( "MODE_RECIPE_FAILURE_MESSAGE_ROOM");
		}
		else if( fLetter == "7" ) {
			actor.getOLC().switchToMode( "MODE_RECIPE_PRICE");
		}
		else if( fLetter == "8" ) {
			actor.getOLC().switchToMode( "MODE_RECIPE_VENDORS");
		}
		else if( fLetter == "A" ) {
			actor.getOLC().switchToMode( "MODE_ADD_RECIPE_ITEM");
		}
		else if( fLetter == "E" ) {
			actor.getOLC().switchToMode( "MODE_EDIT_RECIPE_ITEM_VNUM");
		}
		else if( fLetter == "D" ) {
			actor.getOLC().switchToMode( "MODE_DELETE_RECIPE_ITEM");
		}
		else if( fLetter == "Q" ) {
			saveRecipe( actor.getOLC().recipe );
			actor.getOLC().switchToMode( "MODE_BROWSE_RECIPES");
		}
		else {
			actor.send("Invalid option.\nTry again: ");
		}
	}	
	mode.display = function(actor)
	{
		getCharCols(actor);
		var recipe = actor.getOLC().recipe;
		actor.send("~~~EDITING RECIPE #" + recipe.vnum + "~~~\n");
		var cnt = 1;
		for(var i = 0;i < recipe.ingredients.length;++i,++cnt) {
			var obj = getObjProto( recipe.ingredients[i].vnum );
			var sVnum = strPadding(recipe.ingredients[i].vnum ? recipe.ingredients[i].vnum : "<Undefined>", " ", 6, "left");
			actor.send( "  " + bld + red + cnt + nrm + ") [INGREDIENT] : " + "[" + cyn + sVnum + nrm + "] " + yel + (obj?obj.name:"<Invalid>") + nrm + " ("+recipe.ingredients[i].quantity+")");
		}
		for(var i = 0;i < recipe.results.length;++i) {
			var obj = getObjProto( recipe.results[i].vnum );
			var sVnum = strPadding(recipe.results[i].vnum, " ", 6, "left");
			actor.send( "  " + bld + red + cnt + nrm + ") [RESULT]     : " + "[" + cyn + sVnum + nrm + "] " + yel + (obj?obj.name:"<Invalid>") + nrm + " ("+recipe.results[i].quantity+")");
		}
		actor.send("");
		actor.send(bld + grn + "1" + nrm + ") Recipe Name: " + yel + (recipe.name && recipe.name.length ? recipe.name : "<None>") + nrm);
		actor.send(bld + grn + "2" + nrm + ") Skill Required: " + yel + (recipe.skillRequired ? recipe.skillRequired : "<Undefined>") + nrm);
		actor.send(bld + grn + "3" + nrm + ") Success Message (ACTOR): " + yel + (recipe.successMessageToActor ? recipe.successMessageToActor : "<Standard>") + nrm);
		actor.send(bld + grn + "4" + nrm + ") Success Message (ROOM) : " + yel + (recipe.successMessageToRoom ? recipe.successMessageToRoom : "<Standard>") + nrm);
		actor.send(bld + grn + "5" + nrm + ") Failure Message (ACTOR): " + yel + (recipe.failureMessageToActor ? recipe.failureMessageToActor : "<Standard>") + nrm);
		actor.send(bld + grn + "6" + nrm + ") Failure Message (ROOM) : " + yel + (recipe.failureMessageToRoom ? recipe.failureMessageToRoom : "<Standard>") + nrm);
		actor.send(bld + grn + "7" + nrm + ") Price: " + actor.numCopperToText(recipe.price,true));
		actor.send(bld + grn + "8" + nrm + ") Vendors: " + yel + recipe.vendors + nrm);
		actor.send(bld + grn + "A" + nrm + ") Add Item");
		actor.send(bld + grn + "E" + nrm + ") Edit Item");
		actor.send(bld + grn + "D" + nrm + ") Delete Item");
		actor.send(bld + grn + "Q" + nrm + ") Save and Quit");
	}
	oConfig.modes.push(mode);
	
	/*** Add Recipe Item ***/
	mode = new Object();
	mode.mode = "MODE_ADD_RECIPE_ITEM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( fLetter == "R" ) {
			var newItem = new Object();
			newItem.quantity = 1;
			actor.getOLC().recipe.results.push( newItem );
			actor.getOLC().recipeItem = newItem;
			actor.getOLC().switchToMode( "MODE_EDIT_RECIPE_ITEM");
		}
		else if( fLetter == "I" ) {
			var newItem = new Object();
			newItem.quantity = 1;
			actor.getOLC().recipe.ingredients.push( newItem );
			actor.getOLC().recipeItem = newItem;
			actor.getOLC().switchToMode( "MODE_EDIT_RECIPE_ITEM");
		}
		else {
			actor.send("Input must either be 'R' or 'I'\nTry again:");
		}
	}	
	mode.display = function(actor)
	{
		actor.send("Will this be an [I]ngredient, or a [R]esult?");
	}
	oConfig.modes.push(mode);
	
	/*** Edit Recipe Item ***/
	mode = new Object();
	mode.mode = "MODE_EDIT_RECIPE_ITEM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( fLetter == "1" ) {
			actor.getOLC().switchToMode( "MODE_EDIT_ITEM_VNUM");
		}
		else if( fLetter == "2" ) {
			actor.getOLC().switchToMode( "MODE_EDIT_ITEM_QUANTITY");
		}
		else if( fLetter == "Q" ) {
			actor.getOLC().switchToMode( "MODE_EDIT_RECIPE");
		}
		else {
			actor.send("Invalid option.\nTry again: ");
		}
	}	
	mode.display = function(actor)
	{
		actor.send(bld + grn + "1" + nrm + ") Item Vnum : " + yel + actor.getOLC().recipeItem.vnum);
		actor.send(bld + grn + "2" + nrm + ") Quantity  : " + yel + actor.getOLC().recipeItem.quantity);
		actor.send(bld + grn + "Q" + nrm + ") Save and Quit");
	}
	oConfig.modes.push(mode);
	
	/*** Edit Recipe Item Vnum ***/
	mode = new Object();
	mode.mode = "MODE_EDIT_ITEM_VNUM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		actor.getOLC().recipeItem.vnum = parseInt(vArgs[0]);
		actor.getOLC().switchToMode( "MODE_EDIT_RECIPE_ITEM");
	}	
	mode.display = function(actor)
	{
		actor.send("Enter the new item vnum: ");
	}
	oConfig.modes.push(mode);
	
	/*** Edit Recipe Item Quantity ***/
	mode = new Object();
	mode.mode = "MODE_EDIT_ITEM_QUANTITY";
	mode.parser = function(actor,fLetter,vArgs)
	{
		actor.getOLC().recipeItem.quantity = parseInt(vArgs[0]);
		actor.getOLC().switchToMode( "MODE_EDIT_RECIPE_ITEM");
	}
	mode.display = function(actor)
	{
		actor.send("Enter the new item quantity: ");
	}
	oConfig.modes.push(mode);
	
	/*** Edit Recipe Item Select Vnum ***/
	mode = new Object();
	mode.mode = "MODE_EDIT_RECIPE_ITEM_VNUM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( fLetter == "Q" ) {
			actor.getOLC().switchToMode( "MODE_EDIT_RECIPE");
			return;
		}
		else if( isNumber(vArgs[0]) ) {
			var nr = parseInt(vArgs[0]);
			var olc = actor.getOLC();
			if( nr > 0 && nr <= olc.recipe.ingredients.length ) {
				olc.recipeItem = olc.recipe.ingredients[nr-1];
				olc.switchToMode( "MODE_EDIT_RECIPE_ITEM");
				return;
			}
			nr-=olc.recipe.ingredients.length;
			if( nr > 0 && nr <= olc.recipe.results.length ) {
				olc.recipeItem = olc.recipe.results[nr-1];
				olc.switchToMode( "MODE_EDIT_RECIPE_ITEM");
				return;
			}
			actor.send("The number you entered is out of range. Try again, or type 'Q' to quit.");
			return;
		}
		actor.send("Invalid option. Enter the item number from the list, or Q to quit: ");
	}
	mode.display = function(actor)
	{
		actor.send("Which item do you wish to edit(or Q to Quit)? ");
	}
	oConfig.modes.push(mode);
	
	/*** Delete Recipe Item ***/
	mode = new Object();
	mode.mode = "MODE_DELETE_RECIPE_ITEM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if( fLetter == "Q" ) {
			actor.getOLC().switchToMode( "MODE_EDIT_RECIPE");
			return;
		}
		else if( isNumber(vArgs[0]) ) {
			var nr = parseInt(vArgs[0]);
			var olc = actor.getOLC();
			if( nr > 0 && nr <= olc.recipe.ingredients.length ) {
				olc.recipe.ingredients.splice( nr-1, 1 );
				olc.switchToMode( "MODE_EDIT_RECIPE");
				return;
			}
			nr-=olc.recipe.ingredients.length;
			if( nr > 0 && nr <= olc.recipe.results.length ) {
				olc.recipe.results.splice( nr-1, 1 );
				olc.switchToMode( "MODE_EDIT_RECIPE");
				return;
			}
			actor.send("The number you entered is out of range. Try again, or type 'Q' to quit.");
			return;
		}
		actor.send("Invalid option. Enter the item number from the list, or Q to quit: ");
	}
	mode.display = function(actor)
	{
		actor.send("Which item do you wish to delete(or Q to Quit)? ");
	}
	oConfig.modes.push(mode);
	
	/*** Recipe Name ***/
	mode = new Object();
	mode.mode = "MODE_RECIPE_NAME";
	mode.parser = function(actor,fLetter,vArgs)
	{
		actor.getOLC().recipe.name = vArgs.join(" ");
		actor.getOLC().switchToMode( "MODE_EDIT_RECIPE" );
	}
	mode.display = function(actor)
	{
		actor.send("Enter the new name for this recipe : ");
	}
	oConfig.modes.push(mode);
	
	/*** Recipe Skill Required ***/
	mode = new Object();
	mode.mode = "MODE_RECIPE_SKILL_REQUIRED";
	mode.parser = function(actor,fLetter,vArgs)
	{
		var nr = parseInt( vArgs[0] );
		if( isNaN(nr) || nr < 0 || nr > 100 ) {
			actor.send("Your input must be between 0 and 100 : ");
			return;
		}
		actor.getOLC().recipe.skillRequired = nr;
		actor.getOLC().switchToMode( "MODE_EDIT_RECIPE" );
	}
	mode.display = function(actor)
	{
		actor.send("Enter the skill required to create this recipe : ");
	}
	oConfig.modes.push(mode);
	
	/*** Recipe Success Message ***/
	mode = new Object();
	mode.mode = "MODE_RECIPE_SUCCESS_MESSAGE_ACTOR";
	mode.parser = function(actor,fLetter,vArgs)
	{
		actor.getOLC().recipe.successMessageToActor = vArgs.join(" ");
		actor.getOLC().switchToMode( "MODE_EDIT_RECIPE" );
	}
	mode.display = function(actor)
	{
		actor.send("Enter the message that the user should see when they succeed : ");
	}
	oConfig.modes.push(mode);
	
	/*** Recipe Success Message To Room ***/
	mode = new Object();
	mode.mode = "MODE_RECIPE_SUCCESS_MESSAGE_ROOM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		actor.getOLC().recipe.successMessageToRoom = vArgs.join(" ");
		actor.getOLC().switchToMode( "MODE_EDIT_RECIPE" );
	}
	mode.display = function(actor)
	{
		actor.send("Enter the message that the room should see when user succeeds : ");
	}
	oConfig.modes.push(mode);
	
	/*** Recipe Failure Message To Actor ***/
	mode = new Object();
	mode.mode = "MODE_RECIPE_FAILURE_MESSAGE_ACTOR";
	mode.parser = function(actor,fLetter,vArgs)
	{
		actor.getOLC().recipe.failureMessageToActor = vArgs.join(" ");
		actor.getOLC().switchToMode( "MODE_EDIT_RECIPE" );
	}
	mode.display = function(actor)
	{
		actor.send("Enter the message that the user should see when they fail : ");
	}
	oConfig.modes.push(mode);
	
	/*** Recipe Failure Message To Room ***/
	mode = new Object();
	mode.mode = "MODE_RECIPE_FAILURE_MESSAGE_ROOM";
	mode.parser = function(actor,fLetter,vArgs)
	{
		actor.getOLC().recipe.failureMessageToRoom = vArgs.join(" ");
		actor.getOLC().switchToMode( "MODE_EDIT_RECIPE" );
	}
	mode.display = function(actor)
	{
		actor.send("Enter the message that the room should see when user fails : ");
	}
	oConfig.modes.push(mode);
	
	/*** Recipe Price ***/
	mode = new Object();
	mode.mode = "MODE_RECIPE_PRICE";
	mode.parser = function(actor,fLetter,vArgs)
	{
		var price = parseInt(vArgs[0]);
		if(price < 0) {
			actor.send("The price must be above zero.\r\nTry again : ");
			return;
		}
		actor.getOLC().recipe.price = price;
		actor.getOLC().switchToMode("MODE_EDIT_RECIPE");
	}
	mode.display = function(actor)
	{
		actor.send("Enter the new price for this recipe : ");
	}
	oConfig.modes.push(mode);
	
	/*** Recipe Vendors ***/
	
	mode = new Object();
	mode.mode = "MODE_RECIPE_VENDORS";
	mode.parser = function(actor,fLetter,vArgs)
	{
		if(fLetter == "Q") {
			actor.getOLC().switchToMode("MODE_EDIT_RECIPE");
			return;
		}
		else if(fLetter == "A") {
			actor.getOLC().switchToMode("MODE_RECIPE_ADD_VENDOR");
			return;
		}
		else if(fLetter == "D") {
			actor.getOLC().switchToMode("MODE_RECIPE_DELETE_VENDOR");
			return;
		}
		else {
			actor.send("Invalid option. Try again.");
			return;
		}
	}
	mode.display = function(actor)
	{
		var vendors = actor.getOLC().recipe.vendors;
		if (vendors) {
			for(var i = 1;i <= vendors.length;++i) {
				var index = i-1;
				var vendorVnum = vendors[index];
				var mobProto = getMobProto(vendorVnum);
				actor.send( grn + strPadding(i+"", " ", 2, "left") + nrm + ") " + "[" + cyn + strPadding(vendorVnum+"", " ", 6, "left") + nrm + "] " +
							yel + (mobProto ? mobProto.name : "<Illegal>") + nrm);
			}
		}
		actor.send("");
		actor.send(grn + "A" + nrm + ") Add Vendor");
		actor.send(grn + "D" + nrm + ") Delete Vendor");
		actor.send(grn + "Q" + nrm + ") Quit");
	}
	oConfig.modes.push(mode);
	
	/*** Add Vendor ***/
	mode = new Object();
	mode.mode = "MODE_RECIPE_ADD_VENDOR";
	mode.parser = function(actor,fLetter,vArgs) {
		var recipe = actor.getOLC().recipe;
		var vendors = recipe.vendors;
		
		if( fLetter == 'Q' ) {
			actor.getOLC().switchToMode("MODE_RECIPE_VENDORS");
			return;
		}
		if( !isNumber(vArgs[0]) ) {
			actor.send("Input must be numeric, or 'Q' to quit.");
			return;
		}
		var vendorVnum = parseInt(vArgs[0]);
		
		if(arrContains(vendors,vendorVnum) == true) {
			actor.send("That vendor is already listed. Enter another one, or enter 'Q' to quit.");
			return;
		}
		var mobProto = getMobProto(vendorVnum);
		
		if(!mobProto) {
			actor.send("No mob exists with that vnum. Try again, or enter 'Q' to quit.");
			return;
		}
		if (vendors)
			vendors.push(vendorVnum);
		actor.getOLC().switchToMode("MODE_RECIPE_VENDORS");
	}
	mode.display = function(actor) {
		actor.send("Enter the vnum of the vendor you wish to add(or Q to quit) : ");
	}
	oConfig.modes.push(mode);
	
	/*** Add Vendor ***/
	mode = new Object();
	mode.mode = "MODE_RECIPE_DELETE_VENDOR";
	mode.parser = function(actor,fLetter,vArgs) {
		var recipe = actor.getOLC().recipe;
		var vendors = recipe.vendors;
		
		if( fLetter == 'Q' ) {
			actor.getOLC().switchToMode("MODE_RECIPE_VENDORS");
			return;
		}
		if( !isNumber(vArgs[0]) ) {
			actor.send("Input must be numeric, or 'Q' to quit.");
			return;
		}
		var index = parseInt(vArgs[0]) - 1;
		
		if( index < 0 || index >= vendors.length ) {
			actor.send("Invalid vendor selected.");
		}
		else {
			vendors.splice(index,1);
		}
		actor.getOLC().switchToMode("MODE_RECIPE_VENDORS");
	}
	mode.display = function(actor) {
		actor.send("Enter the vnum of the vendor you wish to add(or Q to quit) : ");
	}
	oConfig.modes.push(mode);
	
	vOLC.push( oConfig );
}
bootOLC();

