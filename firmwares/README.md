# Micropython firmwares that support esp32-cam.
We have two micropython firmware cloned and compiled:
* 1-10 (https://github.com/tsaarni/micropython-with-esp32-cam) MicroPython v1.10-128-g584bc5b2a
* 1-11 (https://github.com/Lennyz1988/micropython) MicroPython v1.11-209-g6b54be556-dirty

Both support camare module. To flash the firmware.bin follow the steps below:
1. connect GPIO0 to GND
2. press reset or pull out/ push in USB serial adapter
3. run erase.sh, to erase flash memory
4. press reset or pull out/ push in USB serial adapter
5. run flash.sh to upload the firmware.bin to flash memory
6. disconnect GPIO0 from GND

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
