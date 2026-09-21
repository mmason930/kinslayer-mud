#!/usr/bin/env python3
"""Repair the phpBB archive schema. Run in the game container; dry-run by default.

Back up phpbb_usersArchive before --apply. Existing rows are preserved. This tool
does not archive/delete accounts or modify permissions, sessions, or posts.
"""
import argparse
import importlib.util
from pathlib import Path

spec = importlib.util.spec_from_file_location("help_database", Path(__file__).with_name("import-command-help.py"))
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


def migrate(db, apply=False):
    tables = dict(db.run("SELECT TABLE_NAME,ENGINE FROM information_schema.TABLES "
                         "WHERE TABLE_SCHEMA=DATABASE() AND TABLE_NAME IN ('users','phpbb_users','phpbb_usersArchive')"))
    if not tables:
        print("No forum installed; skipped.")
        return
    if 'phpbb_users' not in tables and 'phpbb_usersArchive' not in tables:
        print("No forum installed; skipped.")
        return
    if len(tables) != 3:
        raise RuntimeError("Incomplete forum schema; refusing migration")
    source, archive = {}, {}
    for table, name, kind in db.run("SELECT TABLE_NAME,COLUMN_NAME,COLUMN_TYPE FROM information_schema.COLUMNS "
                                  "WHERE TABLE_SCHEMA=DATABASE() AND TABLE_NAME IN ('phpbb_users','phpbb_usersArchive')"):
        (source if table == 'phpbb_users' else archive)[name] = kind
    statements = []
    for name, kind in source.items():
        if name not in archive:
            if name not in ('user_actkey_expiration', 'user_last_active') or kind != 'int unsigned':
                raise RuntimeError("Unexpected missing archive column: " + name)
            statements.append(f"ALTER TABLE phpbb_usersArchive ADD COLUMN `{name}` INT UNSIGNED NOT NULL DEFAULT 0")
        elif archive[name] != kind:
            if name == 'user_id' and kind == 'int unsigned' and archive[name] == 'mediumint unsigned':
                extra = db.run("SELECT EXTRA FROM information_schema.COLUMNS WHERE TABLE_SCHEMA=DATABASE() "
                               "AND TABLE_NAME='phpbb_usersArchive' AND COLUMN_NAME='user_id'")[0][0]
                statements.append("ALTER TABLE phpbb_usersArchive MODIFY COLUMN user_id INT UNSIGNED NOT NULL" +
                                  (" AUTO_INCREMENT" if extra == 'auto_increment' else ""))
            elif name == 'user_password' and kind == 'varchar(255)' and archive[name] == 'varchar(40)':
                statements.append("ALTER TABLE phpbb_usersArchive MODIFY COLUMN user_password VARCHAR(255) NOT NULL DEFAULT ''")
            else:
                raise RuntimeError("Archive column type differs: " + name)
    if archive.keys() - source.keys():
        raise RuntimeError("Unexpected extra archive columns; manual review required")
    for table, engine in tables.items():
        if engine != 'InnoDB':
            statements.append(f"ALTER TABLE `{table}` ENGINE=InnoDB")
    before = db.run("SELECT COUNT(*) FROM phpbb_usersArchive")[0][0]
    for statement in statements:
        print(("Applying: " if apply else "Would apply: ") + statement)
        if apply:
            db.run(statement)
    if db.run("SELECT COUNT(*) FROM phpbb_usersArchive")[0][0] != before:
        raise RuntimeError("Archive row count changed during migration")
    print(f"Archive rows preserved: {before}; schema changes: {len(statements)}")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    migrate(module.Database(), args.apply)
