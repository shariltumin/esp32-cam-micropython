#!/usr/bin/env bash 

esptool.py \
    --chip esp32 \
    --port /dev/ttyUSB0 \
    --baud 460800 write_flash \
    -z 0x1000 firmware.bin

