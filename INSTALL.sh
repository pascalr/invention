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
