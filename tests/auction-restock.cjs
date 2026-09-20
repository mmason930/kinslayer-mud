// Run against a LOCAL MySQL container. Creates and always drops an isolated
// MyISAM database; never reads or writes the game's database.
// AUCTION_TEST_MYSQL_CONTAINER=mysql.kinslayermud.org node tests/auction-restock.cjs
const assert = require('node:assert/strict');
const {execFileSync} = require('node:child_process');
const {readFileSync} = require('node:fs');
const vm = require('node:vm');
const {randomUUID} = require('node:crypto');
const container = process.env.AUCTION_TEST_MYSQL_CONTAINER;
if (!container) throw Error('Set AUCTION_TEST_MYSQL_CONTAINER to a LOCAL test container');
const database = 'auction_regression_' + process.pid;
let lastInsertId = 0;
function sql(statement, useDatabase = true) {
  const output = execFileSync('docker', ['exec', '-i', container, 'sh', '-c',
    'MYSQL_PWD="$MYSQL_ROOT_PASSWORD" exec mysql -uroot --batch --raw "$@"',
    'mysql', ...(useDatabase ? [database] : [])], {
    input: statement + "; SELECT '__auction_insert_id__' AS marker, LAST_INSERT_ID() AS id;\n",
    encoding: 'utf8', stdio: ['pipe', 'pipe', 'pipe'],
  }).trimEnd().split('\n');
  lastInsertId = Number(output.pop().split('\t')[1]);
  assert.equal(output.pop(), 'marker\tid');
  if (!output.length) return [];
  const fields = output.shift().split('\t');
  return output.map(line => Object.fromEntries(line.split('\t').map((v, i) => [fields[i], v])));
}
const quote = s => "'" + String(s).replaceAll('\\', '\\\\').replaceAll("'", "\\'") + "'";
const source = readFileSync('scripts/scripts/addMissingRaresToAuction.js', 'utf8');
let state, context, self;
function fixtureObject(type = 'P', holder = '10', vnum = 1611, id = randomUUID()) {
  sql(`INSERT INTO objects VALUES(${quote(id)},${vnum},${quote(type)},${quote(holder)},${quote(type)},${quote(holder)},7,12345,'custom description')`);
  return id;
}
function auction(id, {house = 1, owner = 37000, active = 0, retrieved = 0, ends = 1000, bid = false} = {}) {
  sql(`INSERT INTO auctionItem(auction_id,object_id,owner_id,end_time,starting_price,buyout_price,active,retrieved,timestamp)
       VALUES(${house},${quote(id)},${owner},${ends},125,250,${active},${retrieved},500)`);
  const listing = lastInsertId;
  if (bid) sql(`INSERT INTO auctionBid VALUES(${listing},200)`);
  return listing;
}
function reset() {
  for (const table of ['auctionBid','auctionItem','objects','users','auctions','obj_protos']) sql('TRUNCATE ' + table);
  sql("INSERT INTO auctions VALUES(1),(2)");
  sql("INSERT INTO users VALUES(37000,NOW()),(37001,NOW()),(1,'2020-01-01'),(2,'2020-01-01'),(10,'2020-01-01'),(11,NOW())");
  sql('INSERT INTO obj_protos VALUES(1611,32768),(2406,32768),(1335,32768),(1334,32768),(1511,32768),(1121,32768),(1416,32768),(999,32768)');
  state = {live: new Map(), prototypes: new Map(), logs: [], stores: 0, loads: 0, failQuery: () => false, failSave: false, missing: new Set()};
  class Item {
    constructor(id, vnum) {this.id=id; this.vnum=vnum; this.cost=125; this.isValid=true; state.live.set(id,this);}
    moveToChar(character) {this.carriedBy=character;}
    storeAndExtract(type, holder) {
      // Native persistence is separately tested in C++; here SQL remains real.
      assert.equal(sql(`SELECT id FROM auctionItem WHERE object_id=${quote(this.id)} AND active=1`).length, 0,
        'must not publish an item before saving it');
      if (state.failSave) return false;
      if (!sql(`SELECT id FROM objects WHERE id=${quote(this.id)}`).length)
        fixtureObject(type, holder, this.vnum, this.id);
      sql(`UPDATE objects SET holder_type=${quote(type)},holder_id=${quote(holder)},top_level_holder_type=${quote(type)},top_level_holder_id=${quote(holder)},pos=-1 WHERE id=${quote(this.id)}`);
      this.isValid=false; state.live.delete(this.id); state.stores++;
      // Deliberately do not decrement prototype.count: item still exists in storage.
      return true;
    }
    extract() {throw Error('destructive extraction must not be used for auction storage');}
  }
  self = {isValid:true, loadObj(vnum) {
    state.loads++;
    const proto=state.prototypes.get(vnum); proto.count++;
    const item=new Item(randomUUID(),vnum); item.moveToChar(this); return item;
  }};
  context = vm.createContext({JSObject:Item, time:()=>2000000000, random:()=>1,
    getObjectList:()=>Array.from(state.live.values()),
    getSval:()=>state.retryItems, setSval:(_self,_id,_key,value)=>{state.retryItems=value;},
    getObjProto:vnum=>state.prototypes.get(vnum) || {max:0,count:0},
    sqlEscapeQuoteString:quote, sqlInsertID:()=>lastInsertId,
    sqlQuery:statement=>{
      if (state.failQuery(statement)) return null;
      const result=sql(statement);
      return {get hasNextRow(){return result.length>0;},get getRow(){const row=result.shift();return {get:key=>row[key]};}};
    },
    loadSingleObjectFromDatabase:id=>{
      if(state.missing.has(id)) return null;
      const row=sql(`SELECT vnum FROM objects WHERE id=${quote(id)}`)[0];
      return row ? new Item(id,Number(row.vnum)) : null;
    }, mudLog:(_a,_b,message)=>state.logs.push(message),
  });
  vm.runInContext(source,context);
}
function run() {context.addMissingRaresToAuction(self,null,null,'',{});}
function active() {return sql('SELECT * FROM auctionItem WHERE active=1');}
function test(name, fn) {reset();fn();console.log('PASS: '+name);}
try {
  sql(`CREATE DATABASE ${database}`,false);
  sql(`CREATE TABLE objects(id varchar(36) PRIMARY KEY,vnum int,holder_type char(1),holder_id varchar(36),top_level_holder_type char(1),top_level_holder_id varchar(36),pos int,bitv0 bigint,obj_str text) ENGINE=MyISAM`);
  sql('CREATE TABLE users(user_id int PRIMARY KEY,last_logon datetime) ENGINE=MyISAM');
  sql('CREATE TABLE obj_protos(vnum int PRIMARY KEY,extra_flags int) ENGINE=MyISAM');
  sql('CREATE TABLE auctions(vnum int PRIMARY KEY) ENGINE=MyISAM');
  sql('CREATE TABLE auctionItem(id int PRIMARY KEY AUTO_INCREMENT,auction_id int,object_id varchar(36),owner_id int,end_time int,starting_price bigint,buyout_price bigint,active tinyint,retrieved tinyint,timestamp int) ENGINE=MyISAM');
  sql('CREATE TABLE auctionBid(ai_id int,bid_amount bigint) ENGINE=MyISAM');
  test('older binaries leave all items untouched',()=>{
    context.JSObject.prototype.storeAndExtract=undefined;
    const id=fixtureObject('A','1'); auction(id); run();
    assert.equal(active().length,0); assert.equal(state.stores,0);
    assert.match(state.logs[0],/requires the game build/);
  });
  test('renew unsold house stock once and preserve its saved data',()=>{
    const first=fixtureObject('A','1'), second=fixtureObject('A','2');
    auction(first); const latest=auction(first); auction(second,{house:2,owner:37001});
    const before=sql('SELECT * FROM objects ORDER BY id');
    run();assert.equal(active().length,2);assert.equal(state.loads,0);assert.equal(state.stores,0);
    assert.equal(active().find(a=>a.object_id===first).id,String(latest));
    assert.equal(sql(`SELECT id FROM auctionItem WHERE object_id=${quote(first)} AND retrieved=0`).length,1);
    assert.deepEqual(sql('SELECT * FROM objects ORDER BY id'),before);
    run();assert.equal(active().length,2);assert.deepEqual(sql('SELECT * FROM objects ORDER BY id'),before);
  });
  test('never reclaim active auctions, player listings, or winning bids',()=>{
    const live=fixtureObject('A','1');auction(live,{active:1,ends:2100000000});
    const won=fixtureObject('A','1');auction(won,{bid:true});auction(won);
    const player=fixtureObject('A','1');auction(player,{owner:10});auction(player);
    const before=sql('SELECT * FROM auctionItem ORDER BY id');
    run();assert.deepEqual(sql('SELECT * FROM auctionItem ORDER BY id'),before);
  });
  test('reclaim only eligible offline auctionable rares, preserving flags and descriptions',()=>{
    const reclaimed=[fixtureObject('P','10'),fixtureObject('S','10'),fixtureObject('C','500')];
    const protectedIds=[fixtureObject('P','11'),fixtureObject('R','1'),fixtureObject('P','10',999),fixtureObject('P','10',1611,'malformed-uuid')];
    const inWorld=fixtureObject('P','10');
    const live=new context.JSObject(inWorld,1611);live.carriedBy={};protectedIds.push(inWorld);
    run();assert.equal(active().length,3);assert.equal(state.stores,3);
    for(const id of reclaimed){const row=sql(`SELECT * FROM objects WHERE id=${quote(id)}`)[0];assert.equal(row.holder_type,'A');assert.equal(row.bitv0,'12345');assert.equal(row.obj_str,'custom description');}
    for(const id of protectedIds) assert.notEqual(sql(`SELECT holder_type FROM objects WHERE id=${quote(id)}`)[0].holder_type,'A');
  });
  test('missing objects are skipped without aborting other eligible items',()=>{
    const missing=fixtureObject('P','10');state.missing.add(missing);fixtureObject('P','10');run();
    assert.equal(active().length,1);assert.match(state.logs.join('\n'),/could not load/);
  });
  test('mint one per cycle, preserve counts, and stop at the rare cap',()=>{
    state.prototypes.set(1611,{max:2,count:0});
    run();assert.equal(active().length,1);assert.equal(state.prototypes.get(1611).count,1);
    run();assert.equal(active().length,2);assert.equal(state.prototypes.get(1611).count,2);
    run();assert.equal(active().length,2);assert.equal(state.loads,2);
  });
  test('persisted counts prevent overproduction even if the live counter is stale',()=>{
    fixtureObject('P','11');fixtureObject('P','11');state.prototypes.set(1611,{max:2,count:0});run();
    assert.equal(state.loads,0);assert.equal(active().length,0);
  });
  test('failed saves retain live items and retry their inactive reservation',()=>{
    const id=fixtureObject('P','10');state.failSave=true;run();
    assert.equal(active().length,0);assert.equal(state.live.get(id).isValid,true);
    const pending=sql('SELECT * FROM auctionItem');assert.equal(pending.length,1);assert.equal(pending[0].retrieved,'1');
    state.failSave=false;run();assert.equal(active().length,1);assert.equal(state.live.has(id),false);
    assert.equal(sql('SELECT COUNT(*) AS count FROM auctionItem')[0].count,'1');
  });
  test('failed activation resumes without recreating or duplicating saved items',()=>{
    const id=fixtureObject('P','10');state.failQuery=q=>q.startsWith('UPDATE auctionItem a JOIN');run();
    assert.equal(active().length,0);assert.equal(sql(`SELECT holder_type FROM objects WHERE id=${quote(id)}`)[0].holder_type,'A');
    assert.equal(sql('SELECT retrieved FROM auctionItem')[0].retrieved,'0');
    state.failQuery=()=>false;run();assert.equal(active().length,1);assert.equal(state.stores,1);
  });
  test('unconfirmed saves after a restart remain inactive for inspection',()=>{
    const id=fixtureObject('A','1');auction(id,{ends:0,retrieved:1});run();
    assert.equal(active().length,0);assert.match(state.logs.join('\n'),/needs inspection/);
  });
  test('unrelated holding-mob inventory is never auctioned',()=>{
    const item=new context.JSObject(randomUUID(),1611);item.moveToChar(self);run();
    assert.equal(active().length,0);assert.equal(item.isValid,true);
  });
  test('SQL errors stop the batch without publishing unsaved items',()=>{
    fixtureObject('P','10');state.failQuery=q=>q.startsWith('INSERT INTO auctionItem');run();
    assert.equal(active().length,0);assert.equal(state.stores,0);assert.match(state.logs.join('\n'),/database query failed/);
    state.failQuery=()=>false;run();assert.equal(active().length,1);assert.equal(state.stores,1);
  });
} finally {
  sql(`DROP DATABASE IF EXISTS ${database}`,false);
}
