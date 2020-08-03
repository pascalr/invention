# zbar
sudo apt-get install libzbar-dev libzbar0

# opencv
# Download latest opencv from their website.
sudo apt-get install build-essential
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev

# cd ~/opencv
# mkdir build
# cd build
# cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
# make -j7
# sudo make install

# libqrencode
sudo apt install autoconf automake autotools-dev libtool pkg-config libpng12-dev
# https://fukuchi.org/works/qrencode/qrencode-4.0.2.tar.gz
./configure
make
sudo make install
sudo ldconfig

# barcode
sudo apt install barcode

# opencv OCR tesseract
# DOESNT WORK FOR c++ for me sudo apt install tesseract-ocr
# DOESNT WORK FOR c++ for me sudo apt install libtesseract-dev
# from source:
git clone https://github.com/tesseract-ocr/tesseract.git # follow INSTALL

# boost
sudo apt-get install libboost-all-dev

# matplotlib
sudo apt-get install python-matplotlib python-numpy python2.7-dev

# Simple Web Server (c++)
git clone https://gitlab.com/eidheim/Simple-Web-Server.git
sudo apt-get install libssl-dev libboost-filesystem-dev libboost-thread-dev

(cd .. && git clone https://gitlab.com/libeigen/eigen.git)

# install makeblock librairies for motor controller
https://github.com/Makeblock-official/Makeblock-Libraries

wget https://codeload.github.com/Makeblock-official/Makeblock-Libraries/zip/master

Arduino IDE: "Sketch-> Include Library-> Add .ZIP Library-> select the downloaded file-> Open"
MeDCMotor.h -> Remove include "MeConfig.h"
MeDCMotor.cpp -> Add #ifdef ME_PORT_DEFINED around the two reset functions



git clone https://github.com/SRombauts/SQLiteCpp et installer dans thirdparty, vient avec sqlite je crois

git submodule add https://github.com/SRombauts/SQLiteCpp.git thirdparty/SQLiteCpp
cd thirdparty/SQLiteCpp
git submodule init
git submodule update

sudo apt install libsqlite3-dev sqlite3


pkg-config --cflags --libs ruby-2.5
