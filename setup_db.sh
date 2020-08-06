#!/bin/bash
rm -f data/test.db
rm -f data/unit_tests.db
sqlite3 data/test.db '.read src/setup_db.sql'
sqlite3 data/unit_tests.db '.read src/setup_db.sql'
