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
        create: function(startRoomVnum, endRoomVnum) {
            return {
                startRoomVnum: startRoomVnum,
                endRoomVnum: endRoomVnum
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
        gapBasin: Raid.Location.create(20622, 8068),
        stagnantPond: Raid.Location.create(1024, 8170),
        falDaraToGapRoad: Raid.Location.create(19301, 8068),
        blightedScout: Raid.Location.create(1090, 8170),
        northOfKeep: Raid.Location.create(5658, 8068),
        nearBaerlonEastGate: Raid.Location.create(10247, 4571),
        northwestOfLugard: Raid.Location.create(412, 8436),
        bowlPatrolInGap: Raid.Location.create(20675, 1779),
        pathCavePatrolNorthwestTarValon: Raid.Location.create(1239, 21390),
        falMoranPatrol: Raid.Location.create(6463, 10392),
        eastOfCaemlynNearAringill: Raid.Location.create(21473, 117)
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
            [20955,20956,20962,20963,20964],
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

    self.numberOfReinforcementsWhileFighting = 3;
    self.desiredFollowersInRoom = 5;
    self.probabilityOfMoving = 30;

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
            self.numberOfReinforcementsWhileFighting,
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

        const advanceRoll = random(1, 100);
        if(advanceRoll >= self.probabilityOfMoving) {
            return;
        }

        let endRoom = getRoom(headMob.raidLocation.endRoomVnum);

        self.debugSend("End Room Vnum: " + endRoom.vnum);
        self.debugSend("End Room Name: " + endRoom.name);

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
            let firstStepToEndRoomText = dirToText(firstStepToEndRoom);

            if(headMobRoom.doorIsClosed(firstStepToEndRoom)) {
                act("$n opens the way " + firstStepToEndRoomText + ".", false, headMob, null, null, constants.TO_ROOM);
                headMobRoom.openDoor(firstStepToEndRoom);
            }

            self.debugSend("First Step To End Room: " + firstStepToEndRoom);
            self.debugSend("Distance To End Room: " + distanceToEndRoom);

            if(distanceToEndRoom === 1) {
                self.summonFollowersToHeadMobRoom(headMob, self.desiredFollowersInRoom);
            }

            // Move to next room
            headMob.comm(firstStepToEndRoomText);
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

            // TODO: Remove test code.
            self.debugSend("Spawned " + follower.name + " in room " + follower.room.vnum);

            ++disbursedFollowerLoaded;
        }
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

    self.beginRaid = function () {

        let raidPartyArray = _.values(self.parties);
        let rnd = random(0, raidPartyArray.length - 1);
        let raidParty = raidPartyArray[rnd];
        let raidLocation = raidParty.allowedStartingLocations[random(0, raidParty.allowedStartingLocations.length - 1)];
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

        /**
         * mgecho %(color_magenta%)*** RAID: %(raider.name%) retires with a flawless reputation, living on only in legend. ***%(color_normal%)
         * mteleport %(raider%) 20800
         * mat 20800 mpurge %(raider%)
         * wait 60s
         */

        /**
         * end
         * mgecho %(color_magenta%)*** A NEW RAID HAS BEGUN! ***%(color_normal%)
         * mecho Mob %(headmob%) will start at room %(startloc%) this raid.
         * mat %(startloc%) mload m %(headmob%)
         * mload m 20801
         * sval bellmob 20801 %(headmob%)
         */
    };

    return self;
})();