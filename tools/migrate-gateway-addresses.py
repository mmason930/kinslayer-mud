#!/usr/bin/env python3
"""Widen game IP-address columns for IPv6; dry-run unless --apply is specified.

Back up the affected tables and stop the game before applying: legacy MyISAM
tables can require a copy/lock during ALTER. No account or login rows are removed.
"""
import argparse
import importlib.util
from pathlib import Path
import re

spec = importlib.util.spec_from_file_location("help_database", Path(__file__).with_name("import-command-help.py"))
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


def migrate(db, apply=False):
    changes = []
    for table, column in [('userLogin', 'host'), ('switchExemption', 'from_host'), ('switchExemption', 'to_host')]:
        rows = db.run("SELECT CHARACTER_MAXIMUM_LENGTH,IS_NULLABLE,COLUMN_DEFAULT,CHARACTER_SET_NAME,COLLATION_NAME,EXTRA,COLUMN_COMMENT,DATA_TYPE "
                      f"FROM information_schema.COLUMNS WHERE TABLE_SCHEMA=DATABASE() AND TABLE_NAME='{table}' AND COLUMN_NAME='{column}'")
        if len(rows) != 1:
            raise RuntimeError('Missing address column: ' + table + '.' + column)
        length, nullable, default, charset, collation, extra, comment, kind = rows[0]
        if kind != 'varchar':
            raise RuntimeError('Unexpected address column type')
        if int(length) >= 45:
            continue
        if int(length) != 32 or default not in (None, '') or extra or comment or not all(re.fullmatch(r'[a-zA-Z0-9_]+', v or '') for v in (charset, collation)):
            raise RuntimeError('Unexpected address column definition; manual review required')
        definition = f"VARCHAR(45) CHARACTER SET {charset} COLLATE {collation} " + ('NULL' if nullable == 'YES' else 'NOT NULL')
        if default == '':
            definition += " DEFAULT ''"
        elif nullable == 'YES':
            definition += ' DEFAULT NULL'
        changes.append(f"ALTER TABLE `{table}` MODIFY COLUMN `{column}` {definition}")
    counts = {table: db.run(f'SELECT COUNT(*) FROM `{table}`')[0][0] for table in ('userLogin', 'switchExemption')}
    for statement in changes:
        print(('Applying: ' if apply else 'Would apply: ') + statement)
        if apply:
            db.run(statement)
    for table, count in counts.items():
        if db.run(f'SELECT COUNT(*) FROM `{table}`')[0][0] != count:
            raise RuntimeError('Address table row count changed during migration: ' + table)
        print(f'{table} rows preserved: {count}')
    print(f'Address schema changes: {len(changes)}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    migrate(module.Database(), args.apply)
