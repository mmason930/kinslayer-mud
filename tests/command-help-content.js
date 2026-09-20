// Run from the game root: js153 tests/command-help-content.js
var global = this;
function JSCharacter() {}
var constants = {BRF: 1};
function mudLog(type, level, message) { throw Error(message); }
function str_cmp(a, b) { return a.toLowerCase() !== b.toLowerCase(); }
function strlenDisplay(value) { return value.replace(/\x1b\[[0-9;]*m/g, '').length; }
load('scripts/lib/HelpManager.js');
load('scripts/lib/-38_strColFormat.js');
var payload = typeof embeddedPayload === 'undefined'
    ? JSON.parse(read('tests/fixtures/command-help.json')) : embeddedPayload;
var manager = Object.create(HelpManager.prototype);
manager.helpFiles = {
    1: {id: 1, parentId: null, minimumLevel: 0, name: 'Table of Contents', keywords: 'contents'},
    2: {id: 2, parentId: 1, minimumLevel: 0, name: 'Commands', keywords: 'commands'},
    3: {id: 3, parentId: 1, minimumLevel: 100, name: 'Immortals', keywords: 'immortals'}
};
var plainActor = {isValid: true, level: 1};
payload.entries.forEach(function(entry, index) {
    var file = {id: 1000 + index, parentId: entry.parentName === 'Immortals' ? 3 : 2,
        minimumLevel: entry.minimumLevel, name: entry.name, keywords: entry.keywords,
        syntax: entry.syntax, description: entry.description};
    manager.helpFiles[file.id] = file;
});
payload.entries.forEach(function(entry, index) {
    var file = manager.helpFiles[1000 + index];
    var actor = {isValid: true, level: entry.minimumLevel};
    var rendered = manager.renderBrowserDescription(actor, file);
    assertEq(rendered.length > 100, true);
    assertEq(rendered.indexOf('undefined'), -1);
    assertEq(rendered.indexOf('`'), -1);
    assertEq(rendered.indexOf('\x1b[36m') >= 0, true);
    if (entry.examples.length) assertEq(rendered.indexOf('\x1b[33m') >= 0, true);
    var nrm = '', bld = '', cyn = '', yel = '';
    var plain = eval(file.description);
    assertEq(plain.indexOf('\x1b'), -1);
    assertEq(plain.replace(/\s+/g, ' ').trim(),
        rendered.replace(/\x1b\[[0-9;]*m/g, '').replace(/\s+/g, ' ').trim());
    assertEq(manager.getSearchedFile(entry.command, actor).id, file.id);
    var article = JSON.parse(manager.getBrowserResponse(JSON.stringify({
        action: 'article', helpFileId: file.id}), actor)).article;
    assertEq(article.description, rendered);
    if (entry.minimumLevel > 0) {
        actor.level--;
        assertEq(manager.getSearchedFile(entry.command, actor), null);
        assertEq(JSON.parse(manager.getBrowserResponse(JSON.stringify({
            action: 'article', helpFileId: file.id}), actor)).article, undefined);
        assertEq(manager.getSearchedFile(entry.command, plainActor), null);
    }
});
var topics = JSON.parse(manager.getBrowserResponse('{"action":"index"}', plainActor)).topics;
topics.forEach(function(topic) { assertEq(manager.helpFiles[topic.id].minimumLevel <= 1, true); });
assertEq(payload.entries.length, 140);
assertEq(topics.length, 121); // 119 public pages plus the two public category pages.
print('PASS: 140 help expressions, color/plain rendering, lookup, browser articles and level restrictions');
