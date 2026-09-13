// Run from the game root: js153 tests/help-access.js
var global = this;
function JSCharacter() {}
var constants = {BRF: 1}, nrm = '', cyn = '', red = '', bld = '', grn = '';
function getCharCols() {}
function mudLog() {}
function str_cmp(a, b) { return a.toLowerCase() !== b.toLowerCase(); }
var sent = [], currentId = null, queries = [], databaseLevel = 100, onlinePlayer = null;
function getSval() { return currentId; }
function setSval(actor, script, key, value) { currentId = value; }
function sqlEscapeQuoteString(value) { return value == null ? 'NULL' : "'" + String(value).replace(/'/g, "''") + "'"; }
function sqlEncodeQuoteDate() { return "'2026-09-13 00:00:00'"; }
function sqlInsertID() { return 99; }
function sqlQuery(sql) {
    queries.push(sql);
    return {hasNextRow: true, getRow: {getInteger: function() { return databaseLevel; }}};
}
function getPlayerById() { return onlinePlayer; }
function getUserNameByUserId() { return 'Editor'; }
Date.prototype.strftime = function() { return 'Sep 13 2026'; };
load('scripts/lib/HelpManager.js');
var manager = global.helpManager = Object.create(HelpManager.prototype);
function file(id, parent, level, name) {
    var f = manager.createHelpFile(parent);
    f.id = id; f.minimumLevel = level; f.name = name; f.keywords = name;
    f.description = '"Description for ' + name + '"';
    f.createdByUserId = 1; f.createdDatetime = f.lastModifiedDatetime = new Date();
    return f;
}
manager.helpFiles = {1: file(1, null, 0, 'Contents'), 2: file(2, 1, 100, 'Staff'),
    3: file(3, 2, 0, 'Staff child'), 4: file(4, 1, 20, 'Advanced'), 5: file(5, 1, 0, 'Basics')};
var actor = {isValid: true, level: 19, id: 1, send: function(text) { sent.push(text); }};
function browse(action, id) {
    return JSON.parse(manager.getBrowserResponse(JSON.stringify({action: action, helpFileId: id}), actor));
}
function ids(files) { return JSON.stringify(files.map(function(f) { return f.id; })); }
assertEq(ids(browse('index').topics), '[1,5]');
assertEq(manager.getSearchedFile('Staff', actor), null);
assertEq(manager.getSearchedFile('Staff child', actor), null);
assertEq(manager.getSearchedFile('Advanced', actor), null);
assertEq(manager.getSearchedFile('Basics', actor).id, 5);
assertEq(ids(manager.getVisibleHelpFilesByParentId(1, actor)), '[5]');
assertEq(manager.getHelpFilesByParentId(1).length, 3); // Staff editor remains unfiltered.
manager.helpFiles[2].description = '(function(){throw Error("Restricted source was evaluated")})()';
assertEq(browse('article', 2).article, undefined);
assertEq(browse('article', 3).article, undefined);
assertEq(browse('article', 2).error, browse('article', 999).error);
manager.getHelpPage(actor, manager.helpFiles[2]);
assertEq(sent.join(''), 'This help file is unavailable.');
sent = [];
manager.getHelpPage(actor, manager.helpFiles[1]);
assertEq(sent.join('\n').indexOf('Staff'), -1);
assertEq(sent.join('\n').indexOf('Advanced'), -1);
assertEq(sent.indexOf('[1] Basics') >= 0, true);
assertEq(manager.renderBrowserDescription(actor, manager.helpFiles[2]), '');
actor.level = 20;
assertEq(browse('article', 4).article.name, 'Advanced');
actor.level = 100;
assertEq(browse('index').topics.length, 5);
assertEq(browse('article', 3).article.name, 'Staff child');
manager.helpFiles[2].minimumLevel = 101;
assertEq(browse('article', 3).article, undefined); // Live edits, no cached authorization.
manager.setCurrentFile(actor, 3);
assertEq(manager.getCurrentFile(actor), null);
actor.level = 105;
assertEq(manager.getCurrentFile(actor).id, 3);
manager.helpFiles[2].parentId = 3;
assertEq(manager.canViewHelpFile(actor, manager.helpFiles[3]), false); // Cycle.
manager.helpFiles[2].parentId = 999;
assertEq(manager.canViewHelpFile(actor, manager.helpFiles[3]), false); // Missing ancestor.
manager.helpFiles[2].parentId = 1;
assertEq(manager.getHelpFileById('__proto__'), null);
[null, '', '100', -1, 106, 1.5, NaN, Infinity].forEach(function(level) {
    assertEq(manager.isValidMinimumLevel(level), false);
});
[0, 1, 99, 100, 105].forEach(function(level) { assertEq(manager.isValidMinimumLevel(level), true); });

// Portal authorization, new/edit persistence, validation before mutation, legacy editor tabs.
function setTimeout() {}
load('scripts/lib/WebSocketCommandProcessor.js');
var processor = new WebSocketCommandProcessor();
processor.loadCommandProcessors();
function portal(command, id) { return JSON.parse(processor.processCommand(JSON.stringify(command), id === undefined ? 1 : id)); }
databaseLevel = 99;
assertEq(!!portal({method: 'Load Help File', helpFileId: 2}).error, true);
assertEq(!!portal({method: 'Save Help File', helpFileId: 2, minimumLevel: 0}).errors, true);
assertEq(manager.helpFiles[2].minimumLevel, 101);
databaseLevel = 100;
assertEq(portal({method: 'Load Help File', helpFileId: 2}).minimumLevel, 101);
onlinePlayer = {isValid: true, level: 1};
assertEq(!!portal({method: 'Load Help File', helpFileId: 2}).error, true);
onlinePlayer = null;
assertEq(!!portal({method: 'Load Help File', helpFileId: 2}, '1 OR 1=1').error, true);
var command = {method: 'Save Help File', helpFileId: 2, minimumLevel: 100, name: 'Staff', parentId: 1, keywords: 'staff', description: '"Staff text"'};
[null, -1, 106, 1.5, '100'].forEach(function(level) {
    command.minimumLevel = level;
    assertEq(!!portal(command).errors, true);
    assertEq(manager.helpFiles[2].minimumLevel, 101);
});
command.minimumLevel = 100;
assertEq(portal(command).minimumLevel, 100);
assertEq(queries[queries.length-1].indexOf('minimum_level = 100,   created_by_user_id') >= 0, true);
delete command.minimumLevel;
assertEq(portal(command).minimumLevel, 100);
command.helpFileId = null;
assertEq(portal(command).minimumLevel, 0);
assertEq(manager.helpFiles[99].minimumLevel, 0);
assertEq(queries[queries.length-1].indexOf('`minimum_level`') >= 0, true);
assertEq(portal({method: 'Load Help File', helpFileId: 1}).parent, null); // No leaked previous parent.

// Loading from SQL retains the level (including level 0).
var rows = [0, 100], originalSqlQuery = sqlQuery;
sqlQuery = function() { return {
    get hasNextRow() { return rows.length > 0; },
    get getRow() { var level = rows.shift(); return {
        get: function(column) { return column === 'id' ? level + 1 : null; },
        getInteger: function(column) { return column === 'minimum_level' ? level : 1; },
        getTimestamp: function() { return new Date(); }
    }; }
}; };
var loaded = new HelpManager();
assertEq(loaded.helpFiles[1].minimumLevel, 0);
assertEq(loaded.helpFiles[101].minimumLevel, 100);
sqlQuery = originalSqlQuery;

// In-game editor validates and saves the field; numbered navigation uses the filtered list.
var vOLC = [];
function bootOLC() { bootHelpeditOLC(); }
function strPadding() { return ''; }
load('scripts/lib/olc/bootHelpeditOLC.js');
var olc = {helpFile: manager.helpFiles[5], switchToMode: function(mode) { this.mode = mode; }};
actor.getOLC = function() { return olc; };
actor.quest = function() { return 0; };
var modes = {};
vOLC[0].modes.forEach(function(mode) { modes[mode.mode] = mode; });
modes.MODE_EDIT_FILE.parser(actor, 'L', ['L']);
assertEq(olc.mode, 'MODE_EDIT_FILE_LEVEL');
['', '-1', '106', '2.5', 'abc'].forEach(function(value) {
    modes.MODE_EDIT_FILE_LEVEL.parser(actor, '', [value]);
    assertEq(olc.helpFile.minimumLevel, 0);
});
modes.MODE_EDIT_FILE_LEVEL.parser(actor, '', ['20']);
assertEq(olc.helpFile.minimumLevel, 20);
assertEq(olc.mode, 'MODE_EDIT_FILE');
modes.MODE_EDIT_FILE.parser(actor, 'Q', ['Q']);
assertEq(queries[queries.length-1].indexOf('minimum_level = 20,') >= 0, true);
var _block = null;
function getArgList(args) { return args.split(' '); }
function strn_cmp(a, b, length) { return a.toLowerCase().slice(0, length) === b.toLowerCase().slice(0, length); }
function act() {}
actor.detach = function() {};
load('scripts/scripts/13_newbieguidehelppaging-Koradin.js');
actor.level = 20;
currentId = 1;
script13(null, actor, null, '1', null);
assertEq(currentId, 4); // Advanced sorts before Basics; Staff has been omitted.
currentId = 3;
script13(null, actor, null, 'back', null); // A newly inaccessible current page must not crash.
print('Help access: level boundaries, inherited restrictions, hidden metadata/source, live edits, navigation, SQL persistence, portal authorization and both editor paths passed.');
