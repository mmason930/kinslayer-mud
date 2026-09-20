const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const root = path.join(__dirname, '..');
const context = {JSCharacter: function(){}, global: {}, constants: {CLASS_CHANNELER: 1, CLASS_THIEF: 2, CLASS_RANGER: 3},
    getSval: actor => actor.step, getObjectClass: x => Array.isArray(x) ? 'Array' : 'Object',
    isName: (name, keywords) => keywords.split(' ').includes(name)};
vm.createContext(context);
for (const file of ['HelpManager.js', 'ClientTools.js', '-22_getObjInListVis.js'])
    vm.runInContext(fs.readFileSync(path.join(root, 'scripts/lib', file), 'utf8'), context);
const manager = Object.create(context.HelpManager.prototype);
manager.helpFiles = {
    1: {id: 1, name: 'Look', keywords: 'look', syntax: 'look [target]', parentId: null, minimumLevel: 0},
    2: {id: 2, name: 'Staff', keywords: 'staff', syntax: 'secret', parentId: null, minimumLevel: 100},
    3: {id: 3, name: 'Restricted child', parentId: 2, minimumLevel: 0}
};
context.global.helpManager = manager;
let saved = null, writes = 0;
const actor = {isValid: true, level: 1, class: 0, step: 1, countJS: id => id === 501,
    getPval: () => saved, setPval: (key, value, immediate) => {
        assert.equal(key, 'WEB_CLIENT_SETTINGS'); assert.equal(immediate, true); saved = value; ++writes; return true;
    }};
const tools = context.global.clientTools;
const response = request => JSON.parse(tools.response(JSON.stringify(request), actor));
assert.deepEqual(response({action: 'init'}).topics.map(t => t.id), [1]);
assert.equal(response({action: 'init'}).topics[0].syntax, 'look [target]');
for (let step = 0; step <= 17; ++step) {
    actor.step = step;
    const lesson = response({action: 'snapshot'}).tutorial;
    assert.equal(lesson.step, step); assert.equal(lesson.active, true);
    assert.ok(lesson.title && lesson.text && lesson.help);
    if (step === 0 || step === 14) assert.equal(lesson.command, '');
}
actor.step = 9;
for (const [classId, weapon] of [[0, 'mace'], [1, 'spear'], [2, 'knife'], [3, 'sword']]) {
    actor.class = classId;
    assert.equal(response({action: 'snapshot'}).tutorial.command, 'wield ' + weapon);
}
assert.equal(writes, 0, 'Reading the companion never advances or saves tutorial progress');
actor.countJS = () => false;
assert.equal(response({action: 'snapshot'}).tutorial.active, false);
assert.equal(response({action: 'settings', settings: {tutorial: false, suggestions: false}}).saved, true);
assert.deepEqual(response({action: 'init'}).settings, {tutorial: false, suggestions: false});
assert.throws(() => response({action: 'settings', settings: []}));
assert.throws(() => response({action: 'settings', settings: {large: 'a'.repeat(12001)}}));
actor.setPval=()=>false;
assert.equal(response({action:'settings',settings:{fontSize:17}}).saved,false);
assert.match(response({action:'settings',settings:{fontSize:17}}).error,/could not be saved/);
const items = [{id: 'first', namelist: 'sword', isValid: true}, {id: 'second', namelist: 'sword', isValid: true}];
const viewer = {canSee: obj => !obj.invisible};
assert.equal(context.getObjInListVis(viewer, '@second', items), items[1]);
assert.equal(context.getObjInListVis(viewer, '@unknown', items), null);
assert.equal(context.getObjInListVis(viewer, '@second', [items[0]]), null);
assert.equal(context.getObjInListVis(viewer, '2.sword', items), items[1]);
items[1].invisible = true;
assert.equal(context.getObjInListVis(viewer, '@second', items), null);
console.log('Client tools server: lesson mapping, class weapons, no duplicate tutorial progress, help permissions, saved preferences, and exact script item selectors passed.');
