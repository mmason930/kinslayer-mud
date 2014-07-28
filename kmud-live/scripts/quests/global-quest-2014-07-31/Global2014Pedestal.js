function Global2014Pedestal(object, maxHitPoints, race)
{
	this.object = object;
	this.maxHitPoints = maxHitPoints;
	this.hitPoints = maxHitPoints;
	this.race = race;
}

Global2014Pedestal.getObject = function()
{
	return this.object;
};

Global2014Pedestal.getMaxHitPoints = function()
{
	return this.maxHitPoints;
};

Global2014Pedestal.getHitPoints = function()
{
	return this.hitPoints;
};

Global2014Pedestal.getRace = function()
{
	return this.race;
};