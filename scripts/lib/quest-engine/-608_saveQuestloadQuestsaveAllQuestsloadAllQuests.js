var QUEST_DATA_TABLE = "quest";
var QUEST_DIALOGUE_TABLE = "questDialogue";
var QUEST_ITEMS_TABLE = "questItem";
var QUEST_TASK_TABLE = "questTask";
var QUEST_REQUIREMENT_TABLE = "questRequirement";
var QUEST_EXTRAS_TABLE = "questExtra";
var QUEST_TAGS_TABLE = "questTag";
var QUEST_OWNER_VNUM_TABLE = "questOwner";
var QUEST_SKILL_ARRAY_SEPARATOR = "~=~";
var QUEST_ITEM_REQUIRED_TOKEN = "A";
var QUEST_ITEM_REWARD_TOKEN = "B";
var QUEST_EDITORS_SEPARATOR = ";";
function loadAllQuests()
{
	mudLog(constants.BRF, 102, "Loading quests...");
	var sQuery;
	var vQuests = [];
	Quest.clearQuests();
	
	sqlQuery("DELETE FROM " + QUEST_DIALOGUE_TABLE + " WHERE quest_id NOT IN(SELECT id FROM " + QUEST_DATA_TABLE + ");");
	sqlQuery("DELETE FROM " + QUEST_ITEMS_TABLE + " WHERE quest_id NOT IN(SELECT id FROM " + QUEST_DATA_TABLE + ");");
	sqlQuery("DELETE FROM " + QUEST_TASK_TABLE + " WHERE quest_id NOT IN(SELECT id FROM " + QUEST_DATA_TABLE + ");");
	sqlQuery("DELETE FROM " + QUEST_OWNER_VNUM_TABLE + " WHERE quest_id NOT IN(SELECT id FROM " + QUEST_DATA_TABLE + ");");
	sqlQuery("DELETE FROM " + QUEST_REQUIREMENT_TABLE + " WHERE quest_id NOT IN(SELECT id FROM " + QUEST_DATA_TABLE + ");");
	sqlQuery("DELETE FROM " + QUEST_EXTRAS_TABLE + " WHERE quest_id NOT IN(SELECT id FROM " + QUEST_DATA_TABLE + ");");
	sqlQuery("DELETE FROM " + QUEST_TAGS_TABLE + " WHERE quest_id NOT IN(SELECT id FROM " + QUEST_DATA_TABLE + ");");
	
	sQuery = "SELECT * FROM " + QUEST_DATA_TABLE + " ORDER BY id ASC;";
	var rs = sqlQuery( sQuery );
	
	sQuery = "SELECT * FROM " + QUEST_DIALOGUE_TABLE + " ORDER BY quest_id ASC,position ASC;";
	var rsDialogue = sqlQuery( sQuery );
	
	sQuery = "SELECT * FROM " + QUEST_ITEMS_TABLE + " ORDER BY quest_id ASC,position ASC;";
	var rsItems = sqlQuery( sQuery );
	
	sQuery = "SELECT * FROM " + QUEST_TASK_TABLE + " ORDER BY quest_id ASC,position ASC, unlockerIndex ASC;";
	var rsTasks = sqlQuery( sQuery );
	
	sQuery = "SELECT * FROM " + QUEST_OWNER_VNUM_TABLE + " ORDER BY quest_id ASC;";
	var rsOwners = sqlQuery( sQuery );
	
	sQuery = "SELECT * FROM " + QUEST_REQUIREMENT_TABLE + " ORDER BY quest_id ASC,position ASC;";
	var rsReqs = sqlQuery( sQuery );
	
	sQuery = "SELECT * FROM " + QUEST_EXTRAS_TABLE + " ORDER BY quest_id ASC";
	var rsExtras = sqlQuery( sQuery );
	
	sQuery = "SELECT * FROM " + QUEST_TAGS_TABLE + " ORDER BY quest_id ASC,position ASC;";
	var rsTags = sqlQuery( sQuery );
	
	while( rs.hasNextRow ) {
		var row = rs.getRow;
		var quest = new Quest();
		
		quest.name = row.get("questName");
		quest.qp = parseInt( row.get("qpReward") );
		quest.fp = parseInt( row.get("fpReward") );
		quest.exp = parseInt( row.get("expReward") );
		quest.gold = parseInt( row.get("goldReward") );
		quest.maxCompletions = parseInt( row.get("numCanComplete") );
		quest.summary = row.get("strSummary");
		quest.skills = _.pull(row.get("skillArray").split(QUEST_SKILL_ARRAY_SEPARATOR), "");
		quest.unlockRank = parseInt( row.get("unlockRank") );
		quest.showGivenBy = parseInt( row.get("showGivenBy") );
		quest.databaseID = parseInt( row.get("id") );
		quest.editors = row.get("editors").split(QUEST_EDITORS_SEPARATOR);

		var strNums = row.get("open").split(";");
		for ( var i = 0; i < strNums.length; i++ ) {
			if ( isNumber(strNums[i]) && strNums[i].length > 0 )
				quest.allowedRaces.push(parseInt(strNums[i]));
		}
		
		/**** Load each dialogue ****/
		while( rsDialogue.hasNextRow ) {
			var row = rsDialogue.peekRow;
			if( parseInt( row.get("quest_id") ) == quest.databaseID ) {

				var dlg = new Quest.Dialogue( parseInt(row.get("timeout")), row.get("dialogue") );
				quest.dialogue.push( dlg );
			}
			else break;
			rsDialogue.skipRow();
		}
		
		/**** Load each task ****/
		while( rsTasks.hasNextRow ) {
			var row = rsTasks.peekRow;
			if(parseInt(row.get("quest_id")) == quest.databaseID) {
				//var vTask = [ row.get("taskName"), row.get("taskProgress"), row.get("taskRequirement"), row.get("unlockerIndex") ];
				// Since we order by position, pushing will guarantee correct order of tasks array
				quest.tasks.push(new Quest.Task(row.get("id"), quest, row.get("taskName"), row.get("taskProgress"), row.get("taskRequirement"), row.get("unlockerIndex")));
			}
			else break;
			rsTasks.skipRow();
		}
		/**** Load each owner vnum ****/
		while( rsOwners.hasNextRow ) {
			var row = rsOwners.peekRow;
			if( parseInt( row.get("quest_id") ) == quest.databaseID ) {
				var vOwner = parseInt( row.get("owner_vnum") );
				quest.ownerVnums.push( vOwner );
			}
			else break;
			rsOwners.skipRow();
		}
		/**** Load each item ****/
		var rewardSlots = {};
		var autoSlotNum = 1;
		var getKey = function(el) {return el.tier;};
		while( rsItems.hasNextRow ) {
			var row = rsItems.peekRow;
			if( parseInt( row.get("quest_id") ) == quest.databaseID ) {
				if( row.get("itemType") == QUEST_ITEM_REQUIRED_TOKEN ) {
					var vItem = [ row.get("item"), parseInt(row.get("itemCount")) ];
					quest.items.push( vItem );
				}
				else if( row.get("itemType") == QUEST_ITEM_REWARD_TOKEN ) {
					var slotId = row.get("slot_id").toString() || "Slot " + autoSlotNum++;
					// Each key in itemRewards maps to an array of items for a given slot
					if (!rewardSlots.hasOwnProperty(slotId)) {
						rewardSlots[slotId] = [];
					}
					var slotTier = parseInt(row.get("slot_tier"));
					var vItem = {
						vnum: parseInt(row.get("item")),
						count: parseInt(row.get("itemCount")),
						loadPercent: parseInt(row.get("loadPercent")),
						id: slotId,
						tier: slotTier
					};
					var isRetooled = Boolean(parseInt(row.get("isRetooled")));
					vItem.isRetooled = isRetooled;
					if ( isRetooled ) {
						vItem.retoolNameList = row.get("retoolNameList").toString();
						vItem.retoolRoomDesc = row.get("retoolDesc").toString();
						vItem.retoolShortDesc = row.get("retoolShortDesc").toString();
						vItem.retoolExtraDesc = row.get("retoolExtraDesc").toString();
					}
					else {
						vItem.retoolNameList = "";
						vItem.retoolRoomDesc = "";
						vItem.retoolShortDesc = "";
						vItem.retoolExtraDesc = "";
					}
					sortedInsert(rewardSlots[slotId], vItem, getKey);
				}
			}
			else break;
			rsItems.skipRow();
		}

		quest.itemRewards = rewardSlots;

		/**** Load each requirement ****/
		while( rsReqs.hasNextRow ) {
			var row = rsReqs.peekRow;
			if( parseInt( row.get("quest_id") ) == quest.databaseID ) {
				quest.priorQuests.push( row.get("required_quest_id") );
			}
			else break;
			rsReqs.skipRow();
		}
		/**** Load each extra ****/
		while( rsExtras.hasNextRow ) {
			var row = rsExtras.peekRow;
			if( parseInt( row.get("quest_id") ) == quest.databaseID ) {
				quest.extraChecks.push( row.get("extra_check") );
			}
			else break;
			rsExtras.skipRow();
		}
		
		/**** Load each tag ****/
		while( rsTags.hasNextRow ) {
			var row = rsTags.peekRow;
			if( parseInt( row.get("quest_id") ) == quest.databaseID ) {
				quest.tags.push( row.get("tag") );
			}
			else break;
			rsTags.skipRow();
		}
		sortedInsert( vQuests, quest, function(q){return q.name;} );

		// Store this quest in a globally accessible location
		Quest.addQuest(quest);
	}
	global.vQuests = vQuests;
	// function fSort(a,b) {
		// var cmpArr = [a.name,b.name];
		// cmpArr.sort();
		// if ( cmpArr[0] == a.name )
			// return -1;
		// else
			// return 1;
	// }
	// global.vQuests.sort(fSort);
	mudLog(constants.BRF, 102, "Quests loaded: " + vQuests.length );
}
function saveAllQuests()
{
	mudLog(constants.BRF, 102, "Saving quests...");
	for(var i = 0;i < global.vQuests.length;++i)
	{
		saveQuest( global.vQuests[ i ] );
	}
	mudLog(constants.BRF, 102, "Quests saved.");
}
function deleteQuestFromDatabase( quest, bShowMsg )
{
	if( !quest ) return;
	if( quest.databaseID == undefined ) return;
	
	sqlQuery( "DELETE FROM " + QUEST_DATA_TABLE            + " WHERE id='"       + sqlEsc(quest.databaseID) + "';" );
	sqlQuery( "DELETE FROM " + QUEST_DIALOGUE_TABLE        + " WHERE quest_id='" + sqlEsc(quest.databaseID) + "';" );
	sqlQuery( "DELETE FROM " + QUEST_ITEMS_TABLE           + " WHERE quest_id='" + sqlEsc(quest.databaseID) + "';" );
	sqlQuery( "DELETE FROM " + QUEST_TASK_TABLE            + " WHERE quest_id='" + sqlEsc(quest.databaseID) + "';" );
	sqlQuery( "DELETE FROM " + QUEST_REQUIREMENT_TABLE     + " WHERE quest_id='" + sqlEsc(quest.databaseID) + "';" );
	sqlQuery( "DELETE FROM " + QUEST_EXTRAS_TABLE          + " WHERE quest_id='" + sqlEsc(quest.databaseID) + "';" );
	sqlQuery( "DELETE FROM " + QUEST_OWNER_VNUM_TABLE      + " WHERE quest_id='" + sqlEsc(quest.databaseID) + "';" );
	sqlQuery( "DELETE FROM " + QUEST_TAGS_TABLE            + " WHERE quest_id='" + sqlEsc(quest.databaseID) + "';" );
	
	quest.databaseID = undefined;
	if ( bShowMsg != false )
		mudLog(constants.BRF, 102, "Quest deleted: "+quest.name);
}
function saveQuest( quest )
{
	var sQuery;
	
	var editors = quest.editors ? sqlEsc( quest.editors.join(QUEST_EDITORS_SEPARATOR) ) : "";
	if( quest.databaseID == undefined ) {
		//Entry does NOT yet exist in the database.
		var sQuery = "INSERT INTO " + QUEST_DATA_TABLE + " (`questName`,`open`,`qpReward`,`fpReward`,`expReward`,`goldReward`,`numCanComplete`,`strSummary`,`skillArray`,`showGivenBy`,`unlockRank`,`editors`) VALUES("
			+ "'" + sqlEsc( quest.name ) + "',"
			+ "'" + quest.allowedRaces.join(";") + "',"
			+ "'" + quest.qp + "',"
			+ "'" + quest.fp + "',"
			+ "'" + quest.exp + "',"
			+ "'" + quest.gold + "',"
			+ "'" + quest.maxCompletions + "',"
			+ "'" + sqlEsc( quest.summary ) + "',"
			+ "'" + (quest.skills ? sqlEsc( quest.skills.join( QUEST_SKILL_ARRAY_SEPARATOR ) ) : "") + "',"
			+ "'" + quest.showGivenBy + "',"
			+ "'" + quest.unlockRank + "',"
			+ "'" + editors + "'"
			+ ");";
		sqlQuery( sQuery );
		quest.databaseID = sqlInsertID();
	} else {
		// If we are changing the name of a quest, we need to update the qvals to reflect the name change
		var nameQuery = "SELECT questName FROM " + QUEST_DATA_TABLE + " WHERE id = "+quest.databaseID+";";
		if ( nameQuery.hasNextRow ) {
			var oldName = nameQuery.peekRow.get('questName');
			if ( str_cmp(oldName,quest.name) ) {
				var qvalUpdate = "UPDATE quests SET quest_name='"+sqlEsc(quest.name)+"' WHERE quest_name='"+oldName+"';";
				sqlQuery(qvalUpdate);
			}
		}
		
		//Entry does exist. We'll do an update.
		var sQuery = "UPDATE " + QUEST_DATA_TABLE + " SET"
			+ " questName='" + sqlEsc( quest.name ) + "',"
			+ " qpReward='" + quest.qp + "',"
			+ " fpReward='" + quest.fp + "',"
			+ " expReward='" + quest.exp + "',"
			+ " goldReward='" + quest.gold + "',"
			+ " numCanComplete='" + quest.maxCompletions + "',"
			+ " strSummary='" + sqlEsc( quest.summary ) + "',"
			+ " skillArray='" + (quest.skills ? sqlEsc( quest.skills.join( QUEST_SKILL_ARRAY_SEPARATOR ) ) : "") + "',"
			+ " showGivenBy='" + Number(quest.showGivenBy) + "',"
			+ " unlockRank='" + quest.unlockRank + "',"
			+ " open='" + sqlEsc( quest.allowedRaces.join(";") ) + "',"
			+ " editors='" + editors + "'"
			+ " WHERE id='" + quest.databaseID + "';";
		sqlQuery( sQuery );
	}
	
	sqlQuery("DELETE FROM " + QUEST_ITEMS_TABLE + " WHERE quest_id='" + quest.databaseID + "';");
	
	/**** Item Requirements ****/
	for(var i = 0;i < quest.items.length;++i) {
		sQuery = "INSERT INTO " + QUEST_ITEMS_TABLE + " (`quest_id`,`position`,`item`,`itemCount`,`itemType`) VALUES("
			+ "'" + quest.databaseID + "',"
			+ "'" + (i+1) + "',"
			+ "'" + quest.items[i][ 0 ] + "',"
			+ "'" + quest.items[i][ 1 ] + "',"
			+ "'" + QUEST_ITEM_REQUIRED_TOKEN + "'"
			+ ");";
		sqlQuery( sQuery );
	}
	
	/**** Item Rewards ****/
	var itemRewards = [];
	for (var id in quest.itemRewards) {
		var slot = quest.itemRewards[id];
		itemRewards = itemRewards.concat(slot);
	}
	for(var i = 0; i < itemRewards.length; ++i) {
		sQuery = "INSERT INTO " + QUEST_ITEMS_TABLE + " (`quest_id`,`position`,`item`,`itemCount`,`loadPercent`,`itemType`,`slot_id`,`slot_tier`,`isRetooled`,`retoolNameList`,`retoolDesc`,`retoolShortDesc`,`retoolExtraDesc`) VALUES("
			+ "'" + quest.databaseID + "',"
			+ "'" + (i+1) + "',"
			+ "'" + itemRewards[i].vnum + "',"
			+ "'" + itemRewards[i].count + "',"
			+ "'" + itemRewards[i].loadPercent + "',"
			+ "'" + QUEST_ITEM_REWARD_TOKEN + "',"
			+ "'" + sqlEsc(itemRewards[i].id) + "',"
			+ "'" + itemRewards[i].tier + "',"
			+ "'" + Number(itemRewards[i].isRetooled) + "',"
			+ "'" + sqlEsc(itemRewards[i].retoolNameList) + "',"	//Namelist
			+ "'" + sqlEsc(itemRewards[i].retoolRoomDesc) + "',"	//Room desc
			+ "'" + sqlEsc(itemRewards[i].retoolShortDesc) + "',"	//Name
			+ "'" + sqlEsc(itemRewards[i].retoolExtraDesc) + "'"	//Extra desc
			+ ");";
		sqlQuery( sQuery );
	}
	
	/**** Dialogue ****/
	sqlQuery("DELETE FROM " + QUEST_DIALOGUE_TABLE + " WHERE quest_id='" + quest.databaseID + "';");
	for(var i = 0;i < quest.dialogue.length;++i) {
		if( getObjectClass(quest.dialogue[ i ]) != "string" ) {
			var sHandle = sqlEsc(quest.dialogue[ i ].handle);
			var iPulses = quest.dialogue[ i ].pulses;
		}
		else {
			var sHandle = sqlEsc( quest.dialogue[ i ] );
			var iPulses = -1;
		}
	
		sQuery = "INSERT INTO " + QUEST_DIALOGUE_TABLE + " (`quest_id`,`position`,`timeout`,`dialogue`) VALUES("
			+ "'" + quest.databaseID + "',"
			+ "'" + (i+1) + "',"
			+ "'" + iPulses + "',"
			+ "'" + sHandle + "'"
			+ ");";
		sqlQuery( sQuery );
	}
	/**** Prior Quests ****/
	sqlQuery("DELETE FROM " + QUEST_REQUIREMENT_TABLE + " WHERE quest_id='" + quest.databaseID + "';");
	for(var i = 0;i < quest.priorQuests.length;++i) {
		sQuery = "INSERT INTO " + QUEST_REQUIREMENT_TABLE + " (`quest_id`,`position`,`required_quest_id`) VALUES("
			+ "'" + quest.databaseID + "',"
			+ "'" + (i+1) + "',"
			+ "'" + sqlEsc(quest.priorQuests[ i ]) + "'"
			+ ");";
		sqlQuery( sQuery );
	}
	
	/**** Extra Checks ****/
	if( quest.extraChecks != undefined && quest.extraChecks.length > 0 ) {
		sqlQuery("DELETE FROM " + QUEST_EXTRAS_TABLE + " WHERE quest_id='" + quest.databaseID + "';");
		for(var i = 0;i < quest.extraChecks.length;++i) {
			sQuery = "INSERT INTO " + QUEST_EXTRAS_TABLE + " (`quest_id`,`extra_check`) VALUES("
				+ "'" + quest.databaseID + "',"
				+ "'" + sqlEsc(quest.extraChecks[ i ]) + "'"
				+ ");";
			sqlQuery( sQuery );
		}
	}
	
	/**** Quest Tasks ****/
	var taskIds = _.map(quest.tasks, "id");
	if (taskIds.length > 0)
		sqlQuery("DELETE FROM " + QUEST_TASK_TABLE + " WHERE quest_id='" + quest.databaseID + "' AND id NOT IN(" + taskIds.join(",") + ");");

	for(var i = 0, task; task = quest.tasks[i++];) {
		sQuery = prepareStatement(
			"UPDATE " + QUEST_TASK_TABLE + " SET position = {0}, taskName = '{1}', taskProgress = '{2}', taskRequirement = '{3}', unlockerIndex = {4} " +
			"WHERE quest_id = {5} && id = {6};",
			i + 1, task.name, task._dynamicProgress, task._completedExpr, task.unlockerTaskIndex, quest.databaseID, task.id
		);
		//sQuery = "INSERT INTO " + QUEST_TASK_TABLE + " (`quest_id`,`position`,`taskName`,`taskProgress`,`taskRequirement`,`unlockerIndex`) VALUES("
		//	+ "'" + quest.databaseID + "',"
		//	+ "'" + (i+1) + "',"
		//	+ "'" + sqlEsc(quest.tasks[ i ][ 0 ]) + "',"
		//	+ "'" + sqlEsc(quest.tasks[ i ][ 1 ]) + "',"
		//	+ "'" + sqlEsc(quest.tasks[ i ][ 2 ]) + "',"
		//	+ "'" + sqlEsc(quest.tasks[ i ][ 3 ]) + "'"
		//	+ ");";
		sqlQuery( sQuery );
	}
	/**** Quest Tags ****/
	if( quest.tags != undefined ) {
		sqlQuery("DELETE FROM " + QUEST_TAGS_TABLE + " WHERE quest_id='" + quest.databaseID + "';");
		for(var i = 0;i < quest.tags.length;++i) {
			sQuery = "INSERT INTO " + QUEST_TAGS_TABLE + " (`quest_id`,`position`,`tag`) VALUES("
				+ "'" + quest.databaseID + "',"
				+ "'" + (i+1) + "',"
				+ "'" + sqlEsc(quest.tags[ i ]) + "'"
				+ ");";
			sqlQuery( sQuery );
		}
	}
	/**** Quest Owner Vnums ****/
	if( quest.ownerVnums != undefined ) {
		sqlQuery("DELETE FROM " + QUEST_OWNER_VNUM_TABLE + " WHERE quest_id='" + quest.databaseID + "';");
		if ( getObjectClass(quest.ownerVnums) != "Array" ) {
			quest.ownerVnums = [quest.ownerVnums];
		}
		for(var i = 0;i < quest.ownerVnums.length;++i) {
			sQuery = "INSERT INTO " + QUEST_OWNER_VNUM_TABLE + " (`quest_id`,`owner_vnum`) VALUES("
				+ "'" + quest.databaseID + "',"
				+ "'" + sqlEsc(quest.ownerVnums[ i ]) + "'"
				+ ");";
			sqlQuery( sQuery );
		}
	}
	mudLog(constants.BRF, 102, "Quest saved: "+quest.name);
}
function loadQuest( sQuestName )
{
}



