//getBestTailoringDrop(char) - for humanoids loading tailoring supplies
//Koradin - June 2010
function getBestTailoringDrop( ch )
{
	var roll = random(1,10);
	var size_roll = random(1,120);
	if (!ch)
		return null;
	if (ch.vnum == -1 || ch.race > 3 || random(1,6) == 3)
		return null;
	var cloth  = [4910,4911,4912];
	var linen  = [4913,4914,4915];
	var wool   = [4916,4917,4918];
	var silk   = [4919,4920,4921];
	var velvet = [4922,4923,4924];
	if (ch.level > 64)
	{
		if (roll > 7) {
			size_roll += 30;
			var results = velvet;
		}
		else {
			size_roll += 50;
			var results = silk;
		}
	}
	else if (ch.level > 49)
	{
		if (roll > 8) {
			size_roll += 10;
			var results = velvet;
		}
		else if (roll > 5) {
			size_roll += 30;
			var results = silk;
		}
		else {
			size_roll += 50;
			var results = wool;
		}
	}
	else if (ch.level > 29)
	{
		if (roll > 8)
			var results = silk;
		else if (roll > 5) {
			size_roll += 10;
			var results = wool;
		}
		else {
			size_roll += 30;
			var results = linen;
		}
	}
	else if (ch.level > 14)
	{
		if (roll > 8)
			var results = wool;
		else if (roll > 5)
			var results = linen;
		else {
			size_roll += 10;
			var results = cloth;
		}
	}
	else
	{
		if (roll > 8)
			var results = linen;
		else if (roll > 5)
			var results = cloth;
		else
			return null;
	}
	if (size_roll > 110)
		var result = results[2];
	else if (size_roll > 80)
		var result = results[1];
	else
		var result = results[0];
	return result;
}
