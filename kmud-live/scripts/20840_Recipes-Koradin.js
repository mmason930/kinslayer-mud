var script20840 = function(self, actor, here, args, extra)
{
	//RECIPES command to view recipes you know
//Koradin - May 2010
_block;
actor.send("");
if (actor.position != constants.POS_SITTING && actor.position != constants.POS_STANDING && actor.position != constants.POS_RESTING)
{
	actor.send("Now is definitely not the time to flip through your recipe book...");
	return;
}
getCharCols(actor);
var mixRecipes = actor.getRecipes(constants.RECIPE_TYPE_MIXING);
var mixNum = 0;
for (i=0;i<mixRecipes.length;i++)
{
	if (mixRecipes[i])
		mixNum += 1;
}
var smithRecipes = actor.getRecipes(constants.RECIPE_TYPE_SMITHING);
var smithNum = 0;
for (i=0;i<smithRecipes.length;i++)
{
	if (smithRecipes[i])
		smithNum += 1;
}
var tailorRecipes = actor.getRecipes(constants.RECIPE_TYPE_TAILORING)
var tailorNum = 0;
for (i=0;i<tailorRecipes.length;i++)
{
	if (tailorRecipes[i])
		tailorNum += 1;
}
var allNum = mixNum + smithNum + tailorNum;
if (allNum == 0) {
	actor.send("Your recipe book is empty! Go learn some recipes first.");
	return;
}
// if (!total_pVnums || total_pVnums.length < 1)
// {
	// actor.send("Your recipe book is empty!  Go learn some recipes first.");
	// return;
// }
// for each (var pVnum in total_pVnums)
// {
	// var potion = getPotionByVnum(pVnum);
	// var potion_type = potion.name.split(" ");
	// if (potion_type.pop() == "vial")
		// mixing_recipes.push(potion);
	// else if (potion_type.pop() == "meal")
		// cooking_recipes.push(potion);
// }
if (!getSval(actor,20840,"recipes") || getSval(actor,20840,"recipes") == 0)
	act("$n cracks open $s recipe book and starts thumbing through the pages.",true,actor,null,null,constants.TO_ROOM);
actor.send(bld+"Recipe Book Table of Contents"+nrm);
actor.send("____________________________________________________________________________");
actor.send("");
actor.send(grn+"1"+nrm+") Smithing                            Recipes: "+cyn+smithNum+nrm);
actor.send(grn+"2"+nrm+") Mixing                              Recipes: "+cyn+mixNum+nrm);
actor.send(grn+"3"+nrm+") Tailoring                           Recipes: "+cyn+tailorNum+nrm);
actor.send(grn+"4"+nrm+") All Recipes                         Recipes: "+cyn+allNum+nrm);
actor.send("");
actor.send("----------------------------------------------------------------------------");
actor.send("["+grn+"#"+nrm+"] Make a Selection ("+grn+"1-4"+nrm+")         ["+grn+"C"+nrm+"] Close Recipe Book");
setSval(actor,20840,"recipes",1);
actor.attach(20839);

};

