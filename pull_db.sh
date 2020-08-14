#!/bin/bash
./backup_db.sh
scp 192.168.0.10:/home/pascalr/invention/client/db/development.sqlite3 /home/pi/invention/client/db/development.sqlite3
