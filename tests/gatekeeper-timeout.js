// Run from the repository root: js153 tests/gatekeeper-timeout.js
// Exercise the real CALL script with a controllable timer and native-object stubs.
var _block = undefined;
var constants = { TO_ROOM: 1 };
var act = function () {};
var keeper, room, queued, commands, onCommand;
var getGateKeeperVector = function () {
    return [{ gateKeeperRoomVnum: 1700, otherRoomVnum: 19301,
              gateKeeperVnum: 1, pulsesToWaitForClose: 140 }];
};
var getRoom = function () { return room; };
var getMobAtRoom = function () { return keeper; };
var setTimeout = function (pulses, callback, args) {
    assertEq(pulses, 140);
    queued = function () { callback(args); };
};
load('scripts/scripts/8000_Call.js');

function prepare() {
    commands = [];
    onCommand = function () {};
    room = {
        vnum: 1700,
        direction: function (dir) { return dir === 0 ? { vnum: 19301 } : null; },
        doorExists: function () { return true; },
        doorName: function () { return 'gate'; }
    };
    keeper = {
        isValid: true,
        race: 1,
        room: room,
        comm: function (command) { commands.push(command); onCommand(command); }
    };
    var actor = { room: room, race: 1, send: function () {} };
    script8000(null, actor, room, '', {});
    assertEq(commands.join(','), 'unlock gate,open gate');
    commands = [];
}

prepare();
queued();
assertEq(commands.join(','), 'close gate,lock gate');

prepare();
keeper.isValid = false;
keeper.room = undefined;
queued();
assertEq(commands.length, 0);

prepare();
keeper.room = undefined;
queued();
assertEq(commands.length, 0);

prepare();
keeper.room = { vnum: 19301, doorName: function () { throw new Error('wrong room'); } };
queued();
assertEq(commands.length, 0);

prepare();
room.doorExists = function () { return false; };
queued();
assertEq(commands.length, 0);

prepare();
onCommand = function () { keeper.isValid = false; keeper.room = undefined; };
queued();
assertEq(commands.join(','), 'close gate');

prepare();
onCommand = function () { keeper.room = { vnum: 19301 }; };
queued();
assertEq(commands.join(','), 'close gate');

print('PASS: normal gate closing; purged, roomless, or relocated keeper; removed door; purge/movement during close');
