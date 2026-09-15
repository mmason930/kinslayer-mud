#!/usr/bin/env python3
"""Insert missing help keywords using the game's DB_* environment and libmysqlclient.

Dry-run by default. Pass --apply to insert; --add-minimum-level permits the
existing minimum-level migration on an older schema. Never updates existing pages.
Run inside the game container. JSON reports contain inserted rows and undo SQL.
"""
import argparse
import ctypes as c
import ctypes.util
import hashlib
import json
import os
import re
from pathlib import Path


def keywords(value):
    return [word for word in re.split(r",\s*", (value or "").lower()) if word]


def literal(value):
    return "0x" + value.encode("ascii").hex() if value else "''"


class Database:
    def __init__(self):
        self.lib = c.CDLL(ctypes.util.find_library("mysqlclient"))
        def bind(name, result, *args):
            fn = getattr(self.lib, name)
            fn.restype, fn.argtypes = result, list(args)
            return fn
        p, s = c.c_void_p, c.c_char_p
        self.init = bind("mysql_init", p, p)
        self.connect = bind("mysql_real_connect", p, p, s, s, s, s, c.c_uint, s, c.c_ulong)
        self.query = bind("mysql_query", c.c_int, p, s)
        self.result = bind("mysql_store_result", p, p)
        self.fields = bind("mysql_num_fields", c.c_uint, p)
        self.fetch = bind("mysql_fetch_row", c.POINTER(s), p)
        self.free = bind("mysql_free_result", None, p)
        self.error = bind("mysql_error", s, p)
        self.insert_id = bind("mysql_insert_id", c.c_ulonglong, p)
        self.close = bind("mysql_close", None, p)
        self.handle = self.init(None)
        env = lambda name: os.environ[name].encode()
        if not self.connect(self.handle, env("DB_HOSTNAME"), env("DB_USERNAME"),
                            env("DB_PASSWORD"), env("DB_SCHEMA"), 3306, None, 0):
            raise RuntimeError("Database connection failed")
        self.run("SET NAMES utf8mb4")

    def run(self, sql):
        if self.query(self.handle, sql.encode()):
            raise RuntimeError(self.error(self.handle).decode())
        result = self.result(self.handle)
        if not result:
            return []
        rows, count = [], self.fields(result)
        try:
            while True:
                row = self.fetch(result)
                if not row:
                    return rows
                rows.append([row[i].decode("utf-8", "strict") if row[i] is not None else None
                             for i in range(count)])
        finally:
            self.free(result)

    def snapshot(self):
        columns = [row[0] for row in self.run("SHOW COLUMNS FROM helpFile")]
        return [dict(zip(columns, row)) for row in self.run("SELECT * FROM helpFile ORDER BY id")]


def plan(entries, rows):
    indexed = {}
    for row in rows:
        for keyword in keywords(row["keywords"]):
            indexed.setdefault(keyword, []).append(row)
    pending, skipped = [], []
    for entry in entries:
        matches = {row["id"]: row for key in keywords(entry["keywords"])
                   for row in indexed.get(key, [])}
        if matches:
            if entry["command"] not in indexed:
                raise ValueError("Alias conflict: " + entry["command"])
            skipped.append({"command": entry["command"], "existingIds": sorted(matches)})
            continue
        if any(row["name"].lower() == entry["name"].lower() for row in rows):
            raise ValueError("Title exists without matching keyword: " + entry["name"])
        parents = [row for row in rows if row["name"] == entry["parentName"]]
        if len(parents) != 1:
            raise ValueError("Ambiguous/missing parent: " + entry["parentName"])
        pending.append((entry, int(parents[0]["id"])))
    return pending, skipped


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path)
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--add-minimum-level", action="store_true")
    args = parser.parse_args()
    payload = json.loads(args.input.read_text()) if args.input else globals()["EMBEDDED_PAYLOAD"]
    assert payload["formatVersion"] == 1
    entries = payload["entries"]
    seen = set()
    for entry in entries:
        for key, length in [("name", 60), ("syntax", 120), ("keywords", 120)]:
            assert 0 < len(entry[key]) <= length
        assert isinstance(entry["minimumLevel"], int) and 0 <= entry["minimumLevel"] <= 105
        assert entry["parentName"] == ("Immortals" if entry["minimumLevel"] >= 100 else "Commands")
        assert entry["description"].endswith("})();")
        entry["description"].encode("ascii")
        assert entry["command"] in keywords(entry["keywords"])
        for key in keywords(entry["keywords"]):
            assert key not in seen, key
            seen.add(key)
    db = Database()
    inserted, locked = [], False
    try:
        before = db.snapshot()
        pending, skipped = plan(entries, before)
        has_level = "minimum_level" in before[0]
        report = {"mode": "apply" if args.apply else "dry-run", "beforeCount": len(before),
                  "schemaNeedsMinimumLevel": not has_level, "plannedInserts": len(pending),
                  "skipped": skipped, "commands": [e["command"] for e, parent in pending]}
        if not args.apply:
            print(json.dumps(report))
            return
        if not has_level:
            if not args.add_minimum_level:
                raise ValueError("Apply the minimum-level migration first, or pass --add-minimum-level")
            db.run("ALTER TABLE helpFile ADD COLUMN minimum_level TINYINT UNSIGNED NOT NULL DEFAULT 0")
        # MyISAM does not roll back transactions: serialize the check and insert,
        # and compensate only this import's tracked rows if validation fails.
        db.run("LOCK TABLES helpFile WRITE")
        locked = True
        before = db.snapshot()
        pending, skipped = plan(entries, before)
        report.update(beforeCount=len(before), plannedInserts=len(pending), skipped=skipped)
        for entry, parent in pending:
            values = [literal(entry[k]) for k in ("name", "syntax", "description", "keywords")]
            values += [str(parent), str(entry["minimumLevel"]), "1", "NOW()", "1", "NOW()"]
            db.run("INSERT INTO helpFile (name,syntax,description,keywords,parent_id,minimum_level,"
                   "created_by_user_id,created_datetime,last_modified_by_user_id,last_modified_datetime) "
                   "VALUES (" + ",".join(values) + ")")
            inserted.append({"id": int(db.insert_id(db.handle)), "command": entry["command"],
                             "descriptionSha256": hashlib.sha256(entry["description"].encode()).hexdigest()})
        after = db.snapshot()
        byid = {int(row["id"]): row for row in after}
        assert len(after) == len(before) + len(inserted)
        for old in before:
            assert byid[int(old["id"])] == old, "Existing article changed"
        for item, (entry, parent) in zip(inserted, pending):
            row = byid[item["id"]]
            for key in ("name", "syntax", "description", "keywords"):
                assert row[key] == entry[key], (entry["command"], key)
            assert int(row["minimum_level"]) == entry["minimumLevel"]
            assert int(row["parent_id"]) == parent
        assert not plan(entries, after)[0], "Not all commands are covered"
        report.update(afterCount=len(after), inserted=inserted, verified=True)
        report["undoSql"] = ["DELETE FROM helpFile WHERE id=" + str(item["id"]) +
                             " AND SHA2(description,256)='" + item["descriptionSha256"] + "';"
                             for item in inserted]
        db.run("UNLOCK TABLES")
        locked = False
        print(json.dumps(report))
    except BaseException:
        if locked and inserted:
            db.run("DELETE FROM helpFile WHERE id IN (" + ",".join(str(i["id"]) for i in inserted) + ")")
        raise
    finally:
        if locked:
            db.run("UNLOCK TABLES")
        db.close(db.handle)


if __name__ == "__main__":
    main()
