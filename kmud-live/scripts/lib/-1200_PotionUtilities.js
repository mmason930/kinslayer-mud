var RECIPE_INDEX_TABLE = "recipe";
var RECIPE_ITEM_TABLE = "recipeItem";
var RECIPE_VENDOR_TABLE = "recipeVendor";
constants.RECIPE_TYPE_MIXING = "M";
constants.RECIPE_TYPE_SMITHING = "S";
constants.RECIPE_TYPE_TAILORING = "T";
function Recipe( vnum, type, name, ingredients, results, skill, successMessageToActor, successMessageToRoom, failureMessageToActor, failureMessageToRoom, price, vendors )
{
	this.vnum = vnum;
	this.type = type;
	this.name = name;
	this.ingredients = ingredients;
	this.results = results;
	this.skillRequired = skill;
	this.successMessageToActor = successMessageToActor;
	this.successMessageToRoom = successMessageToRoom;
	this.failureMessageToActor = failureMessageToActor;
	this.failureMessageToRoom = failureMessageToRoom;
	this.price = price;
	this.vendors = vendors;
}
function getRecipeByVnum( vnum, type )
{
	var vContainer = getRecipeContainer( type );
	if( !vContainer ) {
		mudLog(constants.NRM, 102, "getRecipeByVnum() : Attempting to obtain a recipe of an invalid type '" + type + "'");
		return;
	}
	var recipeIndex = binarySearch(vContainer,vnum,function(r){return r.vnum;});
	if( recipeIndex != -1 )
		return vContainer[ recipeIndex ];
	return null;
//	for(var recipeIndex = 0;recipeIndex < vContainer.length;++recipeIndex)
//	{
//		if( vContainer[ recipeIndex ].vnum == vnum )
//			return vContainer[ recipeIndex ];
//	}
}
// function getRecipeByName( name, type )
// {
	// var vContainer = getRecipeContainer( type );
	// if( !vContainer ) {
		// mudLog(constants.NRM, 102, "getRecipeByVnum() : Attempting to obtain a recipe of an invalid type '" + type + "'");
		// return;
	// }
	// var recipeIndex = binarySearch(vContainer,name,function(r){return r.name;});
	// if( recipeIndex != -1 )
		// return vContainer[ recipeIndex ];
	// return null;
// }
function bootRecipe( recipeRow, vItems, vVendors )
{
	var name = recipeRow.get("name");
	var type = recipeRow.get("type");
	var skill = parseInt(recipeRow.get("skill_required"));
	var successMessageToActor = recipeRow.get("success_message_actor");
	var successMessageToRoom = recipeRow.get("success_message_room");
	var failureMessageToActor = recipeRow.get("failure_message_actor");
	var failureMessageToRoom = recipeRow.get("failure_message_room");
	var vnum = parseInt(recipeRow.get("id"));
	var price = parseInt(recipeRow.get("price"));
	var ingredients = [];
	var results = [];
	var vendors = [];
	
	for(var i = 0;i < vItems.length;++i) {
		var row = vItems[i];
		var item = new Object();
		item.vnum = parseInt(row.get("object_id"));
		item.quantity = parseInt(row.get("quantity"));
		if( row.get("type") == "INGRED" )
			ingredients.push( item );
		else if( row.get("type") == "RESULT" )
			results.push( item );
	}
	for(var i = 0;i < vVendors.length;++i) {
		var row = vVendors[i];
		var vendorVnum = parseInt(row.get("vendor_vnum"));
		vendors.push( vendorVnum );
	}
	return new Recipe(vnum, type, name, ingredients, results, skill, successMessageToActor, successMessageToRoom, failureMessageToActor, failureMessageToRoom, price, vendors);
}
function addRecipe( recipe )
{
	var vContainer = getRecipeContainer( recipe.type );
	if( !vContainer ) {
		mudLog(constants.NRM, 102, "addRecipe() : Attempting to add a recipe of an invalid type '" + recipe.type + "'");
		return;
	}
	sortedInsert( vContainer, recipe, function(e){return e.vnum;} );
}
function getBestMatchingRecipe( container, recipeType )
{
	var vContainer = getRecipeContainer( recipeType );
	for(var p = 0;p < vContainer.length;++p) {
	//For every recipe...
		var pass = true;
		var ing_pass = [];
		if( vContainer[p].ingredients.length == 0 )
			continue;
		for(var i = 0;i < vContainer[p].ingredients.length;++i) {
		//For every ingredient...
			if( container.getItems( vContainer[p].ingredients[i].vnum ).length < vContainer[p].ingredients[i].quantity || container.getItems( vContainer[p].ingredients[i].vnum ).length > vContainer[p].ingredients[i].quantity)
			{
				pass = false;
				break;
			}
		}
		for (c = 0; c < container.contents.length; c++) {
		//For every item in the container...
			ing_pass.push(c);
			for (l = 0; l < vContainer[p].ingredients.length; l++) {
			//For each ingredient...
				if (container.contents[c].vnum == vContainer[p].ingredients[l].vnum) {
					//container.carriedBy.room.echo("setting content "+c+" to true");
					ing_pass[c] = true;
					break;
				}
			}
		}
		var ing_check = true;
		//container.carriedBy.room.echo(ing_pass);
		for (g = 0; g < ing_pass.length; g++)
		{
			if (ing_pass[g] != true) {
				ing_check = false;
				//container.carriedBy.room.echo(ing_pass);
				break;
			}
		}
		if( pass && ing_check == true)
		{
			//container.carriedBy.room.echo(vContainer[p].name);
			return vContainer[p];
		}
	}
	return null;//No good matches.
}
function saveRecipeItem( vnum, recipeType, item, type )
{
	var sql = "";
	sql += "INSERT INTO " + RECIPE_ITEM_TABLE + "(";
	sql += "`recipe_id`,";
	sql += "`recipe_type`,";
	sql += "`type`,";
	sql += "`object_id`,";
	sql += "`quantity`)";
	sql +=" VALUES(";
	sql += "'" + vnum + "',";
	sql += "'" + recipeType + "',";
	sql += "'" + type + "',";
	sql += "'" + item.vnum + "',";
	sql += "'" + item.quantity + "');";
	
	sqlQuery( sql );
}
function saveRecipeVendor( vnum, recipeType, vendorVnum ) {
	var sql = "INSERT INTO " + RECIPE_VENDOR_TABLE + "("
	        + " `recipe_id`,"
			+ " `recipe_type`,"
			+ " `vendor_vnum`"
			+ ") VALUES ("
			+ parseInt(vnum) + ","
			+ "'" + sqlEsc(recipeType) + "',"
			+ parseInt(vendorVnum) + " "
			+ ");";
	
	sqlQuery(sql);
}
function deleteRecipeByVnum( vnum, type )
{
	if( !isNumber(vnum) )
		return;
	var vContainer = getRecipeContainer(type);
	for(var recipeRow = 0;recipeRow < vContainer.length;++recipeRow)
	{
		if( vContainer[ recipeRow ].vnum == vnum ) {
			vContainer.splice( recipeRow, 1 );
			break;
		}
	}
	vnum = sqlEsc(vnum);
	type = sqlEsc(type);
	var sql;
	sql = "DELETE FROM " + RECIPE_INDEX_TABLE
		+ " WHERE id=" + vnum
		+ " AND type='" + type + "';";
	
	sqlQuery( sql );
	
	sql = "DELETE FROM " + RECIPE_ITEM_TABLE
	    + " WHERE recipe_id=" + vnum
	    + " AND recipe_type='" + type + "';";
	
	sqlQuery( sql );
	
	sql = "DELETE FROM player_recipes"
		+ " WHERE recipe_vnum=" + vnum
		+ " AND recipe_type='" + type + "'";
		
	sqlQuery(sql);
	
	var characterList = getCharacterList();
	
	for(var characterIndex = 0;characterIndex < characterList.length;++characterIndex) {
	
		var character = characterList[ characterIndex ];
		
		if(character.vnum == -1) {
		
			character.removeRecipeDataObject(vnum,type,false);
		}
	}
}
function saveRecipe( recipe )
{
	var vnum = sqlEsc( recipe.vnum );
	var recipeType = sqlEsc( recipe.type );
	var name = recipe.name ? sqlEsc( recipe.name ) : '';
	var skill = recipe.skillRequired ? sqlEsc( recipe.skillRequired ) : 0;
	var successMessageToActor = recipe.successMessageToActor ? sqlEsc( recipe.successMessageToActor ) : '';
	var successMessageToRoom = recipe.successMessageToRoom ? sqlEsc( recipe.successMessageToRoom ) : '';
	var failureMessageToActor = recipe.failureMessageToActor ? sqlEsc( recipe.failureMessageToActor ) : '';
	var failureMessageToRoom = recipe.failureMessageToRoom ? sqlEsc( recipe.failureMessageToRoom ) : '';
	var price = parseInt(recipe.price);
	
	if(isNaN(price))price=0;
	
	var sql = "";
	sql += "REPLACE DELAYED INTO " + RECIPE_INDEX_TABLE;
	sql +=" SET id='" + vnum + "',";
	sql += " name='" + name + "',";
	sql += " type='" + recipeType + "',";
	sql += " skill_required='" + skill + "',";
	sql += " success_message_actor='" + successMessageToActor + "',";
	sql += " success_message_room='" + successMessageToRoom + "',";
	sql += " failure_message_actor='" + failureMessageToActor + "',";
	sql += " failure_message_room='" + failureMessageToRoom + "',";
	sql += " price=" + price;
	
	sqlQuery( sql );
	
	sql = "";
	sql += "DELETE FROM " + RECIPE_ITEM_TABLE;
	sql +=" WHERE recipe_id=" + vnum;
	sql +=" AND recipe_type='" + recipeType + "';";
	
	sqlQuery( sql );
	
	sql = "";
	sql += "DELETE FROM " + RECIPE_VENDOR_TABLE;
	sql +=" WHERE recipe_id=" + vnum;
	sql +=" AND recipe_type='" + recipeType + "';";
	
	sqlQuery( sql );
	for(var i = 0;i < recipe.ingredients.length;++i) {
		saveRecipeItem(vnum, recipeType, recipe.ingredients[i], "INGRED");
	}
	for(var i = 0;i < recipe.results.length;++i) {
		saveRecipeItem(vnum, recipeType, recipe.results[i], "RESULT");
	}
	if (recipe.vendors) {
		for(var i = 0;i < recipe.vendors.length;++i) {
			saveRecipeVendor(vnum, recipeType, recipe.vendors[i]);
		}
	}
	
	if( !getRecipeByVnum( recipe.vnum, recipe.type ) )
		addRecipe( recipe );
}
function getRecipeContainer( type ) {
	if( type == constants.RECIPE_TYPE_MIXING )
		return global.vMixingRecipes;
	if( type == constants.RECIPE_TYPE_SMITHING )
		return global.vSmithingRecipes;
	if( type == constants.RECIPE_TYPE_TAILORING )
		return global.vTailoringRecipes;
	return null;
}
function loadAllRecipes()
{
	var resultSet = sqlQuery("SELECT * FROM " + RECIPE_INDEX_TABLE + " ORDER BY id,type ASC;");
	if( resultSet.numRows == 0 )
		return;
	
	var itemQuery = sqlQuery("SELECT * FROM " + RECIPE_ITEM_TABLE + " ORDER BY recipe_id,recipe_type ASC;");
	var vendorQuery = sqlQuery("SELECT * FROM " + RECIPE_VENDOR_TABLE + " ORDER BY recipe_id,recipe_type ASC;");
	global.vTailoringRecipes = [];
	global.vSmithingRecipes = [];
	global.vMixingRecipes = [];
	
	while( resultSet.hasNextRow ) {
		//For each recipe, create an array of item rows.
		var vItems = [];//An array of recipe items(JSRows)
		var vVendors = [];//Virtual numbers of vendors who sell this recipe.
		var row = resultSet.getRow;//The row holding the recipe data(recipe_index table row)
		var recipeType = row.get("type");
		while( itemQuery.hasNextRow ) {
			var iRow = itemQuery.peekRow;//Don't iterate, just get the row.
			if( iRow.get("recipe_id") == row.get("id") && iRow.get("recipe_type") == recipeType )
				vItems.push( iRow );
			else
				break;//The item's vnum doesn't match. Let's exit the loop, since we know we've found all of this recipe' items.
			itemQuery.skipRow();//At this point, we know we're using the row, so we will move the cursor.
		}
		while( vendorQuery.hasNextRow ) {
			var vRow = vendorQuery.peekRow;
			if( vRow.get("recipe_id") == row.get("id") && vRow.get("recipe_type") == recipeType )
				vVendors.push( vRow );
			else
				break;//The item's vnum doesn't match. Let's exit the loop, since we know we've found all of this recipe' items.
			vendorQuery.skipRow();//At this point, we know we're using the row, so we will move the cursor.
		}
		
		var vContainer = getRecipeContainer( recipeType );
		if( !vContainer ) {
			continue;
		}
		var recipe = bootRecipe( row, vItems, vVendors );
		vContainer.push( recipe );
	}
}
/**************************************************
 * Returns an array of all recipe objects listing *
 * vendorVnum in their vendor list unless the     *
 * optional recipeType field is passed, in which  *
 * case only recipes of that type are returned    *
 **************************************************/
                                     /*optional*/
function getVendorRecipes(vendorVnum, recipeType) {
	if ( recipeType == undefined ) {
		recipeType == false;
	}
	// Get Vendor Recipes \\
	
	//Global Vectors
	var mixRec = getRecipeContainer( constants.RECIPE_TYPE_MIXING );
	var smithRec = getRecipeContainer( constants.RECIPE_TYPE_SMITHING );
	var tailRec = getRecipeContainer( constants.RECIPE_TYPE_TAILORING );
	/**Determine Vendor's Available Recipes**/
	//Get Vendor's Mixing Recipes
	var myMix = [];
	for each ( var recipe in mixRec ) {
		if ( arrContains(recipe.vendors, vendorVnum) ) {
			myMix.push(recipe);
		}
	}
	if ( recipeType == constants.RECIPE_TYPE_MIXING ) {
		return myMix;
	}
	//Get Vendor's Smithing Recipes
	var mySmith = [];
	for each ( var recipe in smithRec ) {
		if ( arrContains(recipe.vendors, vendorVnum) ) {
			mySmith.push(recipe);
		}
	}
	if ( recipeType == constants.RECIPE_TYPE_SMITHING ) {
		return mySmith;
	}
	//Get Vendor's Tailoring Recipes
	var myTailor = [];
	for each ( var recipe in tailRec ) {
		if ( arrContains(recipe.vendors, vendorVnum) ) {
			myTailor.push(recipe);
		}
	}
	if ( recipeType == constants.RECIPE_TYPE_TAILORING ) {
		return myTailor;
	}
	//List of all recipes
	var myAll = myMix.concat(mySmith).concat(myTailor);
	return myAll;
}
