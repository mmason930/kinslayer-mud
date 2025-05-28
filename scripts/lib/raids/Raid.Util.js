if (typeof (Raid) === "undefined")
    Raid = {};


/**
 * @namespace Raid.Party
 */
Raid.Party = (function() {
    return {
        create: function(headMobVnum, followerMobVnums, allowedStartingLocations, onHeadMobLoad) {
            return {
                headMobVnum: headMobVnum,
                followerMobVnums: followerMobVnums,
                allowedStartingLocations: allowedStartingLocations,
                onHeadMobLoad: onHeadMobLoad
            }
        }
    }
})();

/**
 * @namespace Raid.Location
 */
Raid.Location = (function() {
    return {
        create: function(startRoomVnum, endRoomVnum, numberOfReinforcementsWhileFighting) {
            return {
                startRoomVnum: startRoomVnum,
                endRoomVnum: endRoomVnum,
                numberOfReinforcementsWhileFighting: numberOfReinforcementsWhileFighting
            }
        }
    }
})();

/**
 * @namespace Raid.Util
 */
Raid.Util = (function() {

    let self = {};
    self.startLocationMap = {
        gapBasin: Raid.Location.create(20622, 8068, 3),
        stagnantPond: Raid.Location.create(1024, 8170, 4),
        falDaraToGapRoad: Raid.Location.create(19301, 8068, 3),
        blightedScout: Raid.Location.create(1090, 8170, 3),
        northOfKeep: Raid.Location.create(5658, 8068, 3),
        nearBaerlonEastGate: Raid.Location.create(10247, 4571, 3),
        northwestOfLugard: Raid.Location.create(412, 8436, 3),
        bowlPatrolInGap: Raid.Location.create(20675, 1779, 3),
        pathCavePatrolNorthwestTarValon: Raid.Location.create(1239, 21390, 3),
        falMoranPatrol: Raid.Location.create(6463, 10392, 3),
        eastOfCaemlynNearAringill: Raid.Location.create(21473, 117, 3)
    };

    self.parties = {
        mirage: Raid.Party.create(
            20890,
            [20965,20966,20967,20968,20969],
            [
                self.startLocationMap.nearBaerlonEastGate,
                self.startLocationMap.northwestOfLugard,
                self.startLocationMap.bowlPatrolInGap,
                self.startLocationMap.pathCavePatrolNorthwestTarValon,
                self.startLocationMap.falMoranPatrol
            ]
        ),

        syyggar: Raid.Party.create(
            5609,
            [20960,20961,20962,20963,20964],
            [
                self.startLocationMap.nearBaerlonEastGate,
                self.startLocationMap.northwestOfLugard,
                self.startLocationMap.bowlPatrolInGap,
                self.startLocationMap.pathCavePatrolNorthwestTarValon,
                self.startLocationMap.falMoranPatrol
            ]
        ),

        kari: Raid.Party.create(
            20892,
            [20955,20956,20957,20958,20959],
            [
                self.startLocationMap.gapBasin,
                self.startLocationMap.stagnantPond,
                self.startLocationMap.falDaraToGapRoad,
                self.startLocationMap.blightedScout,
                self.startLocationMap.northOfKeep
            ]
        ),
        uno: Raid.Party.create(
            1711,
            [20950,20951,20952,20953,20954],
            [
                self.startLocationMap.gapBasin,
                self.startLocationMap.stagnantPond,
                self.startLocationMap.falDaraToGapRoad,
                self.startLocationMap.blightedScout,
                self.startLocationMap.northOfKeep
            ]
        ),
        turak: Raid.Party.create(
            18800,
            [20970,20971,20972,20973,20974],
            [
                self.startLocationMap.nearBaerlonEastGate,
                self.startLocationMap.northwestOfLugard,
                self.startLocationMap.bowlPatrolInGap,
                self.startLocationMap.pathCavePatrolNorthwestTarValon,
                self.startLocationMap.falMoranPatrol,
                self.startLocationMap.stagnantPond,
                self.startLocationMap.falDaraToGapRoad,
                self.startLocationMap.blightedScout,
                self.startLocationMap.northOfKeep,
                self.startLocationMap.eastOfCaemlynNearAringill,
            ]
        ),
        cuarn: Raid.Party.create(
            20637,
            [20975,20976,20977,20978,20979],
            [
                self.startLocationMap.nearBaerlonEastGate,
                self.startLocationMap.northwestOfLugard,
                self.startLocationMap.bowlPatrolInGap,
                self.startLocationMap.pathCavePatrolNorthwestTarValon,
                self.startLocationMap.falMoranPatrol,
                self.startLocationMap.stagnantPond,
                self.startLocationMap.falDaraToGapRoad,
                self.startLocationMap.blightedScout,
                self.startLocationMap.northOfKeep,
                self.startLocationMap.eastOfCaemlynNearAringill,
            ],
            function(headMob) {
                headMob.loadObj(random(1700, 1707));
                headMob.comm("wear bow");
            }
        )
    };

    self.desiredFollowersInRoom = 5;
    self.probabilityOfMoving = 99;
    self.probabilityOfMovingWrongDirection = 8;
    self.secondsBetweenShouts = 180;

    self.getValidRoomsInZone = function(zoneVnum, predicate) {
        return getAllRoomsInZone(zoneVnum, function(room) {
            return (predicate == null || predicate(room)) && !room.roomFlagged(constants.ROOM_INACCESSIBLE) &&
                !room.roomFlagged(constants.ROOM_DEATH) &&
                !room.roomFlagged(constants.ROOM_NOMOB) &&
                !room.roomFlagged(constants.ROOM_PRIVATE) &&
                !room.roomFlagged(constants.ROOM_GODROOM) &&
                room.sector !== constants.SECT_WATER_SWIM &&
                room.sector !== constants.SECT_WATER_NOSWIM &&
                room.sector !== constants.SECT_UNDERWATER &&
                room.sector !== constants.SECT_FLYING
        });
    };

    self.getFollowersInRoom = function(actor) {
        return actor.room.people.filter(function (personInRoom) {
            return personInRoom.leader === actor;
        });
    };

    self.debugSend = function(message) {
        getConnectedPlayers().filter(function(p) {
            return p.name === "Galnor";
        }).forEach(function(p) {
            p.send(message);
        });
    };

    self.summonFollowersDueToFighting = function(headMob) {
        self.debugSend("Processing follower summoning due to fighting");
        if(!headMob.fighting) {
            self.debugSend("Skipping follower summon due to not fighting.");
            return;
        }

        const numberOfFollowersInRoom = self.getFollowersInRoom(headMob).length;

        if(numberOfFollowersInRoom !== 0) {
            self.debugSend("Skipping follower summon due to follower count = " + numberOfFollowersInRoom + ".");
            return;
        }

        self.debugSend("All checks passed. Proceeding with follower summon.");
        self.summonFollowersToHeadMobRoom(
            headMob,
            headMob.raidLocation.numberOfReinforcementsWhileFighting,
            function(headMob) {
                headMob.comm("yell " + capFirstLetter(headMob.fighting.name) + " is attacking me at " + headMob.room.name + "! Help!");
            },
            function(headMob) {
                // If no followers, don't do or say anything.
            }
        );
    };

    self.summonFollowersToHeadMobRoom = function(
        headMob, // JSCharacter
        targetNumberOfFollowersInRoom,
        onFollowersAvailableToSummon, // Function(JSCharacter headMob)
        onNoFollowersAvailableToSummon // Function(JSCharacter headMob)
    ) {
        const followersInRoom = self.getFollowersInRoom(headMob);
        const numberOfFollowersToAdd = targetNumberOfFollowersInRoom - followersInRoom.length;
        const followersOutsideOfRoom = headMob.followers.filter(function(follower) {
            return follower.room.vnum !== headMob.room.vnum;
        });

        if(numberOfFollowersToAdd > 0) {

            if(onFollowersAvailableToSummon) {
                onFollowersAvailableToSummon(headMob);
            }
            //headMob.say("We need more numbers to make our final approach!");
            //act("$n calls for assistance!", false, headMob, null, null, constants.TO_ROOM);

            if(followersOutsideOfRoom.length === 0) {

                if(onNoFollowersAvailableToSummon) {
                    onNoFollowersAvailableToSummon(headMob);
                }
                //headMob.say("It appears we have no further reinforcements. Onward we go!");
                return true;
            }
        }

        for(let additionIndex = 0;additionIndex < numberOfFollowersToAdd;++additionIndex) {
            if(followersOutsideOfRoom.length === 0) {
                break;
            }

            const followerIndex = random(0, followersOutsideOfRoom.length - 1);
            const followerToAdd = followersOutsideOfRoom[followerIndex];

            act("$n leaves in a hurry to assist $N!", false, followerToAdd, null, headMob, constants.TO_ROOM);
            act("You leave in a hurry to assist $N!", false, followerToAdd, null, headMob, constants.TO_CHAR);

            followerToAdd.moveToRoom(headMob.room);
            followersOutsideOfRoom.splice(followerIndex, 1);

            act("$n arrives in a hurry to assist you!", false, followerToAdd, null, headMob, constants.TO_VICT);
            act("$n arrives in a hurry to assist $N!", false, followerToAdd, null, headMob, constants.TO_NOTVICT);
        }
    }

    self.headMobMove = function(headMob) {
        if(headMob.position !== constants.POS_STANDING) {
            return;
        }

        const headMobRoom = headMob.room;

        if(self.canShout(headMob)) {
            self.performPeriodicShout(headMob);
        }

        let validRoomsInZone = self.getValidRoomsInZone(headMobRoom.zoneVnum,function(room) {
            return room.vnum !== headMobRoom.vnum;
        });

        if(validRoomsInZone.length > 0) {

            // If there are too many followers in the room, send some away.
            let followersInRoom = self.getFollowersInRoom(headMob);
            const numberOfFollowersToRemove = followersInRoom.length - self.desiredFollowersInRoom;

            for (let removalIndex = 0; removalIndex < numberOfFollowersToRemove; ++removalIndex) {
                const followerToRemove = followersInRoom[random(0, followersInRoom.length - 1 - removalIndex)];
                act("You send $N away to scout the area.", false, headMob, null, followerToRemove, constants.TO_CHAR);
                act("$n sends $N away to scout the area.", false, headMob, null, followerToRemove, constants.TO_NOTVICT);
                act("$n sends you away to scout the area.", false, headMob, null, followerToRemove, constants.TO_VICT);

                const roomToMoveFollower = validRoomsInZone[random(0, validRoomsInZone.length - 1)];
                followerToRemove.moveToRoom(roomToMoveFollower);

                act("$n has arrived, scouting the area.", false, followerToRemove, null, null, constants.TO_ROOM);
            }
        }

        let followersInRoomFighting = Raid.Util.getFollowersInRoom(headMob)
            .filter(function(follower) {
                return follower.fighting;
            });

        // Don't move if any followers in the current room are fighting.
        if(followersInRoomFighting.length > 0) {
            self.debugSend("Not moving because " + followersInRoomFighting.length + " follower(s) fighting.");
            return;
        }

        const advanceRoll = random(1, 100);
        if(advanceRoll >= self.probabilityOfMoving) {
            self.debugSend("Not moving because rolled " + advanceRoll + " >= " + self.probabilityOfMoving);
            return;
        }

        let endRoom = getRoom(headMob.raidLocation.endRoomVnum);

        self.debugSend("End Room Vnum: " + endRoom.vnum + ", Name: " + endRoom.name);

        if(headMobRoom.vnum === endRoom.vnum) {
            let numberOfAggroTargets = headMobRoom.people.filter(function(personInRoom) {
                return headMob.canAggro(personInRoom);
            });

            if(numberOfAggroTargets < 0) {
                headMob.say("Victory is mine!");
            }
        } else {
            let firstStepToEndRoom = headMobRoom.firstStep(endRoom);
            let distanceToEndRoom = headMobRoom.distanceTo(endRoom);
            let directionMoved = firstStepToEndRoom;

            // Small chance of going the wrong way.
            if(random(1, 100) <= self.probabilityOfMovingWrongDirection) {
                directionMoved = headMobRoom.getRandomExitDirection(function(neighborRoom, neighborExit) {
                    return neighborExit !== firstStepToEndRoom;
                });

                self.debugSend("Changing direction from " + (firstStepToEndRoom == null ? "NULL" : dirToText(firstStepToEndRoom)) + " to " + (directionMoved == null ? "NULL" : dirToText(directionMoved)));
            }

            if(directionMoved === null) {
                return;
            }

            let directionMovedText = dirToText(directionMoved);

            if(headMobRoom.doorIsClosed(directionMoved)) {
                act("$n opens the way " + directionMovedText + ".", false, headMob, null, null, constants.TO_ROOM);
                headMobRoom.openDoor(directionMoved);
            }

            self.debugSend("Moving: " + dirToText(directionMoved));
            self.debugSend("Distance To End Room: " + distanceToEndRoom);

            if(distanceToEndRoom === 1) {
                self.summonFollowersToHeadMobRoom(headMob, self.desiredFollowersInRoom);
            }

            // Move to next room
            headMob.comm(directionMovedText);
        }

        // TODO: Add battering ram functionality for locked doors
        // TODO: If battering ram mob is not in the same room as the leader, have them head home
        // TODO: If followers have no leader, have them wander off and self purge
        // TODO: If too many followers in the same room(> 3), send off to another room
        // TODO: Autorescue
        // TODO: Look into weave points?
    };

    self.loadDisbursedFollowers = function(raidParty, headMob) {
        // Spread followers out throughout the zone.
        const disbursedFollowerTargetCount = 20;
        let disbursedFollowerLoaded = 0;
        let validRoomsInZone = self.getValidRoomsInZone(headMob.room.zoneVnum);

        while(disbursedFollowerLoaded < disbursedFollowerTargetCount) {

            // Select random mob.
            let followerRoll = random(1, 100);
            let followerIndex = null;

            if(followerRoll < 51) {
                followerIndex = 0;
            } else if(followerRoll < 81) {
                followerIndex = 1;
            } else if(followerRoll < 93) {
                followerIndex = 2;
            } else if(followerRoll < 99) {
                followerIndex = 3;
            } else {
                followerIndex = 4;
            }

            let roomToSpawnFollower = validRoomsInZone[random(0, validRoomsInZone.length - 1)];
            self.debugSend("Room To Spawn Follower: " + roomToSpawnFollower);
            let follower = headMob.room.loadMob(raidParty.followerMobVnums[followerIndex]);
            follower.comm("follow headraidmob");
            follower.moveToRoom(roomToSpawnFollower);
            roomToSpawnFollower.echo("A member of " + headMob.name + "'s growing army has arrived, scouting the area.");

            self.debugSend("Spawned " + follower.name + " in room " + follower.room.vnum);

            ++disbursedFollowerLoaded;
        }
    };

    self.performPeriodicShout = function(headMob) {

        llmResponse({
            model: "gpt-4.1-mini",
            prompt: "You're an NPC in a game. Your name is: " + headMob.name + ". You are the head NPC leading a small"
                + " army in a raid. Your raid is in progress and you are advancing on your target. You are now in a room"
                + " called " + headMob.room.name + ". Shout something menacing to everyone nearby. Keep it short, 1-2"
                + " sentences maximum. Provide only the dialog, not even in quotes.",
            onSuccess: function(result) {
                headMob.comm("shout " + result.response);
                headMob.shouts.push(result.response);
                headMob.lastShoutTime = new Date();
            }});
    };

    self.loadSameRoomFollowers = function(raidParty, headMob) {
        // Load the followers in the same room as the head raid mob.
        [
            headMob.room.loadMob(raidParty.followerMobVnums[2]),
            headMob.room.loadMob(raidParty.followerMobVnums[2]),
            headMob.room.loadMob(raidParty.followerMobVnums[3]),
            headMob.room.loadMob(raidParty.followerMobVnums[3]),
            headMob.room.loadMob(raidParty.followerMobVnums[4])
        ].forEach(function(follower) {
            follower.comm("follow headraidmob");
        });

        headMob.comm("group all");
    };

    self.raidHeadMobAnnounceRaid = function(headMob) {
        const raidLocation = headMob.raidLocation;
        const endRoom = getRoom(raidLocation.endRoomVnum);
        const startOfShoutMessage = "My army gathers at " + headMob.room.name + "! We march towards " + endRoom.name + " near " + endRoom.zoneName + ".";
        llmResponse({
            model: "gpt-4.1-mini",
            prompt: "You're an NPC in a game. Your name is: " + headMob.name + ". You are the head NPC leading a small"
            + " army in a raid. Your raid has just begin. You just shouted a menacing message to announce your raid,"
            + " which said: " + startOfShoutMessage + ". Generate me the next 1-2 sentences to add to this dialog."
            + " Provide the dialog only, nothing else, and not in quotes. The announcement should be menacing, and"
            + " also meant for those you're marching against. Make it 1-2 sentences max.",
            onSuccess: function(result) {
                headMob.comm("shout " + startOfShoutMessage + " " + result.response);
                headMob.shouts.push(startOfShoutMessage + " " + result.response);
                headMob.lastShoutTime = new Date();
            }});
    };

    self.canShout = function(headMob) {
        let secondsSinceLastShout = self.secondsBetweenShouts;

        if(headMob.lastShoutTime !== null) {
            secondsSinceLastShout = parseInt(new Date().getTime() / 1000) - parseInt(headMob.lastShoutTime.getTime() / 1000);
        }

        return secondsSinceLastShout >= self.secondsBetweenShouts;
    };

    self.beginRaidPartyAtLocation = function(raidParty, raidLocation) {
        let startRoomVnum = raidLocation.startRoomVnum;
        let startLocationRoom = getRoom(startRoomVnum);

        let headMob = startLocationRoom.loadMob(raidParty.headMobVnum);
        headMob.raidParty = raidParty;
        headMob.raidLocation = raidLocation;

        if(raidParty.onHeadMobLoad) {
            raidParty.onHeadMobLoad(headMob);
        }

        self.debugSend(startLocationRoom);

        self.loadDisbursedFollowers(raidParty, headMob);
        self.loadSameRoomFollowers(raidParty, headMob);

        gecho(function(player) {
            getCharCols(player, constants.CL_NORMAL);
            return mag + "*** A NEW RAID HAS BEGUN! ***" + nrm;
        });

        headMob.attach(20942);
        headMob.attach(20944);
        headMob.attach(20945);
        headMob.shouts = [];

        self.raidHeadMobAnnounceRaid(headMob);
    };

    self.beginRaid = function () {

        let raidPartyArray = _.values(self.parties);
        let rnd = random(0, raidPartyArray.length - 1);
        let raidParty = raidPartyArray[rnd];
        let raidLocation = raidParty.allowedStartingLocations[random(0, raidParty.allowedStartingLocations.length - 1)];

        self.beginRaidPartyAtLocation(raidParty, raidLocation);
    };

    self.onHeadMobDeath = function(headMob) {
        const followers = headMob.followers;
        setTimeout(60 * constants.PULSES_PER_SEC,function() {
            followers.forEach(function(follower) {
                if(follower.isValid) {
                    act("$n flees into the distance, $s leader having perished.", false, follower, null, null, constants.TO_ROOM);
                    act("You flee into the distance, your leader having perished.", false, follower, null, null, constants.TO_CHAR);
                    follower.extract(true);
                }
            });
        });
    }

    return self;
})();

let script20942 = function(self, actor, here, args, extra) {
    Raid.Util.headMobMove(self);
}

let script20944 = function(self, actor, here, args, extra) {
    Raid.Util.summonFollowersDueToFighting(self);
}

let script20945 = function(self, actor, here, args, extra) {
    Raid.Util.onHeadMobDeath(self);
}