// This script stores the names of all clan equipment and contains utility methods related to their implementation
// Written by Fogel
if( !clanEquipment ) {
	var clanEquipment = new Object();
}
function initClanEquipment()
{
	clanEquipment.totalBaseItems = 43;
    clanEquipment.baseItems = getArray( clanEquipment.totalBaseItems, constants.CLANS_MAX );
	// Set the vnums for all of the base items
	clanEquipment.baseItems[0][0] = 16;		// a hat with white plumes
	clanEquipment.baseItems[1][0] = 303;	// a worn animal hide vest
	clanEquipment.baseItems[2][0] = 20115;	// golden knots of rank
	clanEquipment.baseItems[3][0] = 109;	// a pair of dark fur sleeves
	clanEquipment.baseItems[4][0] = 608;	// a swordsman's belt
	clanEquipment.baseItems[5][0] = 201;	// a pair of swordsman gloves
	clanEquipment.baseItems[6][0] = 401;	// a pair of dark cloth leggings
	clanEquipment.baseItems[7][0] = 500;	// a pair of gold-fringed boots
	clanEquipment.baseItems[8][0] = 18;		// a gold entwined chainmail coif
	clanEquipment.baseItems[9][0] = 330;	// a snug-fitting jerkin of cured grolm hide
	clanEquipment.baseItems[10][0] = 20106;	// a set of finely worked shoulder pads sparkling with gold
	clanEquipment.baseItems[11][0] = 119;	// a pair of light, scale backed ringmail sleeves
	clanEquipment.baseItems[12][0] = 621;	// a tough worm-hide belt
	clanEquipment.baseItems[13][0] = 230;	// a tough pair of gatorskin gloves
	clanEquipment.baseItems[14][0] = 416;	// a pair of flexible riveted chainmail leggings
	clanEquipment.baseItems[15][0] = 21800;	// a pair of fur-topped, plated chainmail boots
	clanEquipment.baseItems[16][0] = 17;	// a shimmering steel helmet
	clanEquipment.baseItems[17][0] = 310;	// a shimmering steel breastplate
	clanEquipment.baseItems[18][0] = 20113;	// a set of shimmering steel pauldrons
	clanEquipment.baseItems[19][0] = 110;	// a pair of shimmering steel vambraces
	clanEquipment.baseItems[20][0] = 616	// a shimmering steel belt
	clanEquipment.baseItems[21][0] = 210;	// a pair of shimmering steel gauntlets
	clanEquipment.baseItems[22][0] = 412;	// a pair of shimmering steel greaves
	clanEquipment.baseItems[23][0] = 525;	// a pair of steel-bound combat boots
	clanEquipment.baseItems[24][0] = 320;	// a dull gray-green tunic of overlapping metal scales
	clanEquipment.baseItems[25][0] = 20105;	// a pair of shoulder pads worked with silver
	clanEquipment.baseItems[26][0] = 233;	// a pair of grolm skin gloves
	clanEquipment.baseItems[27][0] = 516;	// a pair of soft-soled boots
	clanEquipment.baseItems[28][0] = 1900;	// a crystal trimmed shield
	clanEquipment.baseItems[29][0] = 20116; // a set of spaulders branded with a sleek fox
	clanEquipment.baseItems[30][0] = 352;	// a black bra
	clanEquipment.baseItems[31][0] = 510;	// a pair of steel-plated war boots
	clanEquipment.baseItems[32][0] = 36;	// a black rimmed helmet
	clanEquipment.baseItems[33][0] = 8720;	// a light-weight splinted plate with red etchings
	clanEquipment.baseItems[34][0] = 140;	// a pair of flexed obsidian vambraces
	clanEquipment.baseItems[35][0] = 517;	// a pair of steel plated leather boots
	clanEquipment.baseItems[36][0] = 4313;  // a gold-plated silver full helm
	clanEquipment.baseItems[37][0] = 4314;  // a silver-breasted cuirass bound in gold
	clanEquipment.baseItems[38][0] = 4315;  // a set of curved silver pauldrons plated in gold
	clanEquipment.baseItems[39][0] = 4316;  // a plated silver belt with a golden scabbard
	clanEquipment.baseItems[40][0] = 4317;  // a heavy pair of gold-knuckled silver gauntlets
	clanEquipment.baseItems[41][0] = 4318;  // a pair of silver full-greaves with golden knee caps
	clanEquipment.baseItems[42][0] = 4319;  // a set of ornate silver vambraces worked with gold
	
	// Assign names for each clan's gear
	// Format is "sdesc|namelist|desc"
	
	// Gray Ajah
	// dodge
	clanEquipment.baseItems[2][constants.CLAN_GRAY_AJAH] = "a soft gray shawl of finely woven silk|gray shawl soft finely woven silk|A gray shawl of finely woven silk lies here.";
	
	// Green Ajah
	// dodge
	clanEquipment.baseItems[2][constants.CLAN_GREEN_AJAH] = "a green fringed shawl embroidered with the Flame of Tar Valon|green fringed shawl flame tar valon embroidered|A flame-embroidered green shawl lies here.";
	
	// Brown Ajah
	// dodge
	clanEquipment.baseItems[2][constants.CLAN_BROWN_AJAH] = "a frumpy brown ink smudged shawl|frumpy brown ink smudged shawl|A brown shawl sits here smudged with ink.";
	
	// Blue Ajah
	// dodge
	clanEquipment.baseItems[2][constants.CLAN_BLUE_AJAH] = "a blue fringed shawl embroidered with the Flame of Tar Valon|blue fringed shawl flame tar valon embroidered|A flame-embroidered blue shawl lies here.";
	
	// Yellow Ajah
	// dodge
	clanEquipment.baseItems[2][constants.CLAN_YELLOW_AJAH] = "a finely woven yellow shawl of pure silk|finely woven yellow shawl pure silk|A yellow silk shawl lies here.";
	
	// Yellow Ajah
	// dodge
	clanEquipment.baseItems[2][constants.CLAN_RED_AJAH] = "a red-fringed shawl embroidered with vines|red fringed shawl embroidered vines|A red-fringed shawl lies here.";
	
	// Queen's Guard
	// dodge
	clanEquipment.baseItems[0][constants.CLAN_ANDORAN] = "a padded leather helmet|padded leather helmet|A padded leather helmet lies here.";
	clanEquipment.baseItems[1][constants.CLAN_ANDORAN] = "a trim padded leather vest|red trim padded leather vest|A padded leather vest with red trim is placed off to the side.";
	clanEquipment.baseItems[3][constants.CLAN_ANDORAN] = "a pair of padded leather sleeves|pair padded leather sleeves|A padded pair of leather sleeves lays nearby.";
	clanEquipment.baseItems[4][constants.CLAN_ANDORAN] = "a loose leather belt with metal studs|loose loop leather belt metal studs|A long loop of metal studded leather stretches across the ground.";
	clanEquipment.baseItems[5][constants.CLAN_ANDORAN] = "a pair of dark leather gloves|pair dark leather gloves|A pair of darkly dyed leather gloves lays stacked on top of each other.";
	clanEquipment.baseItems[6][constants.CLAN_ANDORAN] = "a pair of tight dark trousers|pair tight dark trousers|A pair of dark trousers is folded neatly here.";
	clanEquipment.baseItems[7][constants.CLAN_ANDORAN] = "a pair of red and white boots with gold spurs|pair red white boots gold spurs|A red and gold spurred pair of boots stands upright.";
	// abs
	clanEquipment.baseItems[36][constants.CLAN_ANDORAN] = "a conical steel helm with barred face-guard|conical steel helm barred face guard faceguard|A conical helm with a barred face-guard lays on its side.";
	clanEquipment.baseItems[37][constants.CLAN_ANDORAN] = "a red breastplate emblazoned with a white lion|red breast plate breastplate emblazoned white lion|A shining red breastplate lays here.";
	clanEquipment.baseItems[38][constants.CLAN_ANDORAN] = "a pair of pristine steel pauldrons|pair pristine steel pauldrons|A pair of pristine looking pauldrons lies here.";
	clanEquipment.baseItems[42][constants.CLAN_ANDORAN] = "a pair of pristine steel vambraces|pair pristine steel vambraces|A pair of pristine looking vambraces lies here.";
	clanEquipment.baseItems[39][constants.CLAN_ANDORAN] = "a red leather belt plated with steel|red leather belt plated steel|A steel plated loop of red leather lies in a coil.";
	clanEquipment.baseItems[40][constants.CLAN_ANDORAN] = "a pair of pristine steel gauntlets|pair pristine steel gauntlets|A pair of pristine looking gauntlets lies here.";
	clanEquipment.baseItems[41][constants.CLAN_ANDORAN] = "a pair of pristine steel greaves|pair pristine steel greaves|A pair of pristine looking greaves lies here.";
	clanEquipment.baseItems[31][constants.CLAN_ANDORAN] = "a pair of pristine steel boots with golden spurs|pair pristine steel boots golden spurs|A pair of pristine looking boots lies here, golden spurs glistening in the sun.";
	
	// Borderguard
	//combo
	clanEquipment.baseItems[8][constants.CLAN_SHIENARAN] = "a black steel rimmed coif, etched with gold|black steel rimmed coif etched gold|A gold etched coif made from black steel lies here.";
	clanEquipment.baseItems[24][constants.CLAN_SHIENARAN] = "an ornate king's mail hauberk adorned with a golden hawk|ornate king king's kings mail hauberk adorned gold golden hawk|An ornate hauberk adorned with a golden hawk lies here.";
	clanEquipment.baseItems[10][constants.CLAN_SHIENARAN] = "a set of black leather spaulders with tassles of gold |set black leather spaulders tassles tasseled gold|A set of gold tasseled black leather spaulders lies here.";
	clanEquipment.baseItems[11][constants.CLAN_SHIENARAN] = "a set of intricately woven black and gold ringmail sleeves|set intricate intricately woven black gold mail ringmail sleeves|A set of black and gold ringmail sleeves lies here.";
	clanEquipment.baseItems[13][constants.CLAN_SHIENARAN] = "a pair of gold backed splint mail gauntlets|pair gold backed splint mail gauntlets|A pair of gold backed splint mail gauntlets lies here.";
	clanEquipment.baseItems[14][constants.CLAN_SHIENARAN] = "a pair of finely crafted, blackened ringmail leggings|pair finely crafted black blackened ringmail mail leggings|A well-crafted set of black ringmail leggings lies here.";
	clanEquipment.baseItems[15][constants.CLAN_SHIENARAN] = "a pair of plated ringmail boots etched with golden hawks|pair plated ringmail mail boots etched gold golden hawks|A pair of plated ringmail boots etched with golden hawks lies here.";	
	//abs
	clanEquipment.baseItems[36][constants.CLAN_SHIENARAN] = "a black metal armet with a plume of gold|black metal armet plume gold|A black armet with a golden plume lies here on its side.";
	clanEquipment.baseItems[37][constants.CLAN_SHIENARAN] = "a blackened steel cuirass embossed with a golden hawk|black blackened steel cuirass embossed golden hawk|A black steel cuirass, embossed with a shining golden hawk, lies here.";
	clanEquipment.baseItems[38][constants.CLAN_SHIENARAN] = "a pair of black and gold fluted spaulders|pair black gold fluted spaulders|A pair of black and gold spaulders lies here.";
	clanEquipment.baseItems[42][constants.CLAN_SHIENARAN] = "a set of blackened steel vambraces adorned with a golden hawk|set black blackened steel vambraces adorned golden hawk|A set of blackened steel vambraces, adorned with a shining golden hawk, lies here.";
	clanEquipment.baseItems[40][constants.CLAN_SHIENARAN] = "a pair of toughened black steel gauntlets|pair tough toughened black steel gauntlets|A pair of black steel gauntlets lies here.";
	clanEquipment.baseItems[41][constants.CLAN_SHIENARAN] = "a blackened steel greaves engraved in gold|black blackened steel greaves engraved gold|A gold and black pair of greaves lies here.";
	clanEquipment.baseItems[31][constants.CLAN_SHIENARAN] = "a pair of blackened steel war boots with gold spurs|pair black blackened steel war boots gold spurs|A golden spurred pair of black war boots lies here.";
	
	//Murandian Cavalry
	//abs
	clanEquipment.baseItems[36][constants.CLAN_MURANDIAN] = "a white steel helm with a large blue plume|white steel helm helmet large blue plume|A white steel helmet lies here.";
	clanEquipment.baseItems[37][constants.CLAN_MURANDIAN] = "a white steel breastplate engraved with the Red Bull|white steel breastplate engraved red bull plate|A white steel breastplate engraved with a red bull lies here.";
	clanEquipment.baseItems[38][constants.CLAN_MURANDIAN] = "a set of blue-spiked white steel spaulders|blue spiked white steel spaulders|A pair of spiked spaulders lies to the side.";
	clanEquipment.baseItems[42][constants.CLAN_MURANDIAN] = "a pair of flexible white steel vambraces|flexible white steel vambraces|A white set of vambraces lies here.";
	clanEquipment.baseItems[39][constants.CLAN_MURANDIAN] = "a simple belt of plated white steel|simple belt plated white steel|A white steel belt lies here.";
	clanEquipment.baseItems[40][constants.CLAN_MURANDIAN] = "a flexible pair of blue-knuckled white steel gauntlets|blue knuckled blue-knuckled white steel gauntlets flexible|A pair of white steel gauntlets lies here.";
	clanEquipment.baseItems[41][constants.CLAN_MURANDIAN] = "a pair of white steel greaves with blue etching|white steel greaves blue etching|A pair of white steel greaves lies here.";
	clanEquipment.baseItems[31][constants.CLAN_MURANDIAN] = "a pair of white steel riding boots|white steel riding boots|A pair of white steel riding boots lies here.";
	//combo
	clanEquipment.baseItems[8][constants.CLAN_MURANDIAN] = "a polished steel barbute|polished steel barbute helmet visorless|A visorless steel helmet lies here, reflecting all light.";
	clanEquipment.baseItems[9][constants.CLAN_MURANDIAN] = "a blue striped brigandine bearing a red bull|blue striped brigandine bearing red bull cloth plates|A blue striped brigandine bearing a red bull lies here.";
	clanEquipment.baseItems[10][constants.CLAN_MURANDIAN] = "black leather spaulders|black leather spaulders|A pair of black leather spaulders is lies to the side.";
	clanEquipment.baseItems[11][constants.CLAN_MURANDIAN] = "a pair of steel chain sleeves|pair steel chain sleeves|A blue set of chainmail sleeves lies here.";
	clanEquipment.baseItems[12][constants.CLAN_MURANDIAN] = "a polished balteus with a leather sheath|polished balteus steel belt leather sheath|A polished steel sword belt lies here.";
	clanEquipment.baseItems[13][constants.CLAN_MURANDIAN] = "a pair of leather gauntlets with fingers of mail|pair leather gauntlets fingers mail|A pair of mail-fingered leather gauntlets lies here.";
	clanEquipment.baseItems[14][constants.CLAN_MURANDIAN] = "mail chausses with blue linen leggings|mail chausses blue linen leggings|A pair of blue linen leggings lays on top of a mail chausses.";
	clanEquipment.baseItems[15][constants.CLAN_MURANDIAN] = "a pair of polished steel sabaton|pair polished steel sabaton boots|A pair of boots covered with a steel sabaton lies here.";
	//dodge
	clanEquipment.baseItems[0][constants.CLAN_MURANDIAN] = "an argent laced headband crowned with a red bull|argent laced headband band crowned red bull|A headband crowned with a red bull lies here.";
	clanEquipment.baseItems[1][constants.CLAN_MURANDIAN] = "a golden vest downed in silver, crested with a red bull|golden vest silver red bull crested|A golden vest crested with a red bull lies here.";
	clanEquipment.baseItems[2][constants.CLAN_MURANDIAN] = "a set of silver shoulder pads lined in red|silver shoulder pads red|A pair of silver shoulder pads lies to the side.";
	clanEquipment.baseItems[3][constants.CLAN_MURANDIAN] = "a golden set of sleeves lined with silver lace|golden sleeves silver lace|A golden set of sleeves lies here.";
	clanEquipment.baseItems[4][constants.CLAN_MURANDIAN] = "a pure gold belt adorned with a red bull buckle|gold pure belt red bull buckle|A gold belt with a red bull buckle lies here.";
	clanEquipment.baseItems[5][constants.CLAN_MURANDIAN] = "a pair of red riding gloves with argent stitching|pair gloves red riding argent stitching|A pair of red riding gloves lies here.";
	clanEquipment.baseItems[6][constants.CLAN_MURANDIAN] = "a pair of golden lined cloth leggings|golden cloth leggings|A pair golden cloth leggings lies here.";
	clanEquipment.baseItems[7][constants.CLAN_MURANDIAN] = "a pair of leather riding boots trimmed in red|pair leather riding boots trimmed red|A pair of red boots lies here.";
	
	//Ghob'hlin
	//heavy combo
	clanEquipment.baseItems[32][constants.CLAN_GHOBHLIN] = "a steel helmet shaped like a skull|steel helmet shaped skull|A skull-shaped steel helmet lies on the ground.";
	clanEquipment.baseItems[33][constants.CLAN_GHOBHLIN] = "a flexible steel tunic|flexible steel tunic|A flexible steel tunic lies here.";
	clanEquipment.baseItems[29][constants.CLAN_GHOBHLIN] = "a pair of spiked steel spaulders|pair spiked steel spaulders|A pair of spiked steel spaulders lies here.";
	clanEquipment.baseItems[34][constants.CLAN_GHOBHLIN] = "a pair of scaled steel sleeves|pair scaled steel sleeves|A pair of scaled steel sleeves lies here.";
	clanEquipment.baseItems[12][constants.CLAN_GHOBHLIN] = "a hard plate steel belt|hard steel plate belt|A belt of plated steel lies here.";
	clanEquipment.baseItems[13][constants.CLAN_GHOBHLIN] = "a pair of flexible steel gauntlets|pair flexible steel gauntlets|A pair of flexible steel gauntlets lies here.";
	clanEquipment.baseItems[14][constants.CLAN_GHOBHLIN] = "a pair of scaled steel leggings|pair scaled steel leggings|A pair of scaled steel leggings lies here.";
	clanEquipment.baseItems[35][constants.CLAN_GHOBHLIN] = "a pair of boots studded in steel|pair boots studded steel|A pair of steel studded leather boots lies on the ground.";
	//abs
	clanEquipment.baseItems[16][constants.CLAN_GHOBHLIN] = "a fearsome silver warmask with horns of jet|fearsome silver war mask warmask horns jet|A black horned silver mask lies here, flecked with dried blood.";
	clanEquipment.baseItems[17][constants.CLAN_GHOBHLIN] = "a sinuous black steel cuirass with silver clasps|sinuous black steel cuirass silver clasps|A sinuous cuirass of blackened steel lies here face up.";
	clanEquipment.baseItems[18][constants.CLAN_GHOBHLIN] = "a black pair of wickedly spiked pauldrons|black pair wickedly spiked pauldrons|A pair of spiked pauldrons lies off to the side.";
	clanEquipment.baseItems[19][constants.CLAN_GHOBHLIN] = "a pair of black bracers of overlapping steel plates|pair black bracers overlapping steel plates|A pair of sleeves covered with a series of overlapping steel plates lies here.";
	clanEquipment.baseItems[20][constants.CLAN_GHOBHLIN] = "a length of tainted black chain|length tainted black chain belt|A length of tainted black chain lies in a coil.";
	clanEquipment.baseItems[21][constants.CLAN_GHOBHLIN] = "a black pair of clawed war gauntlets|black pair wickedly clawed war gauntlets|A pair of wickedly clawed gauntlets lies on the ground.";
	clanEquipment.baseItems[22][constants.CLAN_GHOBHLIN] = "a black pair of wickedly spiked greaves|black pair wickedly spiked greaves|A pair of blackened steel greaves lies on the ground, spikes protruding towards the sky.";
	clanEquipment.baseItems[23][constants.CLAN_GHOBHLIN] = "a pair of gleaming black war boots|pair gleaming black war boots|A pair of boots of the blackest steal lies here, gleaming in the light.";
	
	//Blademasters
	//combo
	clanEquipment.baseItems[8][constants.CLAN_BLADEMASTERS] = "a silver etched black leather headband|silver etched black leather band headband|A loop of black leather etched with a silver heron lies here.";
	clanEquipment.baseItems[24][constants.CLAN_BLADEMASTERS] = "a dark tunic of overlapping silver scales|dark tunic overlapping silver scales|A dark, scaled tunic of silver lies here.";
	clanEquipment.baseItems[29][constants.CLAN_BLADEMASTERS] = "a set of fitted dark leather pauldrons|set fitted dark leather pauldrons|A darkly colored set of leather pauldrons lies here.";
	clanEquipment.baseItems[11][constants.CLAN_BLADEMASTERS] = "a pair of silver embroidered black leather sleeves|pair silver embroidered black leather sleeves|A pair of black sleeves with silver embroidery lies here.";
	clanEquipment.baseItems[12][constants.CLAN_BLADEMASTERS] = "an ornate black belt engraved with a heron|ornate black belt engraved heron|An ornate belt engraved with a silver heron lies here.";
	clanEquipment.baseItems[13][constants.CLAN_BLADEMASTERS] = "a pair of silver lined black gloves|pair silver lined black gloves|A pair of black gloves with a silver lining lies here.";
	clanEquipment.baseItems[14][constants.CLAN_BLADEMASTERS] = "a tight pair of silver embroidered trousers|tight pair silver embroidered trousers|A pair of trousers with silver embroidery lies here.";
	clanEquipment.baseItems[15][constants.CLAN_BLADEMASTERS] = "a pair of flexible black leather boots|pair flexible black leather boots|A flexible looking pair of black boots lies here.";
	//abs
	clanEquipment.baseItems[16][constants.CLAN_BLADEMASTERS] = "a conical steel helm with a silver barred face-guard|conical steel helm barred face guard faceguard|A conical helm with a barred face-guard lays on its side.";
	clanEquipment.baseItems[17][constants.CLAN_BLADEMASTERS] = "a highly polished silver breastplate with black etching|highly polished silver breast plate breastplate black etching|A silver breastplate etched with black shines in the light.";
	clanEquipment.baseItems[18][constants.CLAN_BLADEMASTERS] = "a set of steel pauldrons woven with short tassels|set steel pauldrons woven short tassels|A tasseled set of steel pauldrons lies here.";
	clanEquipment.baseItems[19][constants.CLAN_BLADEMASTERS] = "a pair of pristine steel vambraces with silver etching|pair pristine steel vambraces silver etching|A pair of pristine looking silver etched vambraces lies here."
	clanEquipment.baseItems[20][constants.CLAN_BLADEMASTERS] = "a thick silver belt with a heron buckle|thick silver belt heron buckle|A thick silver belt with a heron buckle lies here.";
	clanEquipment.baseItems[21][constants.CLAN_BLADEMASTERS] = "a pair of dark black steel gauntlets|pair dark black steel gauntlets|A pair of black steel gauntlets marked with a heron lies here.";
	clanEquipment.baseItems[22][constants.CLAN_BLADEMASTERS] = "a pair of pristine steel greaves with black kneecaps|pair pristine steel greaves black knee caps kneecaps|A pair of pristine looking greaves with black kneecaps lies here.";
	clanEquipment.baseItems[31][constants.CLAN_BLADEMASTERS] = "a pair of pristine steel boots with polished black spurs|pair pristine steel boots polished black spurs|A spurred pair of pristine looking steel boots lies here.";
	//Dreadguards
	//dodge
	clanEquipment.baseItems[0][constants.CLAN_DREADGUARDS] = "a black leather headband, with silver ancient writings|black leather head band headband silver ancient writings|A loop of black leather engraved with silver writing lies here.";
	clanEquipment.baseItems[1][constants.CLAN_DREADGUARDS] = "a black surcoat with silver stitching|black coat surcoat silver stitching|A black surcoat lies here in a heap.";
	clanEquipment.baseItems[2][constants.CLAN_DREADGUARDS] = "a pair of flexible black pauldrons|flexible black pauldrons|A soft looking pair of black pauldrons lies here.";
	clanEquipment.baseItems[3][constants.CLAN_DREADGUARDS] = "a matching set of black sleeves with silver stitching that shimmers in the light|matching set black sleeves silver stiching shimmer light|A set of black sleeves with silver stitching lies here.";
	clanEquipment.baseItems[4][constants.CLAN_DREADGUARDS] = "a braided black leather belt with a silver cuendillar buckle|braided black leather belt silver cuendillar buckle|A braided loop of black leather lies here.";
	clanEquipment.baseItems[5][constants.CLAN_DREADGUARDS] = "a pair of silver gloves with intricate black stitching|silver gloves intricate black stitching|A pair of black-stitched silver gloves lies here.";
	clanEquipment.baseItems[6][constants.CLAN_DREADGUARDS] = "a set of long black leggings with silver stitchwork|long black leggings silver stitch stitchwork|A long pair of black and silver leggings lies here.";
	clanEquipment.baseItems[7][constants.CLAN_DREADGUARDS] = "a pair of black leather highboots with silver laces|black leather high boots highboots silver laces|A pair of black and silver leather boots stands upright.";
	clanEquipment.baseItems[30][constants.CLAN_DREADGUARDS] = "a black surcoat with silver stitching|black coat surcoat silver stitching|A black surcoat lies here in a heap.";
	//combo
	clanEquipment.baseItems[8][constants.CLAN_DREADGUARDS] = "a black coif with a hanging silver veil|black coif hanging silver veil|A silver veiled black coif lies here.";
	clanEquipment.baseItems[9][constants.CLAN_DREADGUARDS] = "a black hauberk with silver detailings|black hauberk silver detailings|A black and silver hauberk lies here.";
	clanEquipment.baseItems[10][constants.CLAN_DREADGUARDS] = "a grand mantle of black and silver feathers|grand mantle black silver feathers|A large, feathered mantle lies in a heap.";
	clanEquipment.baseItems[11][constants.CLAN_DREADGUARDS] = "a set of silver ringmail sleeves with black cufflinks|set silver ring mail ringmail sleeves black cuff links cufflinks|A set of silver ringmail sleeves with black cufflinks lies here.";
	clanEquipment.baseItems[12][constants.CLAN_DREADGUARDS] = "a black leather belt with a silver cuendillar buckle|black leather belt silver cuendillar buckle|A braided loop of black leather lies here.";
	clanEquipment.baseItems[13][constants.CLAN_DREADGUARDS] = "a set of silver knuckled gauntlets of blackened leather|silver knuckled gauntlets blackened black leather|A set of silver knuckled black gauntlets lies here.";
	clanEquipment.baseItems[14][constants.CLAN_DREADGUARDS] = "a pair of black ringmail leggings with a dull silver codpiece|black ring mail ringmail leggings dull silver cod piece codpiece|A pair of black ringed leggings with an attached codpiece lies here.";
	clanEquipment.baseItems[15][constants.CLAN_DREADGUARDS] = "a pair of knee-high blackened boots|pair knee high knee-high kneehigh blackened boots|A tall pair of black boots lies on the ground.";
	
	//Myrddraal
	//dodge
	clanEquipment.baseItems[0][constants.CLAN_MYRDDRAAL] = "a black shadow-forged hood|black shadow forged shadow-forged hood|A hood of pitch black lies obscured in shadow.";
	clanEquipment.baseItems[1][constants.CLAN_MYRDDRAAL] = "a black shadow-forged vest|black shadow forged shadow-forged vest|A black vest lies here obscured in shadow.";
	clanEquipment.baseItems[2][constants.CLAN_MYRDDRAAL] = "a pair of black shadow-forged steel knots|pair black shadow forged shadow-forged steel knots|A black set of steel knots lies obscured in shadow.";
	clanEquipment.baseItems[3][constants.CLAN_MYRDDRAAL] = "a pair of black shadow-forged steel sleeves|pair black shadow forged shadow-forged steel sleeves|A pair of black steel sleeves lies obscured in shadow.";
	clanEquipment.baseItems[4][constants.CLAN_MYRDDRAAL] = "a black shadow-forged steel belt|black shadow forged shadow-forged steel belt|A loop of black steel lies obscured in shadow.";
	clanEquipment.baseItems[5][constants.CLAN_MYRDDRAAL] = "a pair of black shadow-forged steel gloves|pair black shadow forged shadow-forged steel gloves|A black pair of steel gloves lies obscured in shadow.";
	clanEquipment.baseItems[6][constants.CLAN_MYRDDRAAL] = "a pair of black shadow-forged steel leggings|pair black shadow forged shadow-forged steel leggings|A pair of black steel leggings lies obscured in shadow.";
	clanEquipment.baseItems[27][constants.CLAN_MYRDDRAAL] = "a pair of black shadow-forged steel boots|pair black shadow forged shadow-forged steel boots|A pair of black steel boots lies obscured in shadow.";
	clanEquipment.baseItems[28][constants.CLAN_MYRDDRAAL] = "a black shadow-forged shield|black shadow forged shadow-forged shield|A pitch black shield lies obscured in shadow.";
	
	//...
}
initClanEquipment();
function findClanEquipmentIndex( vnum )
{
	for ( var index = 0; index < clanEquipment.totalBaseItems; index++ )
	{
		if ( clanEquipment.baseItems[index][0] == vnum )
		{
			return index;
		}
	}
	return -1;
}
function getClanGearSDesc( clan, vnum )
{
	return clanEquipment.baseItems[findClanEquipmentIndex( vnum )][clan].split("|")[0];
}
function getClanGearName( clan, vnum )
{
	return clanEquipment.baseItems[findClanEquipmentIndex( vnum )][clan].split("|")[1];
}
function getClanGearDesc( clan, vnum )
{
	return clanEquipment.baseItems[findClanEquipmentIndex( vnum )][clan].split("|")[2];
}
function hasClanEquivalent( clan, vnum )
{
	var index = findClanEquipmentIndex( vnum );
	if( index >= 0 && clanEquipment.baseItems[index][clan] )
	{
		return true;
	}
	return false;
}





