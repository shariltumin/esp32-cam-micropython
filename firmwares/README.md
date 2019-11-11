# Micropython firmwares that support esp32-cam.

**LATEST** - The latest firmware is under 1-11-20191107 MicroPython v1.11-571-g7e374d231.

See [esp32-cam-1-11-571](https://github.com/shariltumin/esp32-cam-micropython/tree/master/esp32-cam-1-11-571) for files needed to compiled the firmware.

See [esp32-cam-1-11-498](https://github.com/shariltumin/esp32-cam-micropython/tree/master/esp32-cam-1-11-498) on how you can compile and build your own firmware. The firmware was compiled without help, webrepl and upip modules.

To flash the firmware.bin follow the steps below:
1. connect GPIO0 to GND
2. press reset or pull out/ push in USB serial adapter
3. run erase.sh, to erase flash memory
4. press reset or pull out/ push in USB serial adapter
5. run flash.sh to upload the firmware.bin to flash memory
6. disconnect GPIO0 from GND

You find erase.sh and flash.sh under [tools](https://github.com/shariltumin/esp32-cam-micropython/tree/master/firmwares/tools).
You may need to install esptool.py

To test (I run this on linux, you may need to change the port)

```
rshell -n -p /dev/ttyUSB0
...
/home/st> repl
...
>>> import camera
>>> dir(camera)
['__class__', '__name__', 'capture', 'deinit', 'flip', 'init']
>>> camera.init()
True
>>> img = camera.capture()
>>> len(img)
84145
>>> print(img[0:8])
b'\xff\xd8\xff\xe0\x00\x10JF'
>>> camera.deinit()
True
```
You now have camera enabled micropython. Nice!

Read [my blog](https://kopimojo.blogspot.com/).
