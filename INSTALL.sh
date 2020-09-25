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

# leptonica for tesseract (either sudo apt install libleptonica-dev, or install from source
https://github.com/DanBloomberg/leptonica.git


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
(cd .. && git clone https://gitlab.com/eidheim/Simple-Web-Server.git)
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

install ruby with rvm
sudo apt-get install zlib1g-dev libssl-dev liblzma-dev
\curl -sSL https://get.rvm.io | bash -s stable --ruby
rvm list known



Download ruby and compile from source
https://www.ruby-lang.org/en/downloads/

# sudo apt install ruby-dev not usefulll probably since installing from source

install yarn for rails
curl -sS https://dl.yarnpkg.com/debian/pubkey.gpg | sudo apt-key add -
echo "deb https://dl.yarnpkg.com/debian/ stable main" | sudo tee /etc/apt/sources.list.d/yarn.list
sudo apt update && sudo apt install yarn


sudo gem install rails
(cd client && bundle install)


sudo apt install tilda
# windows + click + move pour déplacer la fenêtre
# F1 pour faire apparaître ou disparaître.
# F11 pour fullscreen
