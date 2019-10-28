$ cd micropython_src/

$ mkdir MP26102019

$ cd MP26102019

$ pwd
/home/sharil/micropython_src/MP26102019

$ git clone https://github.com/espressif/esp-idf.git

$ git clone https://github.com/micropython/micropython

$ ls 
esp-idf  micropython

$ cat micropython/ports/esp32/Makefile | grep ESPIDF_SUPHASH | head -1
ESPIDF_SUPHASH_V3 := 6ccb4cf5b7d1fdddb8c2492f9cbc926abaf230df

$ export PATH="/home/sharil/micropython/xtensa-esp32-elf/bin:/home/sharil/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

$ export ESPIDF=/home/sharil/micropython_src/MP26102019/esp-idf

$ cd $ESPIDF

$ git checkout 6ccb4cf5b7d1fdddb8c2492f9cbc926abaf230df

$ git submodule update --init --recursive

$ cd components

$ git clone https://github.com/espressif/esp32-camera.git

$ cd ../../micropython/

$ git submodule update --init --recursive

$ make -C mpy-cross

$ cd ports

$ cp -r esp32 esp32-cam

$ cd esp32-cam

$ export MPTD=/home/sharil/Downloads/esp32-cam-1-11-498

$ cp $MPTD/main.c .
$ cp $MPTD/Makefile .
$ cp $MPTD/modcamera.c .
$ cp $MPTD/mpconfigport.h .
$ cp $MPTD/boards/manifest.py ./boards/manifest.py
$ cp $MPTD/boards/sdkconfig.base ./boards/sdkconfig.base

$ make clean

$ make
