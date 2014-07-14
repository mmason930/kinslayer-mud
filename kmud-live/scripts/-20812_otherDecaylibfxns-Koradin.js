//getDecayProgress( item )
//Koradin - May 2010
//RETURNS THE PERCENT THE ITEM HAS DECAYED, OR -1 IF NO DECAY TIMER EXISTS FOR ITEM
function getDecayProgress( item )
{
	if (!item)
		return null;
	if (!item.getPval("decay"))
		return null;
	var pvalArray = item.getPval("decay").split("~");
	var startTime = parseInt(pvalArray[0]);
	var expireTime = parseInt(pvalArray[1]);
	var currentTime = time();
	var percent = Math.floor(((currentTime - startTime) / expireTime) * 100);
	return percent;
}
function multiplyDecaySpeed( item, multiplier)
{
	if (!item)
		return;
	if (!item.getPval("decay"))
		return;
	var pvalArray = item.getPval("decay").split("~");
	var startTime = parseInt(pvalArray[0]);
	var expireTime = parseInt(pvalArray[1]);
	if (!multiplier || multiplier == 1)
		multiplier = 2;
	multiplier = random(multiplier - 1, multiplier + 1);
	multiplier *= 13;
	expireTime = expireTime - multiplier;
	if (expireTime < 1)
		expireTime = 1;
	item.deletePval("decay",true);
	item.setPval("decay",startTime+"~"+expireTime,true);
	return;
	
}
