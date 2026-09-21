#!/usr/bin/env python3
"""Exercise only synthetic tables in a temporary, uniquely named local database."""
import importlib.util
import os
from pathlib import Path
import subprocess
import tempfile
import uuid

root = Path(__file__).resolve().parent.parent
spec = importlib.util.spec_from_file_location('migration', root / 'tools/migrate-forum-archive.py')
migration = importlib.util.module_from_spec(spec)
spec.loader.exec_module(migration)
db = migration.module.Database()
schema = 'kinslayer_test_forum_' + uuid.uuid4().hex
db.run(f'CREATE DATABASE `{schema}`')
try:
    db.run(f'USE `{schema}`')
    db.run('CREATE TABLE users (user_id INT PRIMARY KEY) ENGINE=MyISAM')
    db.run("CREATE TABLE phpbb_users (user_id INT UNSIGNED PRIMARY KEY, user_type INT NOT NULL, "
           "user_password VARCHAR(255) NOT NULL DEFAULT '', user_actkey_expiration INT UNSIGNED NOT NULL DEFAULT 0, "
           "user_last_active INT UNSIGNED NOT NULL DEFAULT 0) ENGINE=InnoDB")
    db.run("CREATE TABLE phpbb_usersArchive (user_id MEDIUMINT UNSIGNED PRIMARY KEY, user_type INT NOT NULL, "
           "user_password VARCHAR(40) NOT NULL DEFAULT '') ENGINE=InnoDB")
    migration.migrate(db, apply=True)
    migration.migrate(db, apply=True)
    # The address migration preserves legacy MyISAM rows and column defaults.
    spec = importlib.util.spec_from_file_location('addresses', root / 'tools/migrate-gateway-addresses.py')
    addresses = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(addresses)
    db.run("CREATE TABLE userLogin (host VARCHAR(32) NOT NULL DEFAULT '') ENGINE=MyISAM DEFAULT CHARSET=latin1")
    db.run("CREATE TABLE switchExemption (from_host VARCHAR(32) NOT NULL, to_host VARCHAR(32) NOT NULL) ENGINE=MyISAM DEFAULT CHARSET=latin1")
    db.run("INSERT INTO userLogin VALUES ('192.0.2.1')")
    db.run("INSERT INTO switchExemption VALUES ('192.0.2.1','192.0.2.2')")
    addresses.migrate(db, apply=True)
    addresses.migrate(db, apply=True)
    full_address = '2001:db8:ffff:ffff:ffff:ffff:ffff:ffff'
    db.run(f"INSERT INTO userLogin VALUES ('{full_address}')")
    assert db.run("SELECT host FROM userLogin WHERE host='192.0.2.1'") == [['192.0.2.1']]
    assert db.run(f"SELECT host FROM userLogin WHERE host='{full_address}'") == [[full_address]]
    with tempfile.TemporaryDirectory(prefix='kinslayer-forum-test-') as directory:
        binary = str(Path(directory) / 'forum-archive')
        subprocess.run(['g++', '-std=c++23', '-O1', '-g', '-I' + str(root / 'src'),
                        str(root / 'tests/forum-archive.cpp'), '-lsqlDatabase', '-lmysqlclient', '-o', binary], check=True)
        subprocess.run([binary], env=dict(os.environ, DB_SCHEMA=schema), check=True, timeout=30)
finally:
    db.run(f'DROP DATABASE `{schema}`')
