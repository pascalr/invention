#!/bin/bash
scp client/db/development.sqlite3 pi@192.168.0.18:/home/pi/backup.development.sqlite3
scp client/db/development.sqlite3 pi@192.168.0.19:/home/pi/invention/client/db/development.sqlite3
