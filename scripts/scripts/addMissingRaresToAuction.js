// Recycle unsold house stock and replenish the configured rare auction items.
// This job only handles auction stock; it does not purge unrelated rare items.
var addMissingRaresToAuction = function(self, actor, here, args, extra) {
    var vnums = [1611, 2406, 1335, 1334, 1511, 1121, 1416];
    var duration = 601200; // Preserve the existing six-day, 23-hour auctions.
    var now = time();
    var completed = 0;
    function log(message) { mudLog(2, 104, "Rare auction restock: " + message); }
    function query(sql) {
        var result = sqlQuery(sql);
        if (!result) throw new Error("database query failed; restock stopped");
        return result;
    }
    function rows(sql) {
        var result = query(sql), output = [];
        while (result.hasNextRow) output.push(result.getRow);
        return output;
    }
    function quote(value) { return sqlEscapeQuoteString(String(value)); }
    function validId(id) {
        return /^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i.test(id);
    }
    function owner(auctionId) { return auctionId === 1 ? 37000 : 37001; }

    // Scripts can be pulled before the corresponding game binary is rebuilt.
    // The old extract() destroys rare counts even when the item remains saved.
    if (!self || !self.isValid || typeof JSObject.prototype.storeAndExtract !== "function") {
        log("requires the game build with JSObject.storeAndExtract; no items changed");
        return;
    }

    try {
        var houses = rows("SELECT a.vnum FROM auctions a JOIN users u ON u.user_id=" +
            "CASE a.vnum WHEN 1 THEN 37000 WHEN 2 THEN 37001 END WHERE a.vnum IN (1,2)");
        if (houses.length !== 2) throw new Error("missing auction house or house owner");

        var live = new Map();
        for (var object of getObjectList()) {
            if (object && object.isValid) live.set(String(object.id), object);
        }

        var retryItems = getSval(self, 33245, "auctionRestockRetry");
        if (!Array.isArray(retryItems)) retryItems = [];
        function forgetRetry(id) {
            retryItems = retryItems.filter(function(entry) { return entry.id !== id; });
            setSval(self, 33245, "auctionRestockRetry", retryItems);
        }

        function activate(id, objectId, auctionId) {
            // A pending listing becomes public only after the complete item was saved.
            query("UPDATE auctionItem a JOIN objects o ON o.id=a.object_id " +
                "SET a.active=1,a.end_time=" + (now + duration) + ",a.timestamp=" + now +
                " WHERE a.id=" + id + " AND a.active=0 AND a.end_time=0 AND a.retrieved=0" +
                " AND o.holder_type='A' AND o.top_level_holder_type='A'" +
                " AND o.holder_id=" + quote(auctionId) + " AND o.top_level_holder_id=" + quote(auctionId));
            if (rows("SELECT id FROM auctionItem WHERE id=" + id + " AND active=1").length !== 1)
                throw new Error("could not activate listing " + id + " for " + objectId);
            forgetRetry(objectId);
            ++completed;
            log("listed object " + objectId + " in auction house " + auctionId);
        }

        function store(item, auctionId, pendingId) {
            var id = String(item.id);
            if (!validId(id)) throw new Error("invalid object UUID; item left on holding mob");
            if (!retryItems.some(function(entry) { return entry.id === id; })) {
                retryItems.push({id: id, auctionId: auctionId});
                setSval(self, 33245, "auctionRestockRetry", retryItems);
            }
            if (!pendingId) {
                // MyISAM has no rollback. Reserve an INACTIVE listing before moving
                // anything. end_time=0 marks our reservations; retrieved=1 means the
                // save has not been confirmed, so a reboot must not publish it.
                query("INSERT INTO auctionItem " +
                    "(auction_id,object_id,owner_id,end_time,starting_price,buyout_price,active,retrieved,timestamp) VALUES (" +
                    auctionId + "," + quote(id) + "," + owner(auctionId) + ",0," +
                    item.cost + "," + (item.cost * 2) + ",0,1," + now + ")");
                pendingId = Number(sqlInsertID());
                if (!(pendingId > 0)) throw new Error("missing reserved listing ID for " + id);
            }
            // Native persistence retains UUID, retools, flags and contents, and
            // unloads only on success without decrementing the world's item count.
            if (!item.storeAndExtract("A", String(auctionId)))
                throw new Error("save failed for " + id + "; item retained on holding mob, listing " + pendingId + " inactive");
            live.delete(id);
            query("UPDATE auctionItem SET retrieved=0 WHERE id=" + pendingId + " AND active=0 AND end_time=0");
            activate(pendingId, id, auctionId);
        }

        // Retry interrupted reservations first. An unconfirmed save with no live
        // item is left inactive for inspection rather than publishing partial data.
        var pending = rows("SELECT id,object_id,auction_id,retrieved FROM auctionItem " +
            "WHERE active=0 AND end_time=0 AND owner_id=CASE auction_id WHEN 1 THEN 37000 WHEN 2 THEN 37001 END");
        for (var reservation of pending) {
            var id = String(reservation.get("object_id"));
            var item = live.get(id);
            var auctionId = Number(reservation.get("auction_id"));
            if (item) {
                if (item.carriedBy === self && vnums.indexOf(item.vnum) !== -1)
                    store(item, auctionId, Number(reservation.get("id")));
            } else if (Number(reservation.get("retrieved")) === 0) {
                activate(Number(reservation.get("id")), id, auctionId);
            } else {
                log("unconfirmed reservation " + reservation.get("id") + " for " + id + " needs inspection; left inactive");
            }
        }

        // A failed reservation INSERT has no database row to retry. Remember
        // only items this job acquired, never the holding mob's unrelated gear.
        for (var retry of retryItems.slice()) {
            var item = live.get(retry.id);
            if (item && item.carriedBy === self && vnums.indexOf(item.vnum) !== -1)
                store(item, retry.auctionId);
            else
                forgetRetry(retry.id);
        }

        // Expired, unsold HOUSE-owned stock should circulate again. Never renew
        // a player listing, an active auction, or an item owed to a winning bidder.
        var unsold = rows("SELECT o.id,o.vnum,o.holder_id,MAX(a.id) AS auction_item_id FROM objects o " +
            "JOIN auctionItem a ON a.object_id=o.id AND a.auction_id=o.holder_id " +
            "AND a.owner_id=CASE a.auction_id WHEN 1 THEN 37000 WHEN 2 THEN 37001 END " +
            "WHERE o.vnum IN (" + vnums.join(",") + ") AND o.holder_type='A' AND o.top_level_holder_type='A' " +
            "AND o.holder_id=o.top_level_holder_id AND a.active=0 AND a.retrieved=0 AND a.end_time>0 AND a.end_time<=" + now +
            " AND NOT EXISTS (SELECT 1 FROM auctionBid b WHERE b.ai_id=a.id)" +
            " AND NOT EXISTS (SELECT 1 FROM auctionItem x WHERE x.object_id=o.id AND " +
            "(x.active=1 OR x.end_time=0 OR (x.retrieved=0 AND (x.owner_id NOT IN (37000,37001) OR " +
            "EXISTS (SELECT 1 FROM auctionBid b WHERE b.ai_id=x.id))))) " +
            "GROUP BY o.id,o.vnum,o.holder_id");
        for (var row of unsold) {
            var id = String(row.get("id"));
            if (!validId(id) || live.has(id)) continue;
            var listingId = Number(row.get("auction_item_id"));
            // One statement also retires duplicate old house listings for this
            // UUID. The saved object and its metadata never move or get recreated.
            query("UPDATE auctionItem SET active=IF(id=" + listingId + ",1,0)," +
                "retrieved=IF(id=" + listingId + ",0,1)," +
                "end_time=IF(id=" + listingId + "," + (now + duration) + ",end_time)," +
                "timestamp=IF(id=" + listingId + "," + now + ",timestamp) " +
                "WHERE object_id=" + quote(id) + " AND owner_id IN (37000,37001) AND active=0 AND retrieved=0 AND end_time>0");
            ++completed;
            log("renewed unsold house listing " + listingId + " for " + id);
        }

        // Only player/storage holders have player IDs. Auction and room IDs must
        // never accidentally join an inactive player with the same numeric ID.
        var reclaim = rows("SELECT o.id,o.vnum FROM objects o JOIN obj_protos p ON p.vnum=o.vnum " +
            "LEFT JOIN users u ON o.top_level_holder_type IN ('P','S') AND u.user_id=o.top_level_holder_id " +
            "WHERE o.vnum IN (" + vnums.join(",") + ") AND (p.extra_flags & (1<<15))<>0 " +
            "AND ((o.top_level_holder_type IN ('P','S') AND u.last_logon<=DATE_SUB(CURDATE(),INTERVAL 90 DAY)) " +
            "OR o.top_level_holder_type IN ('C','O')) " +
            "AND NOT EXISTS (SELECT 1 FROM auctionItem a WHERE a.object_id=o.id AND (a.active=1 OR a.retrieved=0 OR a.end_time=0))");
        for (var row of reclaim) {
            var id = String(row.get("id"));
            if (!validId(id) || live.has(id)) continue;
            var item = loadSingleObjectFromDatabase(id);
            if (!item || !item.isValid) {
                log("could not load " + id + "; saved item left untouched");
                continue;
            }
            item.moveToChar(self);
            live.set(id, item);
            store(item, random(1, 2));
        }

        // Mint at most one of each missing prototype, respecting BOTH persisted
        // items and the game's count (which also includes unsaved live objects).
        for (var vnum of vnums) {
            var proto = getObjProto(vnum);
            if (!proto || proto.max <= 0 || proto.count >= proto.max) continue;
            var count = rows("SELECT COUNT(*) AS count FROM objects WHERE vnum=" + vnum);
            if (Number(count[0].get("count")) >= proto.max) continue;
            var item = self.loadObj(vnum);
            if (!item || !item.isValid) {
                log("could not load prototype " + vnum);
                continue;
            }
            live.set(String(item.id), item);
            store(item, random(1, 2));
        }
        log("completed; " + completed + " listings opened");
    } catch (error) {
        log(String(error));
    }
};
