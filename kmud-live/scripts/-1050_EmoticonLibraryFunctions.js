var emoticonSpawnRooms =
[
	357,
	410,
	21418,
	1911,
	19310,
	20645,
	18144,
	2013,
	10103
];

var emoticonQuestHourOfLastTeleportion = 0;

function spawnEmoticons()
{
	var sql = " SELECT mob_id"
			+ " FROM emoticonQuestMob"
			+ " WHERE killed_datetime IS NULL"
			+ " ORDER BY is_leader DESC";
	
	var resultSet = sqlQuery(sql);
	var leaderMob = null;
	//var spawnRoomVnum = 19310;
	var spawnRoomVnum = emoticonSpawnRooms[ random(0, emoticonSpawnRooms.length - 1) ];
	var spawnRoom = getRoom(spawnRoomVnum);
	var isFirst = true;
	
	while(resultSet.hasNextRow)
	{
		var row = resultSet.getRow;
		var mobVnum = row.getInteger("mob_id");
		var mob = spawnRoom.loadMob(mobVnum);
		
		if(isFirst)
			leaderMob = mob;
		else
			mob.comm("follow " + leaderMob.name);
			
		spawnRoom.loadMob(57);
		mob.comm("ride wagon");
		
		isFirst = false;
	}
	
	leaderMob.comm("group all");
}

function markEmoticonQuestMobAsKilled(emoticonMob, killer)
{
	var sql = " UPDATE emoticonQuestMob SET"
			+ "   killed_datetime = " + sqlEncodeQuoteDate(new Date()) + ","
			+ "   killed_room_id = " + emoticonMob.room.vnum + ","
			+ "   killer_user_id = " + killer.id
			+ " WHERE mob_id = " + emoticonMob.vnum;
			
	sqlQuery(sql);
}

function emoticonQuestReAssessLeadership(dyingMob)
{
	var followers = dyingMob.followers;
	if(followers && followers.length > 0)
	{
		var newLeader = followers[ 0 ];
		
		newLeader.comm("follow self");
		
		for(var index = 1;index < followers.length;++index)
		{
			followers[ index ].comm("follow " + newLeader.name);
		}
	}
}

function emoticonQuestEmoFling(emoMob, target)
{
	var depth = random(7,10);
	var destinationRoom = target.room;
	var transportRooms = [destinationRoom];//These are the rooms the player has been flung through.
	
	act("You point your magic wand at $N!", false, emoMob, null, target, constants.TO_CHAR);
	act("$n points $s magic wand at $N!", false, emoMob, null, target, constants.TO_NOTVICT);
	act("$n points $s magic wand at you!", false, emoMob, null, target, constants.TO_VICT);
	
	act("$n is flung from the room!", false, target, null, null, constants.TO_ROOM);
	target.send("You are flung from the room!");
	
	while(depth-- > 0)
	{
		var tempNeighbors = destinationRoom.neighbors;
		var neighbors = [];
		for(var neighborIndex = 0;neighborIndex < tempNeighbors.length;++neighborIndex)
		{
			if(tempNeighbors[neighborIndex] && !arrContains(transportRooms, tempNeighbors[neighborIndex]))
				neighbors.push(tempNeighbors[neighborIndex]);
		}
		
		if(neighbors.length == 0)
			break;
		
		destinationRoom = neighbors[ random(0, neighbors.length-1) ];
		
		target.moveToRoom(destinationRoom);
		act("$n zooms through the air!", false, target, null, null, constants.TO_ROOM);
		
		transportRooms.push(destinationRoom);
	}
	
	act("$n very suddenly comes crashing down from above!", false, target, null, null, constants.TO_ROOM);
	target.send("You very suddenly crash into something hard! Ouch!");
	target.moveToRoom(destinationRoom);
	target.comm("look");
}

function emoticonQuestGetGroupMembers(mob)
{
	var leader = mob.leader ? mob.leader : mob;
	var groupMembers = leader.followers;
	groupMembers.push(leader);
	
	return groupMembers;
}

function emoticonQuestGetOpponents(mob, groupMembers)
{
	var opponents = [];
	var peopleInRoom = mob.room.people;
	for(var index = 0;index < peopleInRoom.length;++index)
	{
		var personInRoom = peopleInRoom[ index ];
		
		if(personInRoom.fighting && arrContains(groupMembers, personInRoom.fighting))
			opponents.push(personInRoom);
	}
	
	return opponents;
}

function emoticonQuestEmoRandomCombat(mob)
{
	var roll = random(1,100);
	var groupMembers = emoticonQuestGetGroupMembers(mob);
	var opponents = emoticonQuestGetOpponents(mob, groupMembers);

	if(roll >= 1 && roll <= 15)
	{
		if(opponents.length > 2)
		{
			mob.say("Ava Cadavra!");
			//Bring the number of opponents down to about 2.
			for(var index = 2;index < opponents.length;++index)
			{
				if(random(1,4) != 1) //75% Chance
					emoticonQuestEmoFling(mob, opponents[ index ]);
			}
		}
	}
	else if(roll >= 16 && roll <= 25)
	{
		mob.say("Hocus Pocus!");
		
		for(var index = 0;index < groupMembers.length;++index)
		{
			var groupMember = groupMembers[ index ];
			if(groupMember != mob)
			{
				act("A shimmering blue aura surrounds $N, rejuvinating $M!", false, mob, null, groupMember, constants.TO_ROOM);
				act("A shimmering blue aura surrounds you. You feel rejuvinated!", false, mob, null, groupMember, constants.TO_CHAR);
				
				groupMember.hps += random(25, 75);
			}
		}
	}
}

function emoticonQuestBlandRandomCombat(mob)
{
	var roll = random(1,100);
	var groupMembers = emoticonQuestGetGroupMembers(mob);
	var opponents = emoticonQuestGetOpponents(mob, groupMembers);
	
	if(roll >= 1 && roll <= 25)
	{
		for(var index = 0;index < opponents.length;++index)
		{
			var opponent = opponents[ index ];
			if(opponent.fighting != mob && random(1,100) <= 60) //60% chance
			{
				mob.say("Come at me, bro!");
				act("$n rams $s gut into $N, drawing $S attention!", false, mob, null, opponent, constants.TO_NOTVICT);
				act("$n rams $s gut into you, drawing your attention!", false, mob, null, opponent, constants.TO_VICT);
				act("You ram your gut into $N, drawing $S attention!", false, mob, null, opponent, constants.TO_CHAR);
				
				opponent.stopFighting();
				opponent.setFighting(mob);
			}
		}
	}
}

function emoticonQuestHappyMagicSnakes( vArgs )
{
	var mob = vArgs[ 0 ];
	var groupMembers = emoticonQuestGetGroupMembers(mob);
	var opponents = emoticonQuestGetOpponents(mob, groupMembers);
	
	act("$n gesticulates, and a slew of snakes appear!", false, mob, null, null, constants.TO_ROOM);
	act("You gesticulate, and a slew of snakes appear!", false, mob, null, null, constants.TO_CHAR);
	
	for(var snakeCount = 0;snakeCount < opponents.length;++snakeCount)
	{
		//Attack a random opponent.
		var snake = mob.room.loadMob(58);
		snake.setFighting(opponents[ random(0, opponents.length-1) ]);
	}
}

function emoticonQuestHappyRandomCombat(mob)
{
	var roll = random(1,100);
	var groupMembers = emoticonQuestGetGroupMembers(mob);
	var opponents = emoticonQuestGetOpponents(mob, groupMembers);
	
	if(roll >= 1 && roll <= 12)
	{
		mob.say("MAGIC SNAKES!!!");
		setTimeout(3, emoticonQuestHappyMagicSnakes, [mob]);
	}
}

function emoticonQuestRandomTeleportation(mob)
{
	act("$n throws something at the ground, and a plume of smoke appears!", false, mob, null, null, constants.TO_ROOM);
	act("You throw something at the ground. Smoke fills the room!", false, mob, null, null, constants.TO_CHAR);
	
	var groupMembers = emoticonQuestGetGroupMembers(mob);
	var destinationRoomVnum;
	var currentRoomVnum = mob.room.vnum;
	
	do
	{
		destinationRoomVnum = emoticonSpawnRooms[ random(0, emoticonSpawnRooms.length-1) ];
	}while(destinationRoomVnum == currentRoomVnum);
	
	var destinationRoom = getRoom(destinationRoomVnum);
	
	for(var index = 0;index < groupMembers.length;++index)
	{
		var groupMember = groupMembers[ index ];
		
		groupMember.moveToRoom(destinationRoom);
		
		if(groupMember.mount)
			groupMember.mount.moveToRoom(destinationRoom);
	}
	
	emoticonQuestHourOfLastTeleportion = getHour();
}

function emoticonQuestKeyAppears(vArgs)
{
	var roomVnum = vArgs[0];
	var room = getRoom(roomVnum);
	var key = room.loadObj(58);
	var people = room.people;
	
	for(var index = 0;index < people.length;++index)
	{
		getCharCols(people[index]);
		
		people[index].send(bld + cyn + "A key drops to the ground." + nrm);
	}
}

function emoticonQuestCheckIfLastMember(mob)
{
	var groupMembers = emoticonQuestGetGroupMembers(mob);
	
	if(groupMembers.length == 1)
	{
		setTimeout(7, emoticonQuestKeyAppears, [mob.room.vnum]);
	}
}

function emoticonQuestStorePlayerKill(mob, victim, roomVnum)
{
	if(victim.vnum != -1)
		return;

	var sql = " INSERT INTO emoticonQuestMobPlayerKill("
			+ "   `mob_id`,"
			+ "   `user_id`,"
			+ "   `killed_datetime`,"
			+ "   `room_id`"
			+ " ) VALUES ("
			+ mob.vnum + ","
			+ victim.id + ","
			+ sqlEncodeQuoteDate(new Date()) + ","
			+ roomVnum + ")";
			
	sqlQuery(sql);
}

function emoticonQuestReportStatus( vArgs )
{
	var actor = vArgs[ 0 ];
	var buffer = "";
	getCharCols(actor);
	
	buffer += bld + mag + "Three emoticons traveled great lengths on their magical wagons. They crossed seas and forests, villages and deserts.\r\n"
	buffer +=             "At last they reached their destination: KINSLAYER MUD! And their intentions are not friendly. They have vowed\r\n"
	buffer +=             "To pillage and plunder, to murder & loot.\r\n\r\n"
	
	buffer +=             "It is up to you, fellow adventurer, to see to it that they are stopped!" + nrm + "\r\n\r\n";
	
	var sql = " SELECT * FROM emoticonQuestMob";
	var resultSet = sqlQuery(sql);
	
	buffer += bld + cyn + "THE VILLAINS\r\n" + nrm;
	buffer += "-------------------------\r\n";
	while(resultSet.hasNextRow)
	{
		var row = resultSet.getRow;
		var mobProto = getMobProto(row.getInteger("mob_id"));
		var killedDatetime = row.getTimestamp("killed_datetime");
		var killerUserId = row.getInteger("killer_user_id");
		var killedRoomId = row.getInteger("killed_room_id");
		
		buffer += strPadding(mobProto.name, " ", 9, "right") + " - ";
		
		if(killedDatetime == null)
			buffer += "Still Alive! Kill the bastard!";
		else
		{
			var userName = getUserNameByUserId(killerUserId);
			if(userName == null || userName == '')
				userName = "UNKNOWN";
			
			buffer += "Killed By " + userName + " @" + killedDatetime;
		}
		
		buffer += "\r\n";
	}
	
	buffer += "\r\n";
	buffer += bld + cyn + "THE VICTIMS" + nrm + "\r\n";
	buffer += "-------------------------\r\n";
	
	sql = " SELECT COUNT(*) AS cnt,user_id FROM emoticonQuestMobPlayerKill GROUP BY user_id ORDER BY COUNT(*) DESC";
	resultSet = sqlQuery(sql);
	
	while(resultSet.hasNextRow)
	{
		var row = resultSet.getRow;
		var userId = row.getInteger("user_id");
		var deathCount = row.getInteger("cnt");
		
		var userName = getUserNameByUserId(userId);
		
		if(userName == null || userName == '')
			userName = "UNKNOWN";
			
		buffer += userName + " has been killed " + deathCount + " time" + (deathCount == 1 ? "" : "s") + ".\r\n";
	}
	
	buffer += "\r\n";
	buffer += bld + cyn + "THE REWARD" + nrm + "\r\n";
	buffer += "-------------------------\r\n";
	buffer += "a face-shaped chest - Near someplace sinister!";
	
	actor.send(buffer);
}

