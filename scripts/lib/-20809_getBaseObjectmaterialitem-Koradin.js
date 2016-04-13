//Returns the vnum of the base object for retooled <item> of <material>
//Used for smithing and tailoring, not much else, intHanded is optional
//Koradin - May 2010
function getBaseObject( mat, type, intHanded, attribute )
{
	if (type == "Boots")
	{
		if (mat == "Skin")//Boots
			return 31080;//blue velvet slippers//
		else if (mat == "Hide")//Boots
			return 31089;//comfortable shoes
		else if (mat == "Cloth")//Boots
			return 31098;//soft leather boots
		else if (mat == "Linen")//Boots
			return 31107;//soft velvet slippers
		else if (mat == "Wool")//Boots
			return 31116;//wolfhide boots
		else if (mat == "Silk")//Boots
			return 31125;//ranger boots
		else if (mat == "Velvet")//Boots
			return 31134;//gold fringed boots
		else if (mat == "Fur")//Boots
			return 31143;//soft-soled boots
		else if (mat == "Light Leather")//Boots
			return 31728;//worn-out work boots
		else if (mat == "Leather")//Boots
			return 31737;//padded leather boots
		else if (mat == "Quilted Leather")//Boots
			return 31746;//thick leather boots
		else if (mat == "Padded Leather")//Boots
			return 31755;//leather banded shoes
		else if (mat == "Studded Leather")//Boots
			return 31764;//knee-length boots
		else if (mat == "Chainmail")//Boots
			return 31773;//hard leather boots
		else if (mat == "Scale")//Boots
			return (attribute=="Heavier"?31782:32376);//steel plated leather boots
		else if (mat == "Brigandine")//Boots
			return (attribute=="Heavier"?31791:32385);//fur-topped plated chainmail boots
		else if (mat == "Tin")//Boots
			return 11660;//thick leather boots
		else if (mat == "Aluminum")//Boots
			return 11669;//hard leather boots
		else if (mat == "Copper")//Boots
			return 11678;//thin metal boots
		else if (mat == "Iron")//Boots
			return 11687;//thick metal boots
		else if (mat == "Steel")//Boots
			return 11696;//polished metal boots
		else if (mat == "Bronze")//Boots
			return 11705;//plated metal boots
		else if (mat == "Silver")//Boots
			return 11714;//thick steel boots
		else if (mat == "Gold")//Boots
			return 11723;//black steel war boots
		else if (mat == "Platinum")//Boots
			return 11732;//steel-plated war boots
		else if (mat == "Obsidian")//Boots
			return 11741;//steel-bound combat boots
	}
	else if (type == "Gauntlets" || type == "Gloves")
	{
		if (mat == "Skin")//Gloves
			return 31152;//brown linen gloves
		else if (mat == "Hide")//Gloves
			return 31161;//black mittens
		else if (mat == "Cloth")//Gloves
			return 31170;//soft leather riding gloves
		else if (mat == "Linen")//Gloves
			return 31179;//black gloves
		else if (mat == "Wool")//Gloves
			return 31188;//sleek gloves
		else if (mat == "Silk")//Gloves
			return 31197;//smooth black and brown fur gloves
		else if (mat == "Velvet")//Gloves
			return 31206;//gloves embroidered with gold
		else if (mat == "Fur")//Gloves
			return 31215;//swordsman gloves
		else if (mat == "Light Leather")//Gloves
			return 31800;//studded leather gloves
		else if (mat == "Leather")//Gloves
			return 31810;//thick boarhide gloves
		else if (mat == "Quilted Leather")//Gloves
			return 31819;//black fur-coated leather gloves
		else if (mat == "Padded Leather")//Gloves
			return 31828;//half-metal gauntlets
		else if (mat == "Studded Leather")//Gloves
			return 31837;//chainmail gloves
		else if (mat == "Chainmail")//Gloves
			return 31846;//riveted chainmail gloves
		else if (mat == "Scale")//Gloves
			return (attribute=="Heavier"?31855:32395);//grolm skin gloves
		else if (mat == "Brigandine")//Gloves
			return (attribute=="Heavier"?31864:32404);//tough pair of gatorskin gloves
		else if (mat == "Tin")//Gauntlets
			return 11750;//half-metal gauntlets
		else if (mat == "Aluminum")//Gauntlets
			return 11759;//light metal gloves
		else if (mat == "Copper")//Gauntlets
			return 11768;//chainmail gloves
		else if (mat == "Iron")//Gauntlets
			return 11777;//thin metal gauntlets
		else if (mat == "Steel")//Gauntlets
			return 11786;//steel-backed gauntlets
		else if (mat == "Bronze")//Gauntlets
			return 11795;//iron gauntlets
		else if (mat == "Silver")//Gauntlets
			return 11804;//heavy metal gauntlets
		else if (mat == "Gold")//Gauntlets
			return 11813;//thick steel gauntlets
		else if (mat == "Platinum")//Gauntlets
			return 11822;//thick metal gauntlets
		else if (mat == "Obsidian")//Gauntlets
			return 11831;//shimmering steel gauntlets
	}
	else if (type == "Belt")
	{
		if (mat == "Skin")//Belt
			return 31224;//plain sword belt
		else if (mat == "Hide")//Belt
			return 31233;//strand of loose rope
		else if (mat == "Cloth")//Belt
			return 31242;//sturdy leather belt
		else if (mat == "Linen")//Belt
			return 31251;//fine silken sash
		else if (mat == "Wool")//Belt
			return 31260;//silky white belt with gems
		else if (mat == "Silk")//Belt
			return 31269;//black leather belt with silver buckle
		else if (mat == "Velvet")//Belt
			return 31278;//swordsman's belt
		else if (mat == "Fur")//Belt
			return 31287;//studded leather belt
		else if (mat == "Light Leather")//Belt
			return 31873;//tough leather belt
		else if (mat == "Leather")//Belt
			return 31882;//loose copper belt
		else if (mat == "Quilted Leather")//Belt
			return 31891;//belt made of bones with a skull shaped buckle
		else if (mat == "Padded Leather")//Belt
			return 31900;//wolfskin belt
		else if (mat == "Studded Leather")//Belt
			return 31909;//rough steel belt with ornate buckle
		else if (mat == "Chainmail")//Belt
			return 31918;//strange chain belt
		else if (mat == "Scale")//Belt
			return (attribute=="Heavier"?31927:32413);//worm-hide belt
		else if (mat == "Brigandine")//Belt
			return (attribute=="Heavier"?31936:32422);//polished black belt
		else if (mat == "Tin")//Belt
			return 11840;//tough leather belt
		else if (mat == "Aluminum")//Belt
			return 11849;//loose copper belt
		else if (mat == "Copper")//Belt
			return 11858;//belt made of bones with a skull shaped buckle
		else if (mat == "Iron")//Belt
			return 11867;//wolfskin belt
		else if (mat == "Steel")//Belt
			return 11876;//rough steel belt with ornate buckle
		else if (mat == "Bronze")//Belt
			return 11885;//a gilded warbelt
		else if (mat == "Silver")//Belt
			return 11894;//strange chain belt
		else if (mat == "Gold")//Belt
			return 11903;//polished black belt
		else if (mat == "Platinum")//Belt
			return 11912;//tough section of boarhide
		else if (mat == "Obsidian")//Belt
			return 11921;//shimmering steel belt
	}
	else if (type == "Spaulders" || type == "Pads")
	{
		if (mat == "Skin")//Shoulders
			return 31296;//battered and stained shoulder coverings
		else if (mat == "Hide")//Shoulders
			return 31305;//Aes Sedai's shawl
		else if (mat == "Cloth")//Shoulders
			return 31314;//cheap cloth shoulder coverings
		else if (mat == "Linen")//Shoulders
			return 31323;//tough leather shoulder pads
		else if (mat == "Wool")//Shoulders
			return 31332;//bright red stole
		else if (mat == "Silk")//Shoulders
			return 31341;//finely worked shoulder pads sparkling with gold
		else if (mat == "Velvet")//Shoulders
			return 31350;//white leather shoulder pads
		else if (mat == "Fur")//Shoulders
			return 31359;//golden knots of rank
		else if (mat == "Light Leather")//Shoulders
			return 31945;//cheap cloth shoulder coverings
		else if (mat == "Leather")//Shoulders
			return 31954;//blue woven shoulder coverings
		else if (mat == "Quilted Leather")//Shoulders
			return 31963;//tough leather shoulder pads
		else if (mat == "Padded Leather")//Shoulders
			return 31972;//worn metal shoulder pads
		else if (mat == "Studded Leather")//Shoulders
			return 31981;//light metal pauldrons
		else if (mat == "Chainmail")//Shoulders
			return 31990;//finely worked shoulder pads sparkling with gold
		else if (mat == "Scale")//Shoulders
			return (attribute=="Heavier"?31999:32431);//shoulder pads worked with silver
		else if (mat == "Brigandine")//Shoulders
			return (attribute=="Heavier"?32008:32440);//spaulders branded with a sleek fox
		else if (mat == "Tin")//Shoulders
			return 11930;//tough leather shoulder pads
		else if (mat == "Aluminum")//Shoulders
			return 11939;//worn metal shoulder pads
		else if (mat == "Copper")//Shoulders
			return 11948;//light metal pauldrons
		else if (mat == "Iron")//Shoulders
			return 11957;//thin metal pauldrons
		else if (mat == "Steel")//Shoulders
			return 11966;//light steel pauldrons
		else if (mat == "Bronze")//Shoulders
			return 11975;//shoulder pads worked with silver
		else if (mat == "Silver")//Shoulders
			return 11984;//steel pauldrons
		else if (mat == "Gold")//Shoulders
			return 11993;//spaulders branded with a sleek fox
		else if (mat == "Platinum")//Shoulders
			return 12002;//thick steel pauldrons
		else if (mat == "Obsidian")//Shoulders
			return 12011;//shimmering steel pauldrons
	}
	else if (type == "Helmet" || type == "Hat")
	{
		if (mat == "Skin")//Head
			return 31368;//comfortable woolen hood
		else if (mat == "Hide")//Head
			return 31377;//rock colored shoufa
		else if (mat == "Cloth")//Head
			return 31386;//ivy entwined headband
		else if (mat == "Linen")//Head
			return 31395;//dark cloth hood
		else if (mat == "Wool")//Head
			return 31404;//velvet leather shoufa
		else if (mat == "Silk")//Head
			return 31413;//shadowed leather hood
		else if (mat == "Velvet")//Head
			return 31422;//golden silk turban
		else if (mat == "Fur")//Head
			return 31431;//hat with white plumes
		else if (mat == "Light Leather")//Head
			return 32017;//soft leather helmet
		else if (mat == "Leather")//Head
			return 32026;//thin leather helmet
		else if (mat == "Quilted Leather")//Head
			return 32035;//tough leather helmet
		else if (mat == "Padded Leather")//Head
			return 32044;//small round topped helmet
		else if (mat == "Studded Leather")//Head
			return 32053;//thick leather coif
		else if (mat == "Chainmail")//Head
			return 32062;//metal helmet
		else if (mat == "Scale")//Head
			return (attribute=="Heavier"?32071:32449);//metal rimmed helmet
		else if (mat == "Brigandine")//Head
			return (attribute=="Heavier"?32080:32458);//golden entwined chainmail coif
		else if (mat == "Tin")//Head
			return 12020;//light metal helmet
		else if (mat == "Aluminum")//Head
			return 12029;//coif
		else if (mat == "Copper")//Head
			return 12038;//reinforced metal helmet
		else if (mat == "Iron")//Head
			return 12047;//crested steel helmet
		else if (mat == "Steel")//Head
			return 12056;//heavy metal helm
		else if (mat == "Bronze")//Head
			return 12065;//black rimmed helmet
		else if (mat == "Silver")//Head
			return 12074;//full helm
		else if (mat == "Gold")//Head
			return 12083;//shining steel helm
		else if (mat == "Platinum")//Head
			return 12092;//great helm
		else if (mat == "Obsidian")//Head
			return 12101;//shimmering steel helmet
	}
	else if (type == "Leggings" || type == "Greaves")
	{
		if (mat == "Skin")//Leggings
			return 31440;//blue wool leggings
		else if (mat == "Hide")//Leggings
			return 31449;//khaki colored breeches
		else if (mat == "Cloth")//Leggings
			return 31458;//wolfhide leggings
		else if (mat == "Linen")//Leggings
			return 31467;//colorful skirt hemmed with cassidae
		else if (mat == "Wool")//Leggings
			return 31476;//baggy green breeches
		else if (mat == "Silk")//Leggings
			return 31485;//dull green earthen breeches
		else if (mat == "Velvet")//Leggings
			return 31494;//bearskin trousers
		else if (mat == "Fur")//Leggings
			return 31503;//dark cloth leggings
		else if (mat == "Light Leather")//Leggings
			return 32089;//ragged pair of pants
		else if (mat == "Leather")//Leggings
			return 32098;//thin leather pants
		else if (mat == "Quilted Leather")//Leggings
			return 32107;//soft leather breeches
		else if (mat == "Padded Leather")//Leggings
			return 32116;//wolfhide leggings
		else if (mat == "Studded Leather")//Leggings
			return 32125;//worn chainmail leggings
		else if (mat == "Chainmail")//Leggings
			return 32134;//thick leather leggings
		else if (mat == "Scale")//Leggings
			return (attribute=="Heavier"?32143:32467);//chainmail leggings
		else if (mat == "Brigandine")//Leggings
			return (attribute=="Heavier"?32152:32476);//flexible riveted chainmail leggings
		else if (mat == "Tin")//Leggings
			return 12110;//thin metal leggings
		else if (mat == "Aluminum")//Leggings
			return 12119;//ringmail leggings
		else if (mat == "Copper")//Leggings
			return 12128;//bloodspattered bone-plate greaves
		else if (mat == "Iron")//Leggings
			return 12137;//thick iron leggings
		else if (mat == "Steel")//Leggings
			return 12146;//thick metal leggings
		else if (mat == "Bronze")//Leggings
			return 12155;//polished greaves
		else if (mat == "Silver")//Leggings
			return 12164;//black steel greaves
		else if (mat == "Gold")//Leggings
			return 12173;//strange green metallic greaves
		else if (mat == "Platinum")//Leggings
			return 12182;//full-plated greaves
		else if (mat == "Obsidian")//Leggings
			return 12191;//shimmering steel greaves
	}
	else if (type == "Sleeves" || type == "Vambraces")
	{
		if (mat == "Skin")//Sleeves
			return 31512;//loose cotton sleeves
		else if (mat == "Hide")//Sleeves
			return 31521;//loosely fitten white cloth sleeves
		else if (mat == "Cloth")//Sleeves
			return 31530;//light leather sleeves
		else if (mat == "Linen")//Sleeves
			return 31539;//elegant silk sleeves
		else if (mat == "Wool")//Sleeves
			return 31548;//earthen sleeves
		else if (mat == "Silk")//Sleeves
			return 31557;//black satin sleeves
		else if (mat == "Velvet")//Sleeves
			return 31566;//golden silk sleeves
		else if (mat == "Fur")//Sleeves
			return 31575;//dark fur sleeves
		else if (mat == "Light Leather")//Sleeves
			return 32161;//light leather sleeves
		else if (mat == "Leather")//Sleeves
			return 32170;//padded leather sleeves
		else if (mat == "Quilted Leather")//Sleeves
			return 32179;//wolfhide sleeves
		else if (mat == "Padded Leather")//Sleeves
			return 32188;//studded leather sleeves
		else if (mat == "Studded Leather")//Sleeves
			return 32197;//chainmail sleeves
		else if (mat == "Chainmail")//Sleeves
			return 32206;//riveted chainmail sleeves
		else if (mat == "Scale")//Sleeves
			return (attribute=="Heavier"?32215:32485);//ringmail sleeves
		else if (mat == "Brigandine")//Sleeves
			return (attribute=="Heavier"?32224:32494);//light, scale backed ringmail sleeves
		else if (mat == "Tin")//Sleeves
			return 12200;//chainmail sleeves
		else if (mat == "Aluminum")//Sleeves
			return 12209;//riveted chainmail sleeves
		else if (mat == "Copper")//Sleeves
			return 12218;//ringmail sleeves
		else if (mat == "Iron")//Sleeves
			return 12227;//light, scale backed ringmail sleeves
		else if (mat == "Steel")//Sleeves
			return 12236;//scale knit sleeves
		else if (mat == "Bronze")//Sleeves
			return 12245;//black steel vambraces
		else if (mat == "Silver")//Sleeves
			return 12254;//plated vambraces
		else if (mat == "Gold")//Sleeves
			return 12263;//thick metal vambraces
		else if (mat == "Platinum")//Sleeves
			return 12272;//shimmering steel vambraces
		else if (mat == "Obsidian")//Sleeves
			return 12281;//flexed obsidian vambraces
	}
	else if (type == "Vest" || type == "Breastplate")
	{
		if (mat == "Skin")//Body
			return 31584;//grey silk dress
		else if (mat == "Hide")//Body
			return 31593;//fancy cotton-packed vest
		else if (mat == "Cloth")//Body
			return 31602;//wolfhide tunic
		else if (mat == "Linen")//Body
			return 31611;//low-cut blouse with jeweled bodice
		else if (mat == "Wool")//Body
			return 31620;//small blue shirt
		else if (mat == "Silk")//Body
			return 31629;//thick green knit tunic
		else if (mat == "Velvet")//Body
			return 31638;//blue silk shirt
		else if (mat == "Fur")//Body
			return 31647;//worn animal hide vest
		else if (mat == "Light Leather")//Body
			return 32233;//tunic fastened with red laces
		else if (mat == "Leather")//Body
			return 32242;//leather soldiers tunic
		else if (mat == "Quilted Leather")//Body
			return 32251;//studded leather tunic
		else if (mat == "Padded Leather")//Body
			return 32260;//light green chainmail shirt
		else if (mat == "Studded Leather")//Body
			return 32269;//ribbed mail tunic crafted from bronze
		else if (mat == "Chainmail")//Body
			return 32278;//snug-fitting jerkin of cured grolm hide
		else if (mat == "Scale")//Body
			return (attribute=="Heavier"?32287:32503);//dull gray-green tunic of overlapping metal scales
		else if (mat == "Brigandine")//Body
			return (attribute=="Heavier"?32296:32512);//black steel overlapping plates
		else if (mat == "Tin")//Body
			return 12290;//close-fitting chainmail tunic
		else if (mat == "Aluminum")//Body
			return 12299;//thick iron tunic
		else if (mat == "Copper")//Body
			return 12308;//iron plate of Lugard Militia
		else if (mat == "Iron")//Body
			return 12317;//cooling-hardened breastplate
		else if (mat == "Steel")//Body
			return 12326;//light-weight splinted plate with red etchings
		else if (mat == "Bronze")//Body
			return 12335;//thick suit of plate mail
		else if (mat == "Silver")//Body
			return 12344;//woven steel battle vest
		else if (mat == "Gold")//Body
			return 12353;//scaled breastplate bearing an insect insignia
		else if (mat == "Platinum")//Body
			return 12362;//steel sunburst emblazoned breastplate
		else if (mat == "Obsidian")//Body
			return 12371;//shimmering steel breastplate
	}
	//SHIELDS ARE SMITHING ONLY, ONLY INCLUDES MATERIALS FOR SMITHING
	else if (type == "Shield")
	{
		if (attribute == "Dodge")
		{
			if (mat == "Tin")//Shield
				return 12380;//dark spiked shield
			else if (mat == "Aluminum")//Shield
				return 12389;//heavy iron shield
			else if (mat == "Copper")//Shield
				return 12398;//large steel shield
			else if (mat == "Iron")//Shield
				return 12407;//leather covered targe
			else if (mat == "Steel")//Shield
				return 12416;//round iron buckler
			else if (mat == "Bronze")//Shield
				return 12425;//reinforced leather shield
			else if (mat == "Silver")//Shield
				return 12434;//curved silver shield
			else if (mat == "Gold")//Shield
				return 12443;//tarnished bronze heater shield
			else if (mat == "Platinum")//Shield
				return 12452;//golden engraved shield
			else if (mat == "Obsidian")//Shield
				return 12461;//crystal trimmed shield
		}
		else if (attribute == "Parry")
		{
			if (mat == "Tin")//Shield
				return 12470;//dark spiked shield
			else if (mat == "Aluminum")//Shield
				return 12479;//heavy iron shield
			else if (mat == "Copper")//Shield
				return 12488;//large steel shield
			else if (mat == "Iron")//Shield
				return 12497;//leather covered targe
			else if (mat == "Steel")//Shield
				return 12506;//round iron buckler
			else if (mat == "Bronze")//Shield
				return 12515;//reinforced leather shield
			else if (mat == "Silver")//Shield
				return 12524;//curved silver shield
			else if (mat == "Gold")//Shield
				return 12533;//tarnished bronze heater shield
			else if (mat == "Platinum")//Shield
				return 12542;//golden engraved shield
			else if (mat == "Obsidian")//Shield
				return 12551;//crystal trimmed shield
		}
	}
	//CLOAKS ARE TAILORING ONLY, ONLY INCLUDES MATS FOR TAILORING
	else if (type == "Cloak")
	{
		if (mat == "Skin")//Cloak
			return 31656;//fine red cloak
		else if (mat == "Hide")//Cloak
			return 31665;//long dress
		else if (mat == "Cloth")//Cloak
			return 31674;//dark brown cloak
		else if (mat == "Linen")//Cloak
			return 31683;//thick fur cloak with strange insignia
		else if (mat == "Wool")//Cloak
			return 31692;//long dark cloak
		else if (mat == "Silk")//Cloak
			return 31701;//long red cloak embroidered with the Rising Sun
		else if (mat == "Velvet")//Cloak
			return 31710;//woodland camouflage hunting cloak
		else if (mat == "Fur")//Cloak
			return 31719;//fur cloak, held by a silver brooch
		else if (mat == "Light Leather")//Cloak
			return 32305;//black velvet spencer
		else if (mat == "Leather")//Cloak
			return 32314;//large cloak with many pockets
		else if (mat == "Quilted Leather")//Cloak
			return 32323;//fashionable purple dress with gleaming buttons
		else if (mat == "Padded Leather")//Cloak
			return 32332;//white cloak with embroidered sunburst
		else if (mat == "Studded Leather")//Cloak
			return 32341;//elegent dress of beautiful cloth
		else if (mat == "Chainmail")//Cloak
			return 32350;//cloak of shifting colours
		else if (mat == "Scale")//Cloak
			return (attribute=="Heavier"?32359:32521);//cloak with wolf-hide border
		else if (mat == "Brigandine")//Cloak
			return (attribute=="Heavier"?32368:32530);//dark blue coat with brass buttons
	}
	//WEAPONS ONLY FROM HERE DOWN, ONLY SMITHING MATERIALS LISTED
	else if (type == "Small Projectile")
	{
		if (mat == "Tin")//Throwing Knife
			return 2600;
		else if (mat == "Aluminum")//Throwing Knife
			return 2601;
		else if (mat == "Copper")//Throwing Knife
			return 2602;
		else if (mat == "Iron")//Throwing Knife
			return 2603;
		else if (mat == "Steel")//Throwing Knife
			return 2604;
		else if (mat == "Bronze")//Throwing Knife
			return 2611;
		else if (mat == "Silver")//Throwing Knife
			return 2605;
		else if (mat == "Gold")//Throwing Knife
			return 2606;
		else if (mat == "Platinum")//Throwing Knife
			return 2607;
		else if (mat == "Obsidian")//Throwing Knife
			return 2608;
	}
	else if (type == "Short Blade")
	{
		if (attribute == "HitRate")
		{
			if (mat == "Tin")//Short Blade
				return 12560;
			else if (mat == "Aluminum")//Short Blade
				return 12569;
			else if (mat == "Copper")//Short Blade
				return 12578;
			else if (mat == "Iron")//Short Blade
				return 12587;
			else if (mat == "Steel")//Short Blade
				return 12596;
			else if (mat == "Bronze")//Short Blade
				return 12605;
			else if (mat == "Silver")//Short Blade
				return 12614;
			else if (mat == "Gold")//Short Blade
				return 12623;
			else if (mat == "Platinum")//Short Blade
				return 12632;
			else if (mat == "Obsidian")//Short Blade
				return 12641;
		}
		else if (attribute == "Damage")
		{
			if (mat == "Tin")//Short Blade
				return 12650;
			else if (mat == "Aluminum")//Short Blade
				return 12659;
			else if (mat == "Copper")//Short Blade
				return 12668;
			else if (mat == "Iron")//Short Blade
				return 12677;
			else if (mat == "Steel")//Short Blade
				return 12686;
			else if (mat == "Bronze")//Short Blade
				return 12695;
			else if (mat == "Silver")//Short Blade
				return 12704;
			else if (mat == "Gold")//Short Blade
				return 12713;
			else if (mat == "Platinum")//Short Blade
				return 12722;
			else if (mat == "Obsidian")//Short Blade
				return 12731;
		}
	}
	else if (type == "Large Projectile")
	{
		if (attribute == "Spear")
		{
			if (mat == "Tin")//Throwing Spear
				return 2620;
			else if (mat == "Aluminum")//Throwing Spear
				return 2621;
			else if (mat == "Copper")//Throwing Spear
				return 2622;
			else if (mat == "Iron")//Throwing Spear
				return 2623;
			else if (mat == "Steel")//Throwing Spear
				return 2624;
			else if (mat == "Bronze")//Throwing Spear
				return 2629;
			else if (mat == "Silver")//Throwing Spear
				return 2625;
			else if (mat == "Gold")//Throwing Spear
				return 2626;
			else if (mat == "Platinum")//Throwing Spear
				return 2627;
			else if (mat == "Obsidian")//Throwing Spear
				return 2628;
		}
		else if (attribute == "Axe")
		{
			if (mat == "Tin")//Throwing Axe
				return 2640;
			else if (mat == "Aluminum")//Throwing Axe
				return 2641;
			else if (mat == "Copper")//Throwing Axe
				return 2642;
			else if (mat == "Iron")//Throwing Axe
				return 2643;
			else if (mat == "Steel")//Throwing Axe
				return 2644;
			else if (mat == "Bronze")//Throwing Axe
				return 2645;
			else if (mat == "Silver")//Throwing Axe
				return 2646;
			else if (mat == "Gold")//Throwing Axe
				return 2647;
			else if (mat == "Platinum")//Throwing Axe
				return 2648;
			else if (mat == "Obsidian")//Throwing Axe
				return 2649;
		}
	}
	else if (type == "Spear")
	{
		if (attribute == "Offensive")
		{
			if (mat == "Tin" && intHanded == 1)//Spear
				return 0;
			else if (mat == "Aluminum" && intHanded == 1)//Spear
				return 0;
			else if (mat == "Copper" && intHanded == 1)//Spear
				return 0;
			else if (mat == "Iron" && intHanded == 1)//Spear
				return 0;
			else if (mat == "Steel" && intHanded == 1)//Spear
				return 0;
			else if (mat == "Bronze" && intHanded == 1)//Spear
				return 0;
			else if (mat == "Silver" && intHanded == 1)//Spear
				return 0;
			else if (mat == "Gold" && intHanded == 1)//Spear
				return 0;
			else if (mat == "Platinum" && intHanded == 1)//Spear
				return 0;
			else if (mat == "Obsidian" && intHanded == 1)//Spear
				return 0;
			else if (mat == "Tin" && intHanded == 2)//Spear
				return 12920;
			else if (mat == "Aluminum" && intHanded == 2)//Spear
				return 12929;
			else if (mat == "Copper" && intHanded == 2)//Spear
				return 12938;
			else if (mat == "Iron" && intHanded == 2)//Spear
				return 12947;
			else if (mat == "Steel" && intHanded == 2)//Spear
				return 12956;
			else if (mat == "Bronze" && intHanded == 2)//Spear
				return 12965;
			else if (mat == "Silver" && intHanded == 2)//Spear
				return 12974;
			else if (mat == "Gold" && intHanded == 2)//Spear
				return 12983;
			else if (mat == "Platinum" && intHanded == 2)//Spear
				return 12992;
			else if (mat == "Obsidian" && intHanded == 2)//Spear
				return 13001;
		}
		else if (attribute == "Defensive")
		{
			if (mat == "Tin" && intHanded == 1)//Spear
				return 12740;
			else if (mat == "Aluminum" && intHanded == 1)//Spear
				return 12749;
			else if (mat == "Copper" && intHanded == 1)//Spear
				return 12758;
			else if (mat == "Iron" && intHanded == 1)//Spear
				return 12767;
			else if (mat == "Steel" && intHanded == 1)//Spear
				return 12776;
			else if (mat == "Bronze" && intHanded == 1)//Spear
				return 12785;
			else if (mat == "Silver" && intHanded == 1)//Spear
				return 12794;
			else if (mat == "Gold" && intHanded == 1)//Spear
				return 12803;
			else if (mat == "Platinum" && intHanded == 1)//Spear
				return 12812;
			else if (mat == "Obsidian" && intHanded == 1)//Spear
				return 12821;
			else if (mat == "Tin" && intHanded == 2)//Spear
				return 12830;
			else if (mat == "Aluminum" && intHanded == 2)//Spear
				return 12839;
			else if (mat == "Copper" && intHanded == 2)//Spear
				return 12848;
			else if (mat == "Iron" && intHanded == 2)//Spear
				return 12857;
			else if (mat == "Steel" && intHanded == 2)//Spear
				return 12866;
			else if (mat == "Bronze" && intHanded == 2)//Spear
				return 12875;
			else if (mat == "Silver" && intHanded == 2)//Spear
				return 12884;
			else if (mat == "Gold" && intHanded == 2)//Spear
				return 12893;
			else if (mat == "Platinum" && intHanded == 2)//Spear
				return 12902;
			else if (mat == "Obsidian" && intHanded == 2)//Spear
				return 12911;
		}
	}
	else if (type == "Lance")
	{
		if (attribute == "Offensive")
		{
		if (mat == "Tin" && intHanded == 1)//Lance
			return 0;
		else if (mat == "Aluminum" && intHanded == 1)//Lance
			return 0;
		else if (mat == "Copper" && intHanded == 1)//Lance
			return 0;
		else if (mat == "Iron" && intHanded == 1)//Lance
			return 0;
		else if (mat == "Steel" && intHanded == 1)//Lance
			return 0;
		else if (mat == "Bronze" && intHanded == 1)//Lance
			return 0;
		else if (mat == "Silver" && intHanded == 1)//Lance
			return 0;
		else if (mat == "Gold" && intHanded == 1)//Lance
			return 0;
		else if (mat == "Platinum" && intHanded == 1)//Lance
			return 0;
		else if (mat == "Obsidian" && intHanded == 1)//Lance
			return 0;
		else if (mat == "Tin" && intHanded == 2)//Lance
			return 30090;
		else if (mat == "Aluminum" && intHanded == 2)//Lance
			return 30099;
		else if (mat == "Copper" && intHanded == 2)//Lance
			return 30108;
		else if (mat == "Iron" && intHanded == 2)//Lance
			return 30117;
		else if (mat == "Steel" && intHanded == 2)//Lance
			return 30126;
		else if (mat == "Bronze" && intHanded == 2)//Lance
			return 30135;
		else if (mat == "Silver" && intHanded == 2)//Lance
			return 30144;
		else if (mat == "Gold" && intHanded == 2)//Lance
			return 30153;
		else if (mat == "Platinum" && intHanded == 2)//Lance
			return 30162;
		else if (mat == "Obsidian" && intHanded == 2)//Lance
			return 30171;
		}
		else if (attribute == "Defensive")
		{
		if (mat == "Tin" && intHanded == 1)//Lance
			return 30180;
		else if (mat == "Aluminum" && intHanded == 1)//Lance
			return 30189;
		else if (mat == "Copper" && intHanded == 1)//Lance
			return 30198;
		else if (mat == "Iron" && intHanded == 1)//Lance
			return 30207;
		else if (mat == "Steel" && intHanded == 1)//Lance
			return 30216;
		else if (mat == "Bronze" && intHanded == 1)//Lance
			return 30225;
		else if (mat == "Silver" && intHanded == 1)//Lance
			return 30234;
		else if (mat == "Gold" && intHanded == 1)//Lance
			return 30243;
		else if (mat == "Platinum" && intHanded == 1)//Lance
			return 30252;
		else if (mat == "Obsidian" && intHanded == 1)//Lance
			return 30261;
		else if (mat == "Tin" && intHanded == 2)//Lance
			return 30270;
		else if (mat == "Aluminum" && intHanded == 2)//Lance
			return 30279;
		else if (mat == "Copper" && intHanded == 2)//Lance
			return 30288;
		else if (mat == "Iron" && intHanded == 2)//Lance
			return 30297;
		else if (mat == "Steel" && intHanded == 2)//Lance
			return 30306;
		else if (mat == "Bronze" && intHanded == 2)//Lance
			return 30315;
		else if (mat == "Silver" && intHanded == 2)//Lance
			return 30324;
		else if (mat == "Gold" && intHanded == 2)//Lance
			return 30333;
		else if (mat == "Platinum" && intHanded == 2)//Lance
			return 30342;
		else if (mat == "Obsidian" && intHanded == 2)//Lance
			return 30351;
		}
	}
	else if (type == "Chain")
	{	
		if (attribute == "Offensive")
		{
		if (mat == "Tin" && intHanded == 1)//Chain
			return 0;
		else if (mat == "Aluminum" && intHanded == 1)//Chain
			return 0;
		else if (mat == "Copper" && intHanded == 1)//Chain
			return 0;
		else if (mat == "Iron" && intHanded == 1)//Chain
			return 0;
		else if (mat == "Steel" && intHanded == 1)//Chain
			return 0;
		else if (mat == "Bronze" && intHanded == 1)//Chain
			return 0;
		else if (mat == "Silver" && intHanded == 1)//Chain
			return 0;
		else if (mat == "Gold" && intHanded == 1)//Chain
			return 0;
		else if (mat == "Platinum" && intHanded == 1)//Chain
			return 0;
		else if (mat == "Obsidian" && intHanded == 1)//Chain
			return 0;
		else if (mat == "Tin" && intHanded == 2)//Chain
			return 30540;
		else if (mat == "Aluminum" && intHanded == 2)//Chain
			return 30549;
		else if (mat == "Copper" && intHanded == 2)//Chain
			return 30558;
		else if (mat == "Iron" && intHanded == 2)//Chain
			return 30567;
		else if (mat == "Steel" && intHanded == 2)//Chain
			return 30576;
		else if (mat == "Bronze" && intHanded == 2)//Chain
			return 30585;
		else if (mat == "Silver" && intHanded == 2)//Chain
			return 30594;
		else if (mat == "Gold" && intHanded == 2)//Chain
			return 30603;
		else if (mat == "Platinum" && intHanded == 2)//Chain
			return 30612;
		else if (mat == "Obsidian" && intHanded == 2)//Chain
			return 30621;
		}
		else if (attribute == "Defensive")
		{
		if (mat == "Tin" && intHanded == 1)//Chain
			return 30630;
		else if (mat == "Aluminum" && intHanded == 1)//Chain
			return 30639;
		else if (mat == "Copper" && intHanded == 1)//Chain
			return 30648;
		else if (mat == "Iron" && intHanded == 1)//Chain
			return 30657;
		else if (mat == "Steel" && intHanded == 1)//Chain
			return 30666;
		else if (mat == "Bronze" && intHanded == 1)//Chain
			return 30675;
		else if (mat == "Silver" && intHanded == 1)//Chain
			return 30684;
		else if (mat == "Gold" && intHanded == 1)//Chain
			return 30693;
		else if (mat == "Platinum" && intHanded == 1)//Chain
			return 30702;
		else if (mat == "Obsidian" && intHanded == 1)//Chain
			return 30711;
		else if (mat == "Tin" && intHanded == 2)//Chain
			return 0;
		else if (mat == "Aluminum" && intHanded == 2)//Chain
			return 0;
		else if (mat == "Copper" && intHanded == 2)//Chain
			return 0;
		else if (mat == "Iron" && intHanded == 2)//Chain
			return 0;
		else if (mat == "Steel" && intHanded == 2)//Chain
			return 0;
		else if (mat == "Bronze" && intHanded == 2)//Chain
			return 0;
		else if (mat == "Silver" && intHanded == 2)//Chain
			return 0;
		else if (mat == "Gold" && intHanded == 2)//Chain
			return 0;
		else if (mat == "Platinum" && intHanded == 2)//Chain
			return 0;
		else if (mat == "Obsidian" && intHanded == 2)//Chain
			return 0;
		}
	}
	else if (type == "Staff")
	{
		if (mat == "Tin" && intHanded == 1)//Staff
			return 30360;
		else if (mat == "Aluminum" && intHanded == 1)//Staff
			return 30369;
		else if (mat == "Copper" && intHanded == 1)//Staff
			return 30378;
		else if (mat == "Iron" && intHanded == 1)//Staff
			return 30387;
		else if (mat == "Steel" && intHanded == 1)//Staff
			return 30396;
		else if (mat == "Bronze" && intHanded == 1)//Staff
			return 30405;
		else if (mat == "Silver" && intHanded == 1)//Staff
			return 30414;
		else if (mat == "Gold" && intHanded == 1)//Staff
			return 30423;
		else if (mat == "Platinum" && intHanded == 1)//Staff
			return 30432;
		else if (mat == "Obsidian" && intHanded == 1)//Staff
			return 30441;
		else if (mat == "Tin" && intHanded == 2)//Staff
			return 30450;
		else if (mat == "Aluminum" && intHanded == 2)//Staff
			return 30459;
		else if (mat == "Copper" && intHanded == 2)//Staff
			return 30468;
		else if (mat == "Iron" && intHanded == 2)//Staff
			return 30477;
		else if (mat == "Steel" && intHanded == 2)//Staff
			return 30486;
		else if (mat == "Bronze" && intHanded == 2)//Staff
			return 30495;
		else if (mat == "Silver" && intHanded == 2)//Staff
			return 30504;
		else if (mat == "Gold" && intHanded == 2)//Staff
			return 30513;
		else if (mat == "Platinum" && intHanded == 2)//Staff
			return 30522;
		else if (mat == "Obsidian" && intHanded == 2)//Staff
			return 30531;
	}
	else if (type == "Axe")
	{
		if (attribute == "Offensive")
		{
		if (mat == "Tin" && intHanded == 1)//Axe
			return 0;
		else if (mat == "Aluminum" && intHanded == 1)//Axe
			return 0;
		else if (mat == "Copper" && intHanded == 1)//Axe
			return 0;
		else if (mat == "Iron" && intHanded == 1)//Axe
			return 0;
		else if (mat == "Steel" && intHanded == 1)//Axe
			return 0;
		else if (mat == "Bronze" && intHanded == 1)//Axe
			return 0;
		else if (mat == "Silver" && intHanded == 1)//Axe
			return 0;
		else if (mat == "Gold" && intHanded == 1)//Axe
			return 0;
		else if (mat == "Platinum" && intHanded == 1)//Axe
			return 0;
		else if (mat == "Obsidian" && intHanded == 1)//Axe
			return 0;
		else if (mat == "Tin" && intHanded == 2)//Axe
			return 30720;
		else if (mat == "Aluminum" && intHanded == 2)//Axe
			return 30729;
		else if (mat == "Copper" && intHanded == 2)//Axe
			return 30738;
		else if (mat == "Iron" && intHanded == 2)//Axe
			return 30747;
		else if (mat == "Steel" && intHanded == 2)//Axe
			return 30756;
		else if (mat == "Bronze" && intHanded == 2)//Axe
			return 30765;
		else if (mat == "Silver" && intHanded == 2)//Axe
			return 30774;
		else if (mat == "Gold" && intHanded == 2)//Axe
			return 30783;
		else if (mat == "Platinum" && intHanded == 2)//Axe
			return 30792;
		else if (mat == "Obsidian" && intHanded == 2)//Axe
			return 30801;
		}
		else if (attribute == "Defensive")
		{
		if (mat == "Tin" && intHanded == 1)//Axe
			return 30810;
		else if (mat == "Aluminum" && intHanded == 1)//Axe
			return 30819;
		else if (mat == "Copper" && intHanded == 1)//Axe
			return 30828;
		else if (mat == "Iron" && intHanded == 1)//Axe
			return 30837;
		else if (mat == "Steel" && intHanded == 1)//Axe
			return 30846;
		else if (mat == "Bronze" && intHanded == 1)//Axe
			return 30855;
		else if (mat == "Silver" && intHanded == 1)//Axe
			return 30864;
		else if (mat == "Gold" && intHanded == 1)//Axe
			return 30873;
		else if (mat == "Platinum" && intHanded == 1)//Axe
			return 30882;
		else if (mat == "Obsidian" && intHanded == 1)//Axe
			return 30891;
		else if (mat == "Tin" && intHanded == 2)//Axe
			return 0;
		else if (mat == "Aluminum" && intHanded == 2)//Axe
			return 0;
		else if (mat == "Copper" && intHanded == 2)//Axe
			return 0;
		else if (mat == "Iron" && intHanded == 2)//Axe
			return 0;
		else if (mat == "Steel" && intHanded == 2)//Axe
			return 0;
		else if (mat == "Bronze" && intHanded == 2)//Axe
			return 0;
		else if (mat == "Silver" && intHanded == 2)//Axe
			return 0;
		else if (mat == "Gold" && intHanded == 2)//Axe
			return 0;
		else if (mat == "Platinum" && intHanded == 2)//Axe
			return 0;
		else if (mat == "Obsidian" && intHanded == 2)//Axe
			return 0;
		}
	}
	else if (type == "Long Blade")
	{
		if (attribute == "Offensive")
		{
			if (mat == "Tin" && intHanded == 1)//Long Blade
				return 11300;
			else if (mat == "Aluminum" && intHanded == 1)//Long Blade
				return 11309;
			else if (mat == "Copper" && intHanded == 1)//Long Blade
				return 11318;
			else if (mat == "Iron" && intHanded == 1)//Long Blade
				return 11327;
			else if (mat == "Steel" && intHanded == 1)//Long Blade
				return 11336;
			else if (mat == "Bronze" && intHanded == 1)//Long Blade
				return 11345;
			else if (mat == "Silver" && intHanded == 1)//Long Blade
				return 11354;
			else if (mat == "Gold" && intHanded == 1)//Long Blade
				return 11363;
			else if (mat == "Platinum" && intHanded == 1)//Long Blade
				return 11372;
			else if (mat == "Obsidian" && intHanded == 1)//Long Blade
				return 11381;
			else if (mat == "Tin" && intHanded == 2)//Long Blade
				return 11390;
			else if (mat == "Aluminum" && intHanded == 2)//Long Blade
				return 11399;
			else if (mat == "Copper" && intHanded == 2)//Long Blade
				return 11408;
			else if (mat == "Iron" && intHanded == 2)//Long Blade
				return 11417;
			else if (mat == "Steel" && intHanded == 2)//Long Blade
				return 11426;
			else if (mat == "Bronze" && intHanded == 2)//Long Blade
				return 11435;
			else if (mat == "Silver" && intHanded == 2)//Long Blade
				return 11444;
			else if (mat == "Gold" && intHanded == 2)//Long Blade
				return 11453;
			else if (mat == "Platinum" && intHanded == 2)//Long Blade
				return 11462;
			else if (mat == "Obsidian" && intHanded == 2)//Long Blade
				return 11471;
		}
		else if (attribute == "Defensive")
		{
			if (mat == "Tin" && intHanded == 1)//Long Blade
				return 11480;
			else if (mat == "Aluminum" && intHanded == 1)//Long Blade
				return 11489;
			else if (mat == "Copper" && intHanded == 1)//Long Blade
				return 11498;
			else if (mat == "Iron" && intHanded == 1)//Long Blade
				return 11507;
			else if (mat == "Steel" && intHanded == 1)//Long Blade
				return 11516;
			else if (mat == "Bronze" && intHanded == 1)//Long Blade
				return 11525;
			else if (mat == "Silver" && intHanded == 1)//Long Blade
				return 11534;
			else if (mat == "Gold" && intHanded == 1)//Long Blade
				return 11543;
			else if (mat == "Platinum" && intHanded == 1)//Long Blade
				return 11552;
			else if (mat == "Obsidian" && intHanded == 1)//Long Blade
				return 11561;
			else if (mat == "Tin" && intHanded == 2)//Long Blade
				return 11570;
			else if (mat == "Aluminum" && intHanded == 2)//Long Blade
				return 11579;
			else if (mat == "Copper" && intHanded == 2)//Long Blade
				return 11588;
			else if (mat == "Iron" && intHanded == 2)//Long Blade
				return 11597;
			else if (mat == "Steel" && intHanded == 2)//Long Blade
				return 11606;
			else if (mat == "Bronze" && intHanded == 2)//Long Blade
				return 11615;
			else if (mat == "Silver" && intHanded == 2)//Long Blade
				return 11624;
			else if (mat == "Gold" && intHanded == 2)//Long Blade
				return 11633;
			else if (mat == "Platinum" && intHanded == 2)//Long Blade
				return 11642;
			else if (mat == "Obsidian" && intHanded == 2)//Long Blade
				return 11651;
		}
	}
	else if (type == "Club")
	{
		if (attribute == "Offensive")
		{
		if (mat == "Tin" && intHanded == 1)//Club
			return 30900;
		else if (mat == "Aluminum" && intHanded == 1)//Club
			return 30909;
		else if (mat == "Copper" && intHanded == 1)//Club
			return 30918;
		else if (mat == "Iron" && intHanded == 1)//Club
			return 30927;
		else if (mat == "Steel" && intHanded == 1)//Club
			return 30936;
		else if (mat == "Bronze" && intHanded == 1)//Club
			return 30945;
		else if (mat == "Silver" && intHanded == 1)//Club
			return 30954;
		else if (mat == "Gold" && intHanded == 1)//Club
			return 30963;
		else if (mat == "Platinum" && intHanded == 1)//Club
			return 30972;
		else if (mat == "Obsidian" && intHanded == 1)//Club
			return 30981;
		else if (mat == "Tin" && intHanded == 2)//Club
			return 30990;
		else if (mat == "Aluminum" && intHanded == 2)//Club
			return 30999;
		else if (mat == "Copper" && intHanded == 2)//Club
			return 31008;
		else if (mat == "Iron" && intHanded == 2)//Club
			return 31017;
		else if (mat == "Steel" && intHanded == 2)//Club
			return 31026;
		else if (mat == "Bronze" && intHanded == 2)//Club
			return 31035;
		else if (mat == "Silver" && intHanded == 2)//Club
			return 31044;
		else if (mat == "Gold" && intHanded == 2)//Club
			return 31053;
		else if (mat == "Platinum" && intHanded == 2)//Club
			return 31062;
		else if (mat == "Obsidian" && intHanded == 2)//Club
			return 31071;
		}
	}
	else
		return null;
	return null;
}




