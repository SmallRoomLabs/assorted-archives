# apple-1-replica
Emulator / Arduino Mega 2560 Apple 1 Replica

Please note only my stuff - source code - is licensed under the MIT license.

- POM1 emulator is by anarkavre
- Apple 1 firmware is owned by ... Apple ? Woz ? 
- and so on....

Emulates 4k (easy change) Apple 1 Replica.

- Windows/SDL version 
	Input - Keyboard
	Output - stdout or simulated 40x25 terminal.
	Memory - as much as you want :)

- Arduino Mega 2560 versions 
	Input - serial or PS/2 Keyboard
	Output - serial or ST7920 GLCD (32 x 10 chars)
	Memory - 
Coming soon:
	
1) 	Fix the cursor speed bug
2) 	Add cassette input functionality (maybe output ...)
3) 	Driver for Arduino LCD 20x4 LCD display (or other sizes)
4) 	Find an SPI/I2C display that will show 40x25

Paul Robson 2015.