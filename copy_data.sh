#!/bin/bash
scp pi@192.168.0.19:~/invention/client/db/development.sqlite3 client/db/development.sqlite3
scp pi@192.168.0.19:~/invention/client/storage/detected_codes/* client/storage/detected_codes/
cp client/storage/detected_codes/detected_* client/app/assets/images/client/storage/detected_codes/
