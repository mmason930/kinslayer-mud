var addMissingRaresToAuction =  function(self, actor, here, args, extra) {
    var date = new Date();
    // Run every Thursday
    if (date.getDay() != 4) {
        return;
    }
    // Purge all inactive RARE items
    var query = '';
    query = "SELECT u.username, u.last_logon, o.vnum, o.id AS object_id, op.sdesc, o.top_level_holder_type FROM objects o "
                + "LEFT JOIN users u ON u.user_id = o.top_level_holder_id "
                + "LEFT JOIN obj_protos op ON o.vnum = op.vnum "
                + "WHERE op.extra_flags & ( 1 <<15 ) "
                + "AND (u.last_logon <= DATE_SUB( CURDATE( ) , INTERVAL 30 DAY )  OR o.top_level_holder_type = 'O')";
    var result = sqlQuery(query);
    
    while( result.hasNextRow ) {
        var row = result.getRow;
        
        var obj = null;
        obj = loadSingleObjectFromDatabase(row.get('object_id'));
        obj.moveToChar(self);
        if (row.get('top_level_holder_type') == 'P') {
            mudLog(2, 104, "Inactive item extracted from user " + row.get('username') + " with last logon time of " + row.get('last_logon') + " : vnum - " + row.get('vnum') + " sdesc - " + row.get('sdesc') + " id - " + row.get('object_id'));
        } else {
            mudLog(2, 104, "Inactive item extracted, is not on user: vnum - " + row.get('vnum') + " sdesc - " + row.get('sdesc') + " id - " + row.get('object_id'));        
        }
    }

    // Get list of all RARE items
    var sql = "SELECT vnum FROM obj_protos WHERE extra_flags & ( 1 <<15 )";
    var result = sqlQuery(sql);
    
    var rareItems = [];
    while (result.hasNextRow) {
        var row = result.getRow;
        rareItems.push(row.get("vnum"));
    }
    
    var auctionableRareItems = [
        1611,   // a platinum mace with steel spikes
        2406,   // a golden etched obsidian morning star
        1335,   // a giant obsidian-studded bastard sword
        1334,   // a platinum handled serrated long blade
        1511,   // a gold headed double-bladed battle axe
        1121,   // a two-fisted pike with a bronzed steel shaft
        21792,  // a set of Sharan-marked boots
        21793,  // a set of Sharan-marked vambraces
        21794,  // a plumed ebony burgonet with Sharan markings
        21795,  // a pair of Sharan-marked steel-reinforced ebony gauntlets
        21796,  // a pair of Sharan-marked ebony greaves
        21797,  // a Sharan-marked, steel-reinforced ebony leather belt
        21798,  // Sharan-marked, ebony pauldrons
        21799,  // a Sharan-emblazoned cuirass
        1416    // a sharp dagger of obsidian
    ];
    
    // Load items for auction
    for each (var vnum in auctionableRareItems) {
        var obj = getObjProto(vnum);
        if (obj.count < obj.max) {
            // Roll some dice, can load up to as many of this item as are not in the game
            for (var i=0; i < (obj.max-obj.count); i++) {
                // 50% chance of loading item
                if (random(0, 9) < 5) {
                    self.loadObj(vnum);
                }
            }
        }
    }
    
    // Load items into auction
    for each (var item in self.inventory) {
        // Flip a coin and decide which race gets the auction
        var auction_id = random(1, 2);
        var owner_id = null;
        if (auction_id == 1) {
            owner_id = 37000;
        } else {
            owner_id = 37001;
        }
        // Insert item to auction
        var insertSql = "INSERT INTO auctionItem (auction_id, object_id, owner_id, end_time, starting_price, buyout_price, active, timestamp) "
                            + "VALUES("+auction_id+", '"+item.id+"', "+owner_id+", "+(time()+604800)+", "+item.cost+", "+item.cost*2+", 1, "+time()+")";
        var result = sqlQuery(insertSql);
		// Destroy item and insert a copy into the DB with updated holder info
		var insertSql = "INSERT INTO objects VALUES ('"+item.id+"', "+item.vnum+", -1, 'Item created for auction by script #33245.', 0, 0, 0, 0, 0, 0, 'A', "+auction_id+", 'A', "+auction_id+", NOW(), '', 0, 0)";
		item.extract();
		var result = sqlQuery(insertSql);
        mudLog(2, 104, "Loaded object with vnum " + item.vnum + " and id " + item.id + " into auction " + auction_id);
    }
    
}