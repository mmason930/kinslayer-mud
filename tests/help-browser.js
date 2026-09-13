// Run from the game repository root: js153 tests/help-browser.js
function JSCharacter() {}
load('scripts/lib/HelpManager.js');
var manager = Object.create(HelpManager.prototype);
var constants = {BRF: 1};
var logged = [];
function mudLog(level, minimum, message) { logged.push(message); }
manager.helpFiles = {
    1: {id: 1, parentId: null, name: 'Root', keywords: null},
    2: {id: 2, parentId: '1', name: 'Score', keywords: 'hp, score', syntax: 'score',
        description: 'cyn + "HP: " + actor.hp + nrm'},
    3: {id: 3, parentId: 1, name: 'Broken', description: '(function(){throw Error("bad page")})()'},
    4: {id: 4, parentId: 1, name: 'Markup', description: '\'<img src=x onerror="bad()">\''}
};
var actor = {isValid: true, hp: 23};
function request(command, player) {
    command.requestId = 42;
    return JSON.parse(manager.getBrowserResponse(JSON.stringify(command), player === undefined ? actor : player));
}
assertEq(request({action: 'index'}, null).error, 'Sign in to browse the help files.');
assertEq(request({action: 'index'}, {isValid: false}).topics, undefined);
var index = request({action: 'index'});
assertEq(index.requestId, 42);
assertEq(index.topics.length, 4);
assertEq(index.topics[1].parentId, 1);
assertEq(index.topics[0].keywords, '');
assertEq(index.topics[1].description, undefined);
var cyn = 'unchanged';
assertEq(request({action: 'article', helpFileId: 2}).article.description, '\x1b[36mHP: 23\x1b[0m');
assertEq(cyn, 'unchanged');
assertEq(request({action: 'article', helpFileId: 99}).article, undefined);
['2', -2, 2.5, {}, '__proto__'].forEach(function(id) {
    assertEq(request({action: 'article', helpFileId: id}).error, 'Invalid help request.');
});
assertEq(request({action: 'Save Help File', description: 'throw Error()'}).error, 'Invalid help request.');
assertEq(!!request({action: 'article', helpFileId: 3}).article.error, true);
assertEq(logged.length, 1);
assertEq(request({action: 'article', helpFileId: 4}).article.description, '<img src=x onerror="bad()">');
print('Help browser: authentication, metadata, dynamic text, local colors, validation and page errors passed.');
