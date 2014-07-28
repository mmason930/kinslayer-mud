if( !global ) {
	var global = new Object();
}
function initGlobals()
{
	global.trinketCost = 3;
	global.masterWeaponIssue = 8;
	global.masterWeaponRetrieve = 4;
	global.clanCloaks = [];
	global.clanCloaks[2] = 2088;
	global.clanCloaks[3] = 10385;
	global.clanCloaks[4] = 8481;
	global.clanCloaks[6] = 19545;
	global.clanCloaks[7] = 8193;
	global.clanCloaks[9] = 8153;
	global.clanCloaks[11] = 8159;
	global.clanCloaks[12] = 8196;
	global.clanCloaks[13] = 2306;
	global.clanCloaks[14] = 2091;
	global.clanCloaks[16] = 22303;
	global.clanCloaks[17] = 22321;
	global.clanCloaks[18] = 22322;
	global.clanCloaks[19] = 22325;
	global.clanCloaks[20] = 22324;
	global.clanCloaks[21] = 22812;
	global.clanCloaks[22] = 22326;
	global.clanCloaks[23] = 22327;
	global.clanCloaks[25] = 8172;
	global.clanCloaks[27] = 8016;
	global.clanCloaks[28] = 2087;
	global.alderQuestPasswords = ["nevermind","marauder","submerged","swift","deuces","highland","cargo","seabreeze","behold","keepsake","horseshoe","waterfall","melon","blade","hilt","pommel","ruby","stone","livid","spirit","power","onyx","goldenrod","mist","stormy","downpour","jade","mahogany","snowfall","sunshine","light","darkness","eternity","herons","wounded","backstab","traitor","imbecile","foolhardy","meatpie","raven","beak","sparrow","flaming","demise","irridescent","firefly","serenity","absolute","envy","green","red","cherry","apple","bloodied","dragon","reborn","creator","Ishamael","Lanfear","Rahvin","forsaken","darkhound","statue","Callandor","eyeless"];
	global.questMasters = ["Alder","Galnor","Fogel","Ouysa","Koradin"];
	global.cityLeaderNames = ["King Easar","Hammar, the Master of Arms","Murash","Agelmar, Lord of Fal Dara","Shaidar Haran"];
	if ( !global.vQuests )				global.vQuests = [];
		
	if ( !global.vAchievements )		global.vAchievements = [];
	if ( !global.vMixingRecipes )		global.vMixingRecipes = [];
	if ( !global.vSmithingRecipes )		global.vSmithingRecipes = [];
	if ( !global.vTailoringRecipes )	global.vTailoringRecipes = [];
	if ( !global.playerTitleArrays ) 	global.playerTitleArrays = [];
	if ( !global.players )				global.players = [];
	if ( !global.playerRecipes )		global.playerRecipes = [];
	
	
	
	if( !global.dailyQuestsCompleted ) {
		global.dailyQuestsCompleted = new Object();
	}
	
	if( !global.stableTicketMapper ) {
		global.stableTicketMapper = new Object();
		global.stableTicketMapper[ 150 ] = 5002; // Room vnum => ticket vnum
		global.stableTicketMapper[ 1747 ] = 5000;
		global.stableTicketMapper[ 1752 ] = 5000;
		global.stableTicketMapper[ 21380 ] = 5001;
		global.stableTicketMapper[ 13229 ] = 5003;
		global.stableTicketMapper[ 20149 ] = 5004;
		global.stableTicketMapper[ 10363 ] = 5000;
		global.stableTicketMapper[ 4516 ] = 5002;
		global.stableTicketMapper[ 18687 ] = 5002;
	}
	if( !global.stableHorseMapper ) {
		global.stableHorseMapper = new Object();
		global.stableHorseMapper[ 201 ] = 125; // Horse vnum => cost
		global.stableHorseMapper[ 202 ] = 100;
		global.stableHorseMapper[ 209 ] = 75;
		global.stableHorseMapper[ 212 ] = 115;
		global.stableHorseMapper[ 213 ] = 25;
	}
	
	if ( !global.ClanRoom ) {
		constants.CLAN_DHJINNEN = 9;
		constants.CLAN_DREADLORDS = 12;
		constants.CLAN_KOBAL = 7;
		constants.CLAN_KOBOL = 7;
		constants.CLAN_VALON = 6;
		global.ClanRoom = new Object();
		global.ClanRoom[ 117 ] = constants.CLAN_ANDORAN; //clanroom.vnum = clanNum
		global.ClanRoom[ 10385 ] = constants.CLAN_SHIENARAN;
		global.ClanRoom[ 8481 ] = constants.CLAN_MURANDIAN;
		global.ClanRoom[ 8157 ] = constants.CLAN_KOBAL;
		global.ClanRoom[ 8158 ] = constants.CLAN_DHAVOL;
		global.ClanRoom[ 8160 ] = constants.CLAN_KNOMON;
		global.ClanRoom[ 8159 ] = constants.CLAN_GHOBHLIN;
		global.ClanRoom[ 8156 ] = constants.CLAN_DHJINNEN;
		global.ClanRoom[ 8197 ] = constants.CLAN_MYRDDRAAL;
		global.ClanRoom[ 8196 ] = constants.CLAN_DREADLORDS;
		global.ClanRoom[ 8071 ] = constants.CLAN_DREADGUARDS;
		global.ClanRoom[ 23514 ] = constants.CLAN_BLACK_TOWER;
		global.ClanRoom[ 23519 ] = constants.CLAN_BLACK_TOWER;
		global.ClanRoom[ 22200 ] = constants.CLAN_VALON;
		global.ClanRoom[ 22346 ] = constants.CLAN_WHITE_TOWER;
		global.ClanRoom[ 22373 ] = constants.CLAN_YELLOW_AJAH;
		global.ClanRoom[ 22370 ] = constants.CLAN_BROWN_AJAH;
		global.ClanRoom[ 22356 ] = constants.CLAN_WHITE_AJAH;
		global.ClanRoom[ 22359 ] = constants.CLAN_GRAY_AJAH;
		global.ClanRoom[ 22354 ] = constants.CLAN_RED_AJAH;
		global.ClanRoom[ 22362 ] = constants.CLAN_BLUE_AJAH;
		global.ClanRoom[ 22366 ] = constants.CLAN_GREEN_AJAH;
		global.ClanRoom[ 22378 ] = constants.CLAN_WHITE_TOWER;
		global.ClanRoom[ 22331 ] = constants.CLAN_YELLOW_AJAH;
		global.ClanRoom[ 22332 ] = constants.CLAN_BLUE_AJAH;
		global.ClanRoom[ 22333 ] = constants.CLAN_GREEN_AJAH;
		global.ClanRoom[ 22336 ] = constants.CLAN_RED_AJAH;
		global.ClanRoom[ 22337 ] = constants.CLAN_GRAY_AJAH;
		global.ClanRoom[ 22338 ] = constants.CLAN_WHITE_AJAH;
		global.ClanRoom[ 22335 ] = constants.CLAN_BROWN_AJAH;
		global.ClanRoom[ 21111 ] = 5; //TODO: Set to constants.CLAN_MERCHANT_GUILD, currently waiting on a reboot to exist.
	}
	constants.ARENA_OFF = 1;
	constants.ARENA_ON  = 2;
	constants.ARENA_QUEUE = 3;
	constants.ARENA_VOTING = 4;
	constants.ARENA_PLAYING = 5;
	constants.ARENA_BLUE = 1;
	constants.ARENA_RED = 2;
	constants.ARENA_YELLOW = 3;
	constants.ARENA_GREEN = 4;
	if (!global.arenaStage) {
		if (new Date().getDay() != 4)
			global.arenaStage = constants.ARENA_OFF;
		else
			global.arenaStage = constants.ARENA_ON;
	}
	if (!global.arenaMixedBets)
		global.arenaMixedBets = false;
	if (!global.arenaCurrentWagers)
		global.arenaCurrentWagers = [];
	if (!global.arenaZoneVnum)
		global.arenaZoneVnum = 142; //default arena to "The River"
	if (!global.arenaFreeForAll)
		global.arenaFreeForAll = false;
	if (!global.arenaMinToEnter)
		global.arenaMinToEnter = 8;
	if (!global.arenaTotalVotes)
		global.arenaTotalVotes = 0;
	if (!global.arenaKickYes)
		global.arenaKickYes = 0;
	if (!global.arenaKickNo)
		global.arenaKickNo = 0;
	if (!global.arenaEndYes)
		global.arenaEndYes = 0;
	if (!global.arenaEndNo)
		global.arenaEndNo = 0;
	if (!global.arenaTotalPlayers)
		global.arenaTotalPlayers = 0;
	if (!global.arenaAllGames)
		global.arenaAllGames = [];
	if (!global.arenaAllMaps)
		global.arenaAllMaps = [];
	if (!global.arenaImm)
		global.arenaImm = 51;
	if (!global.arenaFlags)
		global.arenaFlags = [];
	/** ARENA MAP OBJECTS **/
	if (typeof arrContains == "undefined" || !arrContains(global.arenaAllMaps, global.arenaMapRiver)) {
		global.arenaMapRiver = new Object();
		global.arenaMapRiver.vnum = 142; //Zone vnum for map
		global.arenaMapRiver.name = "Jungle Fever"; //Map name
		global.arenaMapRiver.min = 14200; //Bottom of zone
		global.arenaMapRiver.max = 14362; //Top of zone
		global.arenaMapRiver.mobDensity = 4; //1 mob per how many rooms in the map
		global.arenaMapRiver.minToEnter = 8;
		global.arenaMapRiver.blueHomeBase = 14222;
		global.arenaMapRiver.redHomeBase = 14298;
		global.arenaMapRiver.yellowHomeBase = 14329;
		global.arenaMapRiver.greenHomeBase = 14309;
		global.arenaMapRiver.yellowCityRooms = [14323,14324,14325,14326,14327,14328,14329];
		global.arenaMapRiver.greenCityRooms = [14315,14314,14313,14306,14307,14308,14309];
		global.arenaMapRiver.blueCityRooms = [14216,14218,14219,14220,14221,14222,14223];
		global.arenaMapRiver.redCityRooms = [14291,14293,14294,14295,14296,14297,14298];
		global.arenaMapRiver.exclusions = [14201, 14299, 14330, 14331, 14332, 14333, 14334, 14335, 14336, 14337]; //Array of room vnums to NOT load mobs in
		global.arenaAllMaps.push(global.arenaMapRiver);
	}
	if (typeof arrContains == "undefined" || !arrContains(global.arenaAllMaps, global.arenaMapValley)) {
		global.arenaMapValley = new Object();
		global.arenaMapValley.vnum = 330; //Zone vnum for map
		global.arenaMapValley.name = "Windy Valley"; //Map name
		global.arenaMapValley.min = 33000; //Bottom of zone
		global.arenaMapValley.max = 33110; //Top of zone
		global.arenaMapValley.mobDensity = 4; //1 mob per how many rooms in the map
		global.arenaMapValley.minToEnter = 6;
		global.arenaMapValley.blueHomeBase = 33024;
		global.arenaMapValley.redHomeBase = 33108;
		global.arenaMapValley.yellowHomeBase = 33011;
		global.arenaMapValley.greenHomeBase = 33100;
		global.arenaMapValley.yellowCityRooms = [33000, 33001, 33002, 33003, 33005, 33011, 33006, 33007, 33008];
		global.arenaMapValley.greenCityRooms = [33012, 33014, 33015, 33013, 33021, 33023, 33022, 33104, 33100];
		global.arenaMapValley.blueCityRooms = [33016, 33017, 33018, 33019, 33020, 33004, 33009, 33010, 33024];
		global.arenaMapValley.redCityRooms = [33047, 33106, 33107, 33108, 33097, 33103, 33045, 33046, 33105];
		global.arenaMapValley.exclusions = [33000,33050, 33048, 33049, 33101, 33098, 33102, 33099]; //Array of room vnums to NOT load mobs in
		global.arenaAllMaps.push(global.arenaMapValley);
	}
	if (typeof arrContains == "undefined" || !arrContains(global.arenaAllMaps, global.arenaMapSpire)) {
		global.arenaMapSpire = new Object();
		global.arenaMapSpire.vnum = 145; //Zone vnum for map
		global.arenaMapSpire.name = "The Spire"; //Map name
		global.arenaMapSpire.min = 14500; //Bottom of zone
		global.arenaMapSpire.max = 14535; //Top of zone
		global.arenaMapSpire.mobDensity = 3; //1 mob per how many rooms in the map
		global.arenaMapSpire.minToEnter = 4;
		global.arenaMapSpire.blueHomeBase = 14531;
		global.arenaMapSpire.redHomeBase = 14535;
		global.arenaMapSpire.yellowHomeBase = 14529;
		global.arenaMapSpire.greenHomeBase = 14533;
		global.arenaMapSpire.blueCityRooms = [14531,14530];
		global.arenaMapSpire.redCityRooms = [14535,14534];
		global.arenaMapSpire.yellowCityRooms = [14529,14528];
		global.arenaMapSpire.greenCityRooms = [14533,14532];
		global.arenaMapSpire.exclusions = [14529,14531,14533,14535]; //Array of room vnums to NOT load mobs in
		global.arenaAllMaps.push(global.arenaMapSpire);
	}
	if (typeof arrContains == "undefined" || !arrContains(global.arenaAllMaps, global.arenaMapHalls)) {
		global.arenaMapHalls = new Object();
		global.arenaMapHalls.vnum = 146; //Zone vnum for map
		global.arenaMapHalls.name = "Halls of Madness"; //Map name
		global.arenaMapHalls.min = 14600; //Bottom of zone
		global.arenaMapHalls.max = 14635; //Top of zone
		global.arenaMapHalls.mobDensity = 3; //1 mob per how many rooms in the map
		global.arenaMapHalls.minToEnter = 4;
		global.arenaMapHalls.blueHomeBase = 14612;
		global.arenaMapHalls.redHomeBase = 14600;
		global.arenaMapHalls.yellowHomeBase = 14606;
		global.arenaMapHalls.greenHomeBase = 14618;
		global.arenaMapHalls.blueCityRooms = [14611,14612,14613];
		global.arenaMapHalls.redCityRooms = [14623,14600,14601];
		global.arenaMapHalls.yellowCityRooms = [14605,14606,14607];
		global.arenaMapHalls.greenCityRooms = [14617,14618,14619];
		global.arenaMapHalls.currentBlockedRooms = [];
		global.arenaMapHalls.exclusions = [14600,14606,14612,14618]; //Array of room vnums to NOT load mobs in
		global.arenaAllMaps.push(global.arenaMapHalls);
	}
	if (typeof arrContains == "undefined" || !arrContains(global.arenaAllMaps, global.arenaMapField)) {
		global.arenaMapField = new Object();
		global.arenaMapField.vnum = 144; //Zone vnum for map
		global.arenaMapField.name = "Battle Field"; //Map name
		global.arenaMapField.min = 14400; //Bottom of zone
		global.arenaMapField.max = 14417; //Top of zone
		global.arenaMapField.mobDensity = 2; //1 mob per how many rooms in the map
		global.arenaMapField.minToEnter = 2;
		global.arenaMapField.blueHomeBase = 14410;
		global.arenaMapField.redHomeBase = 14412;
		global.arenaMapField.yellowHomeBase = 14414;
		global.arenaMapField.greenHomeBase = 14417;
		global.arenaMapField.blueCityRooms = [14410];
		global.arenaMapField.redCityRooms = [14412];
		global.arenaMapField.yellowCityRooms = [14414];
		global.arenaMapField.greenCityRooms = [14417];
		global.arenaMapField.exclusions = [14410,14412,14414,14417,14409,14411,14413,14416,14415,14499]; //Array of room vnums to NOT load mobs in
		global.arenaAllMaps.push(global.arenaMapField);
	}
	if (!global.arenaZones)
		global.arenaZones = [142, 330, 144, 145, 146];
	if (!global.arenaItems)
		global.arenaItems = [14200, 14201, 14202, 14203, 14204, 14205]; // Holds all items having to do with the Arena
	if (!global.arenaStartRooms) //blue   red  yellow  green
		global.arenaStartRooms = [14201, 14299, 14415, 14499];
	if (!global.arenaCurrentMap)
		global.arenaCurrentMap = global.arenaMapRiver; //Default map is "The River"
	/** ARENA GAME TYPES **/
	if (typeof arrContains == "undefined" || !arrContains(global.arenaAllGames, global.arenaGameDM)) {
		global.arenaGameDM = new Object();
		global.arenaGameDM.name = "Death Match";
		global.arenaGameDM.votes = 0; //Number of votes by players
		global.arenaGameDM.respawn = 5; //Respawn time (in seconds)
		global.arenaGameDM.regen = 1; //Percentage hps,sps,dps,mvs you start with at respawn point (*100)
		global.arenaGameDM.limits = [5, 10, 15, 20];
		global.arenaGameDM.limit = 5; //This is different for each game type, here it means number of kills
		global.arenaGameDM.limitType = "Kills  ";
		global.arenaGameDM.goalType = "Rank";
		global.arenaGameDM.info = "In a Death Match, the player or team who reaches the required kill count first wins!";
		global.arenaAllGames.push(global.arenaGameDM);
	}
	if (typeof arrContains == "undefined" || !arrContains(global.arenaAllGames, global.arenaGameTB)) {
		global.arenaGameTB = new Object();
		global.arenaGameTB.name = "Timed Battle";
		global.arenaGameTB.votes = 0; //Number of votes by players
		global.arenaGameTB.respawn = 25; //Respawn time (in seconds)
		global.arenaGameTB.regen = 1; //Percentage hps,sps,dps,mvs you start with at respawn point (*100)
		global.arenaGameTB.limits = [10, 15, 20, 25];	
		global.arenaGameTB.limit = 15; //This is different for each game type, here it means minute limit
		global.arenaGameTB.limitType = "Minutes";
		global.arenaGameTB.goalType = "Rank";
		global.arenaGameTB.timer = 0; //Timed match - This will be set to the limit at the start of the game
		global.arenaGameTB.info = "In a Timed Battle, the player or team with the most kills when time runs out wins!\n\nTies are decided by overall score.";
		global.arenaAllGames.push(global.arenaGameTB);
	}
	if (typeof arrContains == "undefined" || !arrContains(global.arenaAllGames, global.arenaGameCTF)) {
		global.arenaGameCTF = new Object();
		global.arenaGameCTF.name = "Capture the Flag";
		global.arenaGameCTF.votes = 0; //Number of votes by players
		global.arenaGameCTF.respawn = 25; //Respawn time (in seconds)
		global.arenaGameCTF.regen = 1; //Percentage hps,sps,dps,mvs you start with at respawn point (*100)
		global.arenaGameCTF.limits = [2, 3, 5, 7];
		global.arenaGameCTF.limit = 3; //This is different for each game type, here it means number of captures
		global.arenaGameCTF.limitType = "Flags  ";
		global.arenaGameCTF.goalType = "Captured  Returned";
		global.arenaGameCTF.redCaptures = 0;
		global.arenaGameCTF.blueCaptures = 0;
		global.arenaGameCTF.yellowCaptures = 0;
		global.arenaGameCTF.greenCaptures = 0;
		global.arenaGameCTF.redReturns = 0;
		global.arenaGameCTF.blueReturns = 0;
		global.arenaGameCTF.yellowReturns = 0;
		global.arenaGameCTF.greenReturns = 0;
		global.arenaGameCTF.redFlagRoom = getRandomArenaRoom("red");
		global.arenaGameCTF.blueFlagRoom = getRandomArenaRoom("blue");
		global.arenaGameCTF.yellowFlagRoom = getRandomArenaRoom("yellow");
		global.arenaGameCTF.greenFlagRoom = getRandomArenaRoom("green");
		global.arenaGameCTF.info = "The player or team to capture the required number of flags wins!\n\nType CAPTURE FLAG to take the enemy flag, return your flag, and score points.\n\nNotes:\nYou can't hold the flag and a weapon at the same time.\nYou'll travel slower while holding the flag.\nTo score, type CAPTURE FLAG at your home base.";
		global.arenaAllGames.push(global.arenaGameCTF);	
	}
	if (typeof arrContains == "undefined" || !arrContains(global.arenaAllGames, global.arenaGameKH)) {
		global.arenaGameKH = new Object();
		global.arenaGameKH.name = "King of the Hill";
		global.arenaGameKH.votes = 0; //Number of votes by players
		global.arenaGameKH.respawn = 20; //Respawn time (in seconds)
		global.arenaGameKH.regen = 1; //Percentage hps,sps,dps,mvs you start with at respawn point (*100)
		global.arenaGameKH.limits = [3, 5, 7, 9];
		global.arenaGameKH.limit = 5; //This is different for each game type, here it means minutes held
		global.arenaGameKH.limitType = "Minutes";
		global.arenaGameKH.goalType = "Time Held";
		global.arenaGameKH.blueHillTimer = 0;
		global.arenaGameKH.redHillTimer = 0;
		global.arenaGameKH.yellowHillTimer = 0;
		global.arenaGameKH.greenHillTimer = 0;
		global.arenaGameKH.info = "In King of the Hill, a hill will load in a random room on the map.\n\nThe player or team who holds the hill for the required time wins!\n\nTo hold the hill, you must be unengaged.\n\nNotes: \nThe hill will move after every minute held.\nStanding in the rooms around the hill help less than the hill room itself.";
		global.arenaAllGames.push(global.arenaGameKH);
	}
	if (typeof arrContains == "undefined" || !arrContains(global.arenaAllGames, global.arenaGameLMS)) {
		global.arenaGameLMS = new Object();
		global.arenaGameLMS.name = "Last Man Standing";
		global.arenaGameLMS.votes = 0; //Number of votes by players
		global.arenaGameLMS.respawn = 20; //Respawn time (in seconds)
		global.arenaGameLMS.regen = 1; //Percentage hps,sps,dps,mvs you start with at respawn point (*100)
		global.arenaGameLMS.limits = [2, 3, 5, 7];
		global.arenaGameLMS.limit = 5; //This is different for each game type, here it means minutes held
		global.arenaGameLMS.limitType = "Lives  ";
		global.arenaGameLMS.goalType = "Lives Left";
		global.arenaGameLMS.totalPlayers = 0;
		global.arenaGameLMS.timer = 1 // Timer = for LMS, players are forced out of the respawn point every tic to speed up the game.
		global.arenaGameLMS.info = "The Last Man Standing wins!\n\nIf you run out of lives, you are ELIMINATED.\nLeaving the match early will negatively affect your score!";
		global.arenaAllGames.push(global.arenaGameLMS);
	}
	if (typeof arrContains == "undefined" || !arrContains(global.arenaAllGames, global.arenaGameSM)) {
		global.arenaGameSM = new Object();
		global.arenaGameSM.name = "SuperMOBS";
		global.arenaGameSM.votes = 0; //Number of votes by players
		global.arenaGameSM.respawn = 20; //Respawn time (in seconds)
		global.arenaGameSM.regen = 1; //Percentage hps,sps,dps,mvs you start with at respawn point (*100)
		global.arenaGameSM.limits = [10, 15, 20, 25];		
		global.arenaGameSM.limit = 15; //This is different for each game type, here it means minutes
		global.arenaGameSM.limitType = "Minutes";
		global.arenaGameSM.goalType = "Smobs";
		global.arenaGameSM.blueSmobKills = 0;
		global.arenaGameSM.redSmobKills = 0;
		global.arenaGameSM.yellowSmobKills = 0;
		global.arenaGameSM.greenSmobKills = 0;
		global.arenaGameSM.totalSmobs = 0;
		global.arenaGameSM.info = "In a SuperMOBS Match, smobs will load in nearly every room on the map.\n\nThe player or team to kill the most smobs before time runs out wins!";
		global.arenaGameSM.timer = 0; //Timed match - This will be set to the limit at the start of the game
		global.arenaAllGames.push(global.arenaGameSM);
	}
	if (!global.arenaCurrentGame)
		global.arenaCurrentGame = global.arenaGameDM; // Default game type is Death Match	
	if (!global.arenaAIBlue)
		global.arenaAIBlue = [14200, 14201, 14202, 14203, 14204, 14205, 14206, 14207];
	if (!global.arenaAIRed)
		global.arenaAIRed = [14208, 14209, 14210, 14211, 14212, 14213, 14214, 14215];
	if (!global.arenaAIYellow)
		global.arenaAIYellow = [14216, 14217, 14218, 14219, 14220, 14221, 14222, 14223];
	if (!global.arenaAIGreen)
		global.arenaAIGreen = [14224, 14225, 14226, 14227, 14228, 14229, 14230, 14231];
	if (!global.arenaAllAI)
		global.arenaAllAI = [14200, 14201, 14202, 14203, 14204, 14205, 14206, 14207, 14208, 14209, 14210, 14211, 14212, 14213, 14214, 14215, 14216, 14217, 14218, 14219, 14220, 14221, 14222, 14223, 14224, 14225, 14226, 14227, 14228, 14229, 14230, 14231];
	if (!global.arenaAllMobs)
		global.arenaAllMobs = [8202, 18901, 200, 1006, 1500, 1808, 1505];
	if (!global.arenaBlueMobs)
		global.arenaBlueMobs = [20950, 20951, 20956, 19303, 5];
	if (!global.arenaRedMobs)
		global.arenaRedMobs = [20960, 20961, 20966, 1005, 6];
	if (!global.arenaYellowMobs)
		global.arenaYellowMobs = [20960, 20961, 20966, 1005, 6];
	if (!global.arenaGreenMobs)
		global.arenaGreenMobs = [20970, 20971, 18804, 20838, 10259];
	if (!global.arenaMobDensity)
		global.arenaMobDensity = global.arenaCurrentMap.mobDensity; // mobs will appear about every 3 rooms in Arena by default
	if (!global.arenaVotingTimeLimit)
		global.arenaVotingTimeLimit = 3; //Default time for players to vote for game type (minutes)
	if (!global.arenaPlayers)
		global.arenaPlayers = []; //Players currently in the arena
	if (!global.arenaWatchers)
		global.arenaWatchers = []; //Players receiving arena match messages
	if (!global.arenaTeams) {
		if (global.arenaFreeForAll == true)
			global.arenaTeams = 1; //FFA mode on, 1 team
		else
			global.arenaTeams = 4; //FFA mode off, default to all 4 teams
	}
	if (!global.arenaLegends) {
		var arenaLegends = [];
		var queryText = "SELECT u.username, u.last_logon, p.* FROM pvals p JOIN users u ON u.user_id = p.owner_id WHERE p.sKey = 'arenaStats4' AND p.value != '0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~' AND p.value != '0~0~0~0~0~0~0~0~0~0~0~0~0~0~0~0'"; //AND u.last_logon > '2011-08-18 16:30:00' AND u.last_logon <= '2011-08-18 22:30:00' ";
		var queryResult = sqlQuery(queryText);
		while( queryResult.hasNextRow ) {
			var row = queryResult.getRow;
			var score = row.get("value");
			var actorName = row.get("username");
			score = score.split("~");
			var totalScore = 0;
			for (var _autoKey in score) {
				var point = score[_autoKey];
				totalScore += parseInt(point);
			}
			if (totalScore > 0) {
				arenaLegends.push([actorName, totalScore]);
			}
		}
		function compare(a, b) {
			return b[1] - a[1];
		}
		arenaLegends.sort(compare);
		var legendList = [["Unknown",0],["Unknown",0],["Unknown",0],["Unknown",0],["Unknown",0]];
		for (var _autoKey in arenaLegends) {
			var legend = arenaLegends[_autoKey];
			alreadyListed = false;
			var score = legend[1];
			for (i=0; i<legendList.length; i++) {
				if (score > legendList[i][1] && alreadyListed == false) {
					legendList.splice(i,1,legend);
					alreadyListed = true;
				}
			}
		}
		global.arenaLegends = [];
		for (var _autoKey in legendList) {
			var legend = legendList[_autoKey];
			global.arenaLegends.push(legend[0]);

		}
	}
	
	
/** MANORS / OUTPOSTS **/
	loadManors();
	global.innCost = 1000;
	global.barCost = 1000;
	global.warehouseCost = 1000;
	global.portalCost = 1000;
	global.siegeCost = 1000;
	global.gateCost = 1000;
	
/** GLOBAL EVENT JULY 31, 2014 **/

	global.dsAdam = null;
	global.lsAdam = null;
	global.eventIsActive = false;
	global.dsPlayers = [];
	global.lsPlayers = [];
}
















