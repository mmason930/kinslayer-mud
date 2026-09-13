// Run from the game directory: js153 tests/pathfinding-scripts.js
var constants = {RACE_HUMAN: 1, AFF_NOQUIT: 1, CL_OFF: 0, POS_STANDING: 1};
var bld = '', nrm = '';
function getCharCols() {}
function setTimeout() {}
function setSval() {}
function dirToText(direction) { return 'dir' + direction; }
function random(low, high) { return low; }
var destination = {vnum: 999};
function getRoom() { return destination; }
function getSval() { return 999; }

// Only waitpulse syntax needs the game's generator preprocessor; it is outside
// the movement logic tested here and is replaced by a no-op in this shell test.
evaluate(read('scripts/quests/zind-quests/9852_Long-hairedmanbrushespastALDERQUEST.js').replace('waitpulse 1;', ''));
load('scripts/quests/zind-quests/9853_Long-hairedmanmovesaroundALDERQUEST.js');

function checkQuest(script, limit, useDoor, unreachable, legacy) {
    var calls = 0, separate = 0, moves = 0, commands = [], nextDirection = 0;
    var actor = {
        race: 1, quest: function() { return 0; }, affectedBy: function() { return false; },
        qval: function() {}, send: function() {}, loadObj: function() {}, comm: function() {}
    };
    function makeRoom() {
        var room = {
            people: [],
            routeTo: function(target) {
                assertEq(target, destination);
                ++calls;
                return {firstStep: nextDirection, distance: unreachable ? -1 : 20 - moves};
            },
            distanceTo: function() {
                if (!legacy) throw Error('redundant distance search');
                ++calls; return unreachable ? -1 : 20 - moves;
            },
            firstStep: function() { ++separate; return nextDirection; },
            doorExists: function() { return useDoor && moves === 0; },
            doorName: function(direction) { return 'door' + direction; }
        };
        if (legacy) delete room.routeTo;
        return room;
    }
    var mob = {vnum: 9850, room: makeRoom(), name: 'NPC', comm: function(command) {
        commands.push(command);
        if (command.indexOf('pick ') === 0) {
            nextDirection = 2;
            this.room = makeRoom(); // a pick trigger moves the NPC
        } else if (command.indexOf('open ') === 0) {
            nextDirection = 3; // an open trigger changes the route
        } else {
            ++moves;
            this.room = makeRoom();
        }
    }};
    script(mob, actor, mob.room, '', {});
    assertEq(moves, unreachable ? 0 : limit);
    assertEq(calls, unreachable ? 1 : limit);
    assertEq(separate, (useDoor && !unreachable ? 2 : 0) + (legacy ? calls : 0));
    if (useDoor && !unreachable) {
        assertEq(commands[0], 'pick door0');
        assertEq(commands[1], 'open door2');
        assertEq(commands[2], 'dir3');
    }
}
checkQuest(script9852, 4, false, false);
checkQuest(script9852, 4, true, false);
checkQuest(script9852, 4, false, true);
checkQuest(script9853, 8, false, false);
checkQuest(script9853, 8, true, false);
checkQuest(script9853, 8, false, true);

checkQuest(script9852, 4, true, false, true);
checkQuest(script9853, 8, false, false, true);
checkQuest(script9853, 8, false, true, true);

load('scripts/lib/raids/Raid.Util.js');
Raid.Util.canShout = function() { return false; };
Raid.Util.debugSend = function() {};
Raid.Util.getValidRoomsInZone = function() { return []; };
Raid.Util.getFollowersInRoom = function() { return []; };
var raidSearches = 0, raidMoves = [];
var headRoom = {
    vnum: 1, zoneVnum: 1,
    routeTo: function(target) { ++raidSearches; return {firstStep: 2, distance: 10}; },
    firstStep: function() { throw Error('redundant direction search'); },
    distanceTo: function() { throw Error('redundant distance search'); },
    doorIsClosed: function() { return false; }
};
// Keep the deterministic advance, but suppress the unrelated random detour.
Raid.Util.probabilityOfMovingWrongDirection = 0;
Raid.Util.headMobMove({position: 1, room: headRoom, raidLocation: {endRoomVnum: 999},
    comm: function(command) { raidMoves.push(command); }});
assertEq(raidSearches, 1);
assertEq(raidMoves.join(','), 'dir2');
// Older game binaries can still run scripts pulled before the native upgrade.
delete headRoom.routeTo;
headRoom.firstStep = function() { return 2; };
headRoom.distanceTo = function() { return 10; };
Raid.Util.headMobMove({position: 1, room: headRoom, raidLocation: {endRoomVnum: 999},
    comm: function(command) { raidMoves.push(command); }});
assertEq(raidSearches, 1);
assertEq(raidMoves.join(','), 'dir2,dir2');
print('PASS: raid uses one route search; quests combine distance/direction, preserve movement limits and refresh after command side effects');
