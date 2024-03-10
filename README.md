# armCapture
motion capture for realtime or recorded motion for robot arms


coded for atmega 328p

To write the hex file, using a board that has an arduino bootloader:


windows:
 avrdude -c arduino -P com99 -b 115200 -p m328p -e -U flash:w:main.hex
 
 
linux: 
  avrdude -c arduino -P /dev/ttyUSB99 -b 115200 -p m328p -e -U flash:w:main.hex
  

Remember to change the serial port to the one your arduino is on.
