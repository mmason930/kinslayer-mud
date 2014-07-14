var script9885 = function(self, actor, here, args, extra)
{
	//Alder
//May 2010
//a delicate ceremonial dagger can break
var owner = self.worn_by;
if ( owner != null && owner.position == constants.POS_FIGHTING && random(1,100) == 1 ) {
	getCharCols(actor,constants.CL_OFF);
	actor.send(bld+capFirstLetter(self.name)+"'s blade breaks off of the hilt, rendering the weapon useless."+nrm);
	self.extract();
}
};


var testFunction123 = function()
{
};
