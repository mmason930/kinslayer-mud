var MercUtil = (function(MercUtil) {

	MercUtil.MERCENARY_LIST_PVAL_NAME = "mercenaryList";
	MercUtil.MAX_MERCENARIES = 5;
	MercUtil.MAX_MERCENARIES_PER_DEPOT = 10;
	MercUtil.MAX_MERCENARIES_OWNED = 2;
	MercUtil.DUMPING_ROOM_VNUM = 25;
	MercUtil.NUMBER_OF_TIMES_LOADED = MercUtil.NUMBER_OF_TIMES_LOADED ? MercUtil.NUMBER_OF_TIMES_LOADED : 0;

	if(MercUtil.NUMBER_OF_TIMES_LOADED == 0) {

		MercUtil.depotRooms = {
			8081: {//
				roomVnum: 8081,
				race: constants.RACE_TROLLOC,
				mercenaryArray: [],
				mercenariesInPlay: []
			},
			388: {
				roomVnum: 388,
				race: constants.RACE_HUMAN,
				mercenaryArray: [],
				mercenariesInPlay: []
			}
		};
	}

	function MercObj(className, race) {
		this.maxSpell = 3; //Spell bonus -- adds to max_maxSpell (which is really just an sval)
		/**********************************************************************
		 * Class:		Skills:												  *
		 *--------------------------------------------------------------------*
		 * Channeler	Healing[0]											  *
		 * Warrior		Search[2], Precise Strike[3], Shield Block[4]		  *
		 * Ranger		Precise Strike[3], Shield Block[4], Pick[5]			  *
		 * Thief		Bash[1], Search[2]									  *
		 **********************************************************************/

		this.skillsList = ["Healing","Bash","Search","Precise Strike","Shield Block","Pick","Channeling"];
		this.skillBonus = 4; //Multiplier for skills(channeling or other class skills)
		this.skill = -1; //Which skill does the merc possess?
		this.str = 15;
		this.intel = 15;
		this.wis = 15;
		this.dex = 15;
		this.con = 15;
		this.className = className; //This is actually an int corresponding to constants.CLASS_XXX
		this.chan = false; //Merc can channel?
		if(this.className == constants.CLASS_CHANNELER || this.className == constants.DREADGUARD) {
			/** Channeler Stats**/
			if(race == constants.RACE_HUMAN) {
				this.vnum = 18603;
			}
			else if(race == constants.RACE_TROLLOC){
				this.vnum = 18607;
			}
			if(random(-2,1) > 0) {
				this.skill = random(-3,0);
			}
			this.wis = 19 + random(-1,2);
			this.intel = 19 + random(-1,2);
			this.dex = 16 + random(-1,1);
			this.str = 15 + random(-3,1);
			this.con = 15 + random(-2,1);
			this.chan = true;
		}
		else if(this.className == constants.CLASS_WARRIOR) {
			/** Warrior Stats **/
			if(race == constants.RACE_HUMAN) {
				this.vnum = 18600;
			}
			else if(race == constants.RACE_TROLLOC){
				this.vnum = 18604;
			}
			if(random(-2,1) > 0) {
				this.skill = random(2,4);
			}
			this.str = 18 + random(0,4);
			this.intel = 15 + random(-2,2);
			this.wis = 15 + random(-2,2);
			this.dex = 15 + random(-4, 1);
			this.con = 20 + random(-1,2);
		}
		else if(this.className == constants.CLASS_RANGER) {
			/** Ranger Stats **/
			if(race == constants.RACE_HUMAN) {
				this.vnum = 18601;
			}
			else if(race == constants.RACE_TROLLOC){
				this.vnum = 18605;
			}
			if(random(-2,1) > 0) {
				this.skill = random(3,5);
			}
			this.str = 17 + random(-1,3);
			this.intel = 15 + random(-1,4);
			this.wis = 17 + random(-2,3);
			this.dex = 17 + random(-2,4);
			this.con = 17 + random(0,4);
		}
		else if(this.className == constants.CLASS_THIEF) {
			/** Thief Stats **/
			if(race == constants.RACE_HUMAN) {
				this.vnum = 18602;
			}
			else if(race == constants.RACE_TROLLOC){
				this.vnum = 18606;
			}
			if(random(-2,1) > 0) {
				this.skill = random(1,2);
			}
			this.str = 15 + random(0,4);
			this.intel = 15 + random(0,3);
			this.wis = 13 + random(-2,2);
			this.dex = 18 + random(0,4);
			this.con = 17 + random(0,4);
		}
		if(this.skill == this.skillsList.length - 1) { //Skill: Channeling
			this.chan = true;
		}
		this.name = getMobName(this.vnum);
		this.hps = 16*this.con + 4*this.wis + 2*this.intel;
		this.mvs = Math.floor(3.5*this.con + 5.5*this.dex); //for trollocs add 100
		if(this.race == constants.RACE_TROLLOC) {
			this.mvs += this.mvs;
		}
		if(this.chan) {
			this.maxSpell = 5*this.wis + 4*this.intel + this.con;
			this.sps = this.maxSpell;
		}
		this.skillBonus = Math.floor((5*this.wis + 3*this.intel + 2*this.dex)/10) + random(-4,4);
		this.price = function() {
			/* Calculate the merc's price based on multiple stats and bonsuses
			 * and number of historically purchased mercs of that class */
			var base = 0; //ADJUST TO AFFECT BASE PRICE OF ALL MERCS
			var price = base + 2*this.str + this.intel + this.wis + 2*this.dex + 3*this.con + 3*parseInt(fread("purchHistClass" + this.className))/50;
			if(this.chan) {
				/**** CHANGE THIS TO AFFECT PRICE OF CHANNELER MERCS ****/
				price *= 5.25;
			}
			if(this.skill > -1) {
				price *= 3;
			}
			return Math.floor(price);
		};

	}

	MercUtil.getMercenaryArray = function(roomVnum) {

		return this.depotRooms.hasOwnProperty(roomVnum) ? this.depotRooms[roomVnum].mercenaryArray : null;
	};

	MercUtil.getMercenariesInPlayArray = function(roomVnum) {

		return this.depotRooms.hasOwnProperty(roomVnum) ? this.depotRooms[roomVnum].mercenariesInPlay : null;
	};

	MercUtil.removeMercenaryFromDepot = function(roomVnum, myMercObj) {

		var array = this.getMercenaryArray(roomVnum);

		if(array)
		{
			var index = array.indexOf(myMercObj);

			if(index != -1)
				array.splice(index, 1);
		}
	};

	MercUtil.addMercenaryToDepot = function(roomVnum, myMercObj) {

		var array = this.getMercenaryArray(roomVnum);

		if(array)
			array.push(myMercObj);
	};

	MercUtil.clearMercenaryArray = function(roomVnum) {

		var mercenaryArray = this.getMercenaryArray(roomVnum);

		if(mercenaryArray != null)
			mercenaryArray.length = 0;
	};

	MercUtil.populateMercenaryArray = function(roomVnum) {

		var mercenaryArray = this.getMercenaryArray(roomVnum);
		var race = this.depotRooms[roomVnum].race;

		if(mercenaryArray == null)
			return false;

		mercenaryArray.length = 0;

		for(var i = 0;i < this.MAX_MERCENARIES_PER_DEPOT;++i) {
			// Pick a class for each mercenary
			var randClass = random(constants.CLASS_WARRIOR, constants.CLASS_CHANNELER-1); //remove "-1" when channies go in
			if(randClass == constants.CLASS_CHANNELER) {
				//Reduce number of CHANNELER loads
				if(random(constants.CLASS_WARRIOR, constants.CLASS_CHANNELER) != constants.CLASS_CHANNELER) {
					randClass = random(constants.CLASS_WARRIOR, constants.CLASS_RANGER);
				}
			}

			mercenaryArray.push(new MercObj(randClass, race));
		}

		return true;
	};

	MercUtil.removeMercenaryFromInPlay = function(roomVnum, mercenaryData)
	{
		var mercenariesInPlay = this.getMercenariesInPlayArray(roomVnum);
		if(mercenariesInPlay)
		{
			var index = mercenariesInPlay.indexOf(mercenaryData);

			if(index != -1)
				mercenariesInPlay.splice(index, 1);
		}
	};

	MercUtil.addMercenaryToInPlay = function(roomVnum, mercenaryData)
	{
		var mercenariesInPlay = this.getMercenariesInPlayArray(roomVnum);
		if(mercenariesInPlay)
			mercenariesInPlay.push(mercenaryData);
	};

	MercUtil.getSavedListPage = function(actor) {

		return getSval(actor, -18600, "savedListPage");
	};

	MercUtil.getMercenariesInPlay = function(roomVnum)
	{
		if(!this.depotRooms.hasOwnProperty(roomVnum))
			return 0;

		return this.depotRooms[roomVnum].mercenariesInPlay;
	};

	MercUtil.setSavedListPage = function(actor, page) {

		setSval(actor, -18600, "savedListPage", page);
	};

	MercUtil.getDepotRoomVnum = function(raceId) {
		return this.DEPOT_ROOMS[raceId];
	};

	MercUtil.getMercArray = function(room) {
		return getSval(room, -18600, "mercArray");
	};

	MercUtil.setMercArray = function(room, mercArray) {

		setSval(room, -18600, "mercArray", mercArray);
	};

	MercUtil.getMaximumMercenaries = function() {
		return this.MAX_MERCENARIES;
	};

	MercUtil.getMaximumMercenariesOwned = function() {
		return this.MAX_MERCENARIES_OWNED;
	}

	MercUtil.getDumpingRoomVnum = function() {
		return this.DUMPING_ROOM_VNUM;
	};

	MercUtil.generateMercenaryId = function(owner, mercenary) {

		return owner.id + "-" + MercUtil.getMercenaryNumber(mercenary);
	};

	MercUtil.getMercenaryOwnerType = function() {

		return "M";
	};

	MercUtil.getMercenaryData = function(mercenary) {
		return getSval(mercenary, -18600, "mercenaryData");
	};

	MercUtil.setMercenaryData = function(mercenary, mercenaryData) {
		setSval(mercenary,  -18600, "mercenaryData", mercenaryData);
	};

	MercUtil.getOwner = function(mercenary) {

		return getSval(mercenary, 18600, "owner");
	};

	MercUtil.setOwner = function(mercenary, owner) {

		setSval(mercenary, 18600, "owner", owner);
	};

	MercUtil.getPrice = function(mercenary) {

		return getSval(self, 18600, "price");
	};

	MercUtil.setPrice = function(mercenary, price) {

		setSval(mercenary, 18600, "price", price);
	};

	MercUtil.getMercenaryNumber = function(mercenary) {

		return getSval(mercenary, 18600, "mercNum");
	};

	MercUtil.setMercenaryNumber = function(mercenary, number) {

		setSval(mercenary, 18600, "mercNum", number);
	};

	MercUtil.findNextMercenaryNumber = function(owner) {

		var mercenaryArray = this.getMercenaryNumberArray(owner);
		return mercenaryArray.length == 0 ? 1 : Number(mercenaryArray[mercenaryArray.length - 1]) + 1;
	};

	MercUtil.getHomeRoom = function(mercenary) {

		return getSval(mercenary, 18600, "homeRoom");
	};

	MercUtil.setHomeRoom = function(mercenary, homeRoom) {

		setSval(mercenary, 18600, "homeRoom", homeRoom);
	};

	MercUtil.getMercenaryNumberArray = function(owner) {

		var currentMercenaries = owner.getPval(this.MERCENARY_LIST_PVAL_NAME);
		return currentMercenaries == null ? [] : currentMercenaries.split(",").sort(function(a,b) { return Number(a) > Number(b) });
	};

	MercUtil.getRendezvousRoom = function(mercenary) {
		return getSval(mercenary, 18605, "mercRendezvousRoom");
	};

	MercUtil.setRendezvousRoom = function(mercenary, roomObject) {

		setSval(mercenary, 18605, "mercRendezvousRoom", roomObject);
	};

	MercUtil.addMercenaryToOwner = function(mercenary, owner, price) {

		var currentMercenaries = owner.getPval(this.MERCENARY_LIST_PVAL_NAME);
		var mercenaryNumber = this.findNextMercenaryNumber(owner);

		this.setMercenaryNumber(mercenary, mercenaryNumber);
		this.setPrice(mercenary, price);
		this.setOwner(mercenary, owner);

		if(currentMercenaries == null)
			currentMercenaries = mercenaryNumber;
		else
			currentMercenaries += "," + mercenaryNumber;

		owner.setPval(this.MERCENARY_LIST_PVAL_NAME, currentMercenaries, true);

		MercUtil.saveMercenaryEquipment(mercenary);
		owner.attach(18614);

		var homeRoom = this.getHomeRoom(mercenary);
		this.addMercenaryToInPlay(homeRoom.vnum, this.getMercenaryData(mercenary));
		this.removeMercenaryFromDepot(homeRoom.vnum, this.getMercenaryData(mercenary));

		return mercenaryNumber;
	};

	MercUtil.removeMercenaryFromOwner = function(mercenary, owner) {

		var mercenaryArray = this.getMercenaryNumberArray(owner);
		var mercenaryData = MercUtil.getMercenaryData(mercenary);
		var index = mercenaryArray.indexOf(MercUtil.getMercenaryNumber(mercenary));
		if(index != -1)
		{
			mercenaryArray.splice(index, 1);

			if(mercenaryArray.length == 0)
				owner.deletePval(this.MERCENARY_LIST_PVAL_NAME);
			else
				owner.setPval(this.MERCENARY_LIST_PVAL_NAME, mercenaryArray.join(","), true);
		}

		var homeRoom = this.getHomeRoom(mercenary);
		this.addMercenaryToDepot(homeRoom.vnum, mercenaryData);
		this.removeMercenaryFromInPlay(homeRoom.vnum, mercenaryData);
	};

	MercUtil.extractMercenary = function(mercenary) {

		mercenary.extract();
	};

	MercUtil.extractAndSaveMercenary = function(mercenary) {

		this.saveMercenaryEquipment(mercenary);

		var extractionRoom = getRoom(this.getDumpingRoomVnum());
		mercenary.moveToRoom(extractionRoom);
		mercenary.extract();
		getRoom(extractionRoom).items.forEach(function(item) { item.extract()	});
	};

	MercUtil.clearSavedMercenaryEquipment = function(mercenary) {

		var owner = this.getOwner();
		saveTopLevelHolderItems(this.getMercenaryOwnerType(), this.generateMercenaryId(mercenary, owner), []);
	};

	MercUtil.saveMercenaryEquipment = function(mercenary) {

		var owner = this.getOwner(mercenary);
		var items = mercenary.inventory;

		for(var itemWearPosition = 0;itemWearPosition < constants.NUM_WEARS;++itemWearPosition)
		{
			var wornItem = mercenary.eq(itemWearPosition);
			if(wornItem)
				items.push(wornItem);
		}

		//With our list of equipment and inventory, we can now save.
		saveTopLevelHolderItems(this.getMercenaryOwnerType(), this.generateMercenaryId(mercenary, owner), items);
	};

	MercUtil.retrieveMercenaryEquipment = function(mercenary, owner) {

		return loadObjectsByHolderFromDatabase(this.getMercenaryOwnerType(), this.generateMercenaryId(mercenary, owner));
	};

	MercUtil.reloadMercenaryArrays = function() {

		for (var depotRoomVnum in this.depotRooms) {
			MercUtil.populateMercenaryArray(depotRoomVnum);
		}
	};

	if(MercUtil.NUMBER_OF_TIMES_LOADED == 0) {

		MercUtil.reloadMercenaryArrays();
	}

	++MercUtil.NUMBER_OF_TIMES_LOADED;

	return MercUtil;
})(MercUtil || {});
