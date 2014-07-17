//Returns an INTEGER for how many trades the actor has prac'd to 99%
//Koradin - May 2010
function getNumberOfArtisanTrades( actor )
{
	var number = 0;
	if (actor.quest("artisan_Mixing") == 1)
		number += 1;
	if (actor.quest("artisan_Cooking") == 1)
		number += 1;
	if (actor.quest("artisan_Fishing") == 1)
		number += 1;
	if (actor.quest("artisan_Herbalism") == 1)
		number += 1;
	if (actor.quest("artisan_Mining") == 1)
		number += 1;
	if (actor.quest("artisan_Smithing") == 1)
		number += 1;
	if (actor.quest("artisan_Tailoring") == 1)
		number += 1;
	if (actor.quest("artisan_Skinning") == 1)
		number += 1;
	return number;
}
