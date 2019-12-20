#!/bin/bash

cd workspace

git clone https://github.com/sisoputnfrba/hilolay
git clone https://github.com/sisoputnfrba/linuse-tests-programs
git clone https://github.com/sisoputnfrba/so-commons-library
git clone https://gist.github.com/mesaglio/8b5902a3f2b4a5630926ab5261a490b7

cd

cd workspace/so-commons-library
make
sudo make install

cd 

cd workspace/hilolay
make
sudo make install

cd 

cd workspace/tp-2019-2c-SuperandO
./script_para_shared

cd

cd workspace/linuse-tests-programs
sudo make 

sudo make entrega