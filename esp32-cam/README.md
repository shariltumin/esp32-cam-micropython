# Workflow on how I compiled micropython with camera module on Linux.

Change to home directory

`$ cd`

Create working directory

`$ mkdir Esp32Cam`

Go to working directory

`$ cd Esp32Cam`

Get micropython

`$ git clone https://github.com/micropython/micropython.git`

Get espressif esp-idf

`$ git clone https://github.com/espressif/esp-idf.git`

Find out which esp-idf version we need to compile micropython

`$ cat micropython/ports/esp32/Makefile | grep ESPIDF_SUPHASH | head -1`

`ESPIDF_SUPHASH := 6b3da6b1882f3b72e904cc90be67e9c4e3f369a9`

Prepare esp-idf

`$ cd esp-idf`

Check out the version of esp-idf this micropython need (example) from above

`$ git checkout 6b3da6b1882f3b72e904cc90be67e9c4e3f369a9`

Get all submodule

`$ git submodule update --init --recursive`

Get the camera component of esp-idf

`$ cd components`

from espressif esp32-camera. Note down the name of cloned directory.

`$ git clone https://github.com/espressif/esp32-camera.git`

Now prepare micropython. Change working directory to micropython clone

`$ cd ~/Esp32Cam/micropython`

Get all needed submodule

`$ git submodule update --init --recursive`

Setup PATH variable with xtensa-esp32-elf tool (example)

`$ export PATH="/home/st/micropython/xtensa-esp32-elf/bin:/home/st/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"`

Setup ESPIDF (example)

`$ export ESPIDF=/home/st/Esp32Cam/esp-idf`

Make mpy-cross needs for frozen python module

$` make -C mpy-cross`

Make esp32-cam port

`$ cd ports`

Copy from esp32 port

`$ cp -r esp32 esp32-cam`

Workspace ready under esp32-cam. Do modification

`$ cd esp32-cam`

Copy all files needed which will build micropython with camera module assumming these files are under ~/Downloads/esp32-cam

$ `cp ~/Downloads/esp32-cam/mpconfigport.h mpconfigport.h`

`$ cp ~/Downloads/esp32-cam/modcamera.c modcamera.c`

`$ cp ~/Downloads/esp32-cam/main.c main.c`

`$ cp ~/Downloads/esp32-cam/Makefile Makefile`

`$ cp ~/Downloads/esp32-cam/boards/sdkconfig.base boards/sdkconfig.base`

Edit modcamera.c pixel_format and jpeg_quality

frame_size = FRAMESIZE_XGA # good enough for streaming

frame_size = FRAMESIZE_UXGA # good still photo

`$ vi modcamera.c`

Reduce the firmware size by deleting modules under modules (example)

`$ rm modules/webrepl.py`

`$ rm modules/webrepl_setup.py`

`$ rm modules/websocket_helper.py`

`$ rm modules/upip.py`

`$ rm modules/upip_utarfile.py`

Clean up

`$ make clean`

Build micropython with camera support

`$ make V=1`

When done see what micropython we got (example)

`$ strings build-GENERIC/firmware.bin | grep MicroPython | tail -1`

```
MicroPython v1.11-292-g5641aa55d-dirty on 2019-09-11; ESP32 module with ESP32
```

Do we have camera? (example)

`$ strings build-GENERIC/firmware.bin | grep camera`

```
camera
[0;31mE (%d) %s: Detected camera not supported.
/home/st/Esp32Cam/esp-idf/components/esp32-camera-for-micropython/driver/camera.c
camera_xclk
```

Nice, we did it, celebrate have a cup of coffe with cream and read [my blog](https://kopimojo.blogspot.com/).

**LATEST** The latest compiled firmware is under [1-11-20190915](https://github.com/shariltumin/esp32-cam-micropython/firmwares/1-11-20190915)

