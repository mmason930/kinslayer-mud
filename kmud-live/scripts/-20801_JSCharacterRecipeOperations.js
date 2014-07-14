//Recipes database query lib fxns
//Koradin + Alder - May 2010
JSCharacter.prototype.getRecipes = function( type ) {
	if( this.vnum != -1 )
		return [];
	if( !this._recipes )
		this._recipes = [];
	if( !this._recipes[ type ] )
		this._recipes[ type ] = [];
	return this._recipes[ type ];
}
JSCharacter.prototype.loadRecipes = function() {
	this.clearAllRecipes();
	var sql = "";
	sql += "SELECT recipe_vnum, recipe_type";
	sql +=" FROM player_recipes";
	sql +=" WHERE user_id='" + sqlEsc(this.id) + "'";
	sql +=" ORDER BY recipe_vnum ASC;";
	var rs = sqlQuery( sql );
	var vRecipes;
	while ( rs.hasNextRow ) {
		var recipe = new Object();
		var row = rs.getRow;
		recipe.vnum = parseInt(row.get('recipe_vnum'));
		recipe.type = row.get('recipe_type');
		
		vRecipes = this.getRecipes( recipe.type );
		vRecipes.push( recipe );
	}
}
JSCharacter.prototype.clearRecipesOfType = function( type ) {
	var vRecipes = this.getRecipes( type );
	vRecipes.splice(0,vRecipes.length);
}
JSCharacter.prototype.clearAllRecipes = function() {
	if( this._recipes ) {
		delete this._recipes;
	}
}
JSCharacter.prototype.unlockRecipe = function( iVnum, sType )
{
	getCharCols(this);
	var vRecipes = this.getRecipes( sType );
	var existingRecipeIndex = binarySearch(vRecipes, iVnum, function(e){return e.vnum;});
	var recipeProto = getRecipeByVnum(iVnum,sType);
	
	if( !recipeProto ) {//There is no recipe prototype by this vnum. Therefore, we can't unlock it.
		return;//And so, we will do nothing.
	}
	if( existingRecipeIndex != -1 ) {//They have already unlocked this recipe.
		return;//And so, we will do nothing.
	}
	//Otherwise, they have not yet unlocked it. Let's go ahead and take care of that.
	
	sqlQuery("INSERT INTO player_recipes (`user_id`,`recipe_vnum`,`recipe_type`) VALUES('"+sqlEsc(this.id)+"',"+sqlEsc(iVnum)+",'"+sqlEsc(sType)+"');");
	
	var newPlayerRecipe = new Object();
	newPlayerRecipe.type = sType;
	newPlayerRecipe.vnum = iVnum;
	sortedInsert(vRecipes, newPlayerRecipe, function(e){return e.vnum;});
	this.send("You have learned the recipe for " + grn + bld + recipeProto.name + nrm + "!");
}
JSCharacter.prototype.isRecipeUnlocked = function( vRecipe ) {
	var vnum = vRecipe.vnum;
	for each ( var recipe in this.getRecipes(vRecipe.type) ) {
		if ( recipe.vnum == vnum ) {
			return true;
		}
	}
	return false;
}
/***********************************************************
 * sOperation                                              *
 *   "clear" - removes player's recipe array from game     *
 * Not supplying sOperation will return a writable         *
 * version of player's recipe array if it exists           *
 * NOTE: if you are just trying to see a player's recipes, *
 *       use 'actor.recipes', not this function.           *
 **********************************************************/
JSCharacter.prototype.accessRecipes = function ( sOperation ) {
	if ( this.vnum != -1 ) {
		return false;
	}
	var clear = strn_cmp(sOperation,"clear",1);
	if ( clear ) {
		this._recipes = [];
		return true;
	}
	else {
		return this._recipes;
	}
	return false;
}
JSCharacter.prototype.lockRecipe = function( iVnum, sType )
{
	getCharCols(this);
	var recipe = getRecipeByVnum(iVnum, sType);
	var unlocked = this.isRecipeUnlocked(recipe);
	if (unlocked == true) {
		// getRoom(98).echo("This recipe is unlocked. Deleting...");
		sqlQuery("DELETE FROM player_recipes WHERE user_id = "+sqlEsc(this.id)+" && recipe_vnum = "+sqlEsc(iVnum)+";");
		
		this.removeRecipeDataObject(iVnum,sType,true);
	}
	return false;
}
                                                                      /*optional*/
JSCharacter.prototype.removeRecipeDataObject = function( iVnum, sType, show )
{
	var sRecipes = this.getRecipes(sType);
	for ( var i = 0; i < sRecipes.length; i++ ) {
		var rVnum = sRecipes[i].vnum;
		if ( rVnum == iVnum ) {
			sRecipes.splice(i,1);
			
			if(show) {
			
				this.send(red+bld+"You have forgotten the recipe for "+nrm+bld+getRecipeByVnum(iVnum,sType).name+nrm+red+bld+"!"+nrm);
			}
			return true;
		}
	}
}
