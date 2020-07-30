#!/bin/bash
rm -f data/test.db
sqlite3 data/test.db '.read src/setup_db.sql'
