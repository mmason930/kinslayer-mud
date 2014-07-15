//setDecayTimer( item, item type, timer, <timer type> )
//Koradin - May 2010
function setDecayTimer( item, timer, timerType)
{
	if (!item)
		return;
	if (!itemType)
		var itemType = constants.OBJECT_DECAY_GENERAL;
	if (!timer)
		return;
	if (!timerType)
		timer *= 60;
	if (timerType == constants.OBJECT_DECAY_MINUTES)
		timer *= 60;
	else if (timerType == constants.OBJECT_DECAY_HOURS)
		timer *= 3600;
	else if (timerType == constants.OBJECT_DECAY_DAYS)
		timer *= 86400;
	else if (timerType == constants.OBJECT_DECAY_WEEKS)
		timer *= 604800;
	else if (timerType == constants.OBJECT_DECAY_MONTHS)
		timer *= 2592000;
	else if (timerType == constants.OBJECT_DECAY_YEARS)
		timer *= 31536000;
	item.setPval("decay",time()+"~"+timer,true);
}
function getCurrentDecayMessage(item,complete)
{
	var progress = getDecayProgress(item);
	if (progress < 1 || !progress || progress == null)
		return "";
	if (!item)
		return "";
	if (!item.decayType)
	{
		var shortMessages = "scratched marred gouged broken";
		var longMessage = capFirstLetter(item.name)+" breaks down completely, falling into pieces.";
	}
	if (item.decayType == constants.OBJECT_DECAY_GENERAL)
	{
		var shortMessages = "scratched marred gouged broken";
		var longMessage = capFirstLetter(item.name)+" breaks down completely, falling into pieces.";
	}
	else if (item.decayType == constants.OBJECT_DECAY_WOOD)
	{
		var shortMessages = "scratched marred gouged cracked";
		var longMessage = "A loud CRACK sounds as "+item.name+" breaks, becoming useful for only firewood.";
	}
	else if (item.decayType == constants.OBJECT_DECAY_PLANT)
	{
		var shortMessages = "limp withering withered dying";
		var longMessage = capFirstLetter(item.name)+" withers away completely, blowing away with the breeze.";
	}
	else if (item.decayType == constants.OBJECT_DECAY_METAL)
	{
		var shortMessages = "tarnished brittle rusted corroded";
		var longMessage = capFirstLetter(item.name)+" finally falls apart, corroded pieces dropping away.";
	}
	else if (item.decayType == constants.OBJECT_DECAY_CLOTH)
	{
		var shortMessages = "thinning faded weathered tattered";
		var longMessage = capFirstLetter(item.name)+" falls apart, strips of cloth dropping away.";
	}
	else if (item.decayType == constants.OBJECT_DECAY_FOOD)
	{
		var shortMessages = "aging stale moldy rotten";
		var longMessage = "Flies surround "+item.name+" and consume it completely.";
	}
	var message = shortMessages.split(" ");
	if (complete == true)
		return longMessage;
	if (progress > 89)
		return message[3];
	else if (progress > 79)
		return message[2];
	else if (progress > 64)
		return message[1];
	else if (progress > 44)
		return message[0];
	else
		return "";
}
