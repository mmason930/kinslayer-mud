//	June 2010 - Fogel
//      Moved this method to JavaScript to avoid having crafting items show up as retooled.
JSObject.prototype.isRetooled = function()
{
	//	Ignore crafting base items.
	if( this.vnum >= 4800 && this.vnum <= 4999 )
		return false;
		
	var name = this.name;
	var namelist = this.namelist;
	var ldesc = this.ldesc;
	this.setRetoolName("");
	this.setRetoolDesc("");
	this.setRetoolSDesc("");
	if( name == this.name && namelist == this.namelist && ldesc == this.ldesc )
		return false;
	else
	{
		this.setRetoolSDesc(name);
		this.setRetoolName(namelist);
		this.setRetoolDesc(ldesc);
		return true;
	}
}
