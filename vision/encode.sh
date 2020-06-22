#!/bin/bash
#barcode -u mm -p A4 -b 012345 -b 099999 -t 4x8 -m 6,6 -e upc-e -o test.ps
barcode -u mm -b 012345 -b 099999 -t 4x8 -m 6,6 -e upc-e -o test.ps
echo "\nquit" | gs -sDEVICE=pnggray -sOutputFile=test2.png -r144 test.ps > /dev/null
rm test.ps
