// Run from the game root: js153 tests/quest-browser.js
var global = this;
var loadAllQuests = null;
var Quest = {allQuests: []};
load('scripts/lib/quest-engine/-501_Quest.Task.js');
load('scripts/lib/QuestBrowser.js');
var values = {QUEST_1: 1, QUEST_2: 0, QUEST_3: -1, QUEST_4: 1};
var actor = {isValid: true, vnum: -1, name: 'Tester', quest: function(key) { return values[key] || 0; }};
function quest(id) {
    return {id: id, name: 'Quest ' + id, qvalStr: 'QUEST_' + id, summary: '"Hello " + actor.name', tasks: [],
        hasBegun: function(actor) { return actor.quest(this.qvalStr) > 0; }};
}
var first = quest(1), secret = quest(2), complete = quest(3), broken = quest(4);
first.tasks = [new Quest.Task(1, first, 'First step', null, '2', -1), new Quest.Task(2, first, 'Secret step', null, '1', 0)];
broken.tasks = [{hasUnlocked: function() { throw Error('bad task'); }}];
Quest.allQuests = [first, secret, complete, broken];
function request(ids, player, action, detailId) {
    return JSON.parse(global.questBrowser.getBrowserResponse(JSON.stringify({action: action || 'snapshot', requestId: 42, ids: ids, detailId: detailId}), player === undefined ? actor : player));
}
assertEq(!!request([], null).error, true);
assertEq(!!request([], {isValid: true, vnum: 100}).error, true);
[null, {}, '1', [1,2,3,4,5], ['1'], [1.5], [-1], [null]].forEach(function(ids) { assertEq(!!request(ids).error, true); });
assertEq(!!request([], actor, 'complete').error, true);
var list = request([]);
assertEq(list.requestId, 42);
assertEq(JSON.stringify(list.quests.map(function(q) { return q.id; })), '[1,4]');
assertEq(list.quests[0].tasks, undefined); // Do not evaluate task code for untracked quests.
var result = request([1,2,3,4], actor, 'snapshot', 1);
assertEq(result.quests.length, 2);
assertEq(result.quests[0].tasks.length, 1);
assertEq(result.quests[0].tasks[0].current, 0);
assertEq(result.quests[0].summary, 'Hello Tester');
assertEq(result.quests[0].ready, false);
assertEq(!!result.quests[1].error, true);
values.QUEST_1_TASK_1 = 2;
result = request([1]);
assertEq(result.quests[0].tasks.length, 2);
assertEq(result.quests[0].tasks[0].completed, true);
values.QUEST_1_TASK_2 = 1;
assertEq(request([1]).quests[0].ready, true);
values.QUEST_1 = -1;
assertEq(request([1]).quests.length, 1);
values.QUEST_1 = 1;
first.name = 'Edited live';
assertEq(request([1]).quests[0].name, 'Edited live');
Quest.allQuests = [broken];
assertEq(request([1]).quests.length, 1);
print('Quest browser: authentication, validation, accepted quests only, locked tasks, progress, readiness, completion, live edits, and broken quest isolation passed.');
