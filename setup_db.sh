#!/bin/bash
rm -f data/test.db
bin/sqlite_shell data/test.db '.read src/setup_db.sql'
