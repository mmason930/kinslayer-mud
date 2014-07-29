function Global2014Pedestal(object, maxHitPoints, race)
{
	this.object = object;
	this.maxHitPoints = maxHitPoints;
	this.hitPoints = maxHitPoints;
	this.repairPercent = 0;
	this.race = race;
	this.roomVnum = object.room.vnum;
	this.objectId = object.id;
}

Global2014Pedestal.prototype.getObject = function()
{
	return this.object;
};

Global2014Pedestal.prototype.getMaxHitPoints = function()
{
	return this.maxHitPoints;
};

Global2014Pedestal.prototype.getHitPoints = function()
{
	return this.hitPoints;
};

Global2014Pedestal.prototype.getRace = function()
{
	return this.race;
};

Global2014Pedestal.prototype.isDisabled = function()
{
	return this.getHitPoints() == 0;
};

Global2014Pedestal.prototype.getRepairPercent = function()
{
	return this.repairPercent;
}

Global2014Pedestal.prototype.takeDamage = function(character, damageAmount)
{
	if(!this.object.isValid)
	{
		mudLog(constants.BRF, 100, "Pedestal `" + this.objectId + "` in room #" + this.roomVnum + " is invalid but taking damage(" + damageAmount + ").");
		return;
	}

	if(!character.isValid)
	{
		mudLog(constants.BRF, 100, "Pedestal `" + this.objectId + "` in room #" + this.roomVnum + " is taking damage(" + damageAmount + ") from invalid character.");
		return;
	}

	if(damageAmount <= 0)
	{
		mudLog(constants.BRF, 100, "Pedestal `" + this.objectId + "` in room #" + this.roomVnum + " is taking negative damage(" + damageAmount + ") from " + character.name + ".");
		return;
	}

	mudLog(constants.BRF, 100, "Pedestal in room #" + this.roomVnum + " has taken " + damageAmount + " from " + character.name + ".");

	this.hitPoints = Math.max(0, this.getHitPoints() - damageAmount);

	mudLog(constants.BRF, 100, "Pedestal in room #" + this.roomVnum + " is now at " + this.getHitPoints() + " hit point" + (this.getHitPoints() == 1 ? "" : "s") + ".");

	if(this.hitPoints <= 0)
	{
		mudLog(constants.BRF, 100, "Pedestal in room #" + this.roomVnum + " has been disabled.");
		this.repairPercent = 0;
	}
};

Global2014Pedestal.prototype.repair = function(character, amount)
{
	if(!this.object.isValid)
	{
		mudLog(constants.BRF, 100, "Pedestal `" + this.objectId + "` in room #" + this.roomVnum + " is invalid but being repaired (" + amount + ").");
		return;
	}

	if(!character.isValid)
	{
		mudLog(constants.BRF, 100, "Pedestal `" + this.objectId + "` in room #" + this.roomVnum + " is being repaired(" + amount + ") from invalid character.");
		return;
	}

	if(damageAmount <= 0)
	{
		mudLog(constants.BRF, 100, "Pedestal `" + this.objectId + "` in room #" + this.roomVnum + " is being repaired a negative amount(" + amount + ") from " + character.name + ".");
		return;
	}

	mudLog(constants.BRF, 100, "Pedestal in room #" + this.roomVnum + " has received " + amount + "% repair from " + character.name + ".");

	this.repairPercent = Math.min(100, this.getRepairPercent() + amount);

	mudLog(constants.BRF, 100, "Pedestal in room #" + this.roomVnum + " is now at " + this.getRepairPercent() + "% repaired.");

	if(this.hitPoints <= 0)
	{
		mudLog(constants.BRF, 100, "Pedestal in room #" + this.roomVnum + " has been enabled.");
		this.hitPoints = this.getMaxHitPoints();
	}
};
