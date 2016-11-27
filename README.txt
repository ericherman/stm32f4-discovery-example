stm32f4-discovery-example is a very simple example of how to program
an stm32f4 discovery board.


License
-------

Licensed under the terms of the GNU Lesser General Public License
(LGPL), version 3 or later.

See COPYING.GPL3 and COPYING.LGPL3 for details.



In short:
--------
Choose an example: "cd rotate-chars"

Use "make" to build the firmware.

Use "make flash" to push the firmware to the stm32f4-discovery.

Use GNU screen to test the stm32f4-discovery's firmware.


====================================================================

The project started as Kendrick Shaw's slightly modified version of
the USB CDC example.

The original example just echoed back whatever it received, so
to be more convincing that something is happening, a little code was
added to swap each letter with the another letter in the alphabet and
flash the LEDs when data comes in.

Next an example using SPI to read data from the on-board accelerometer
was added. Everytime a character is sent, the accelerometer's X, Y,
and Z values are returned.

To build an example:

 * Install the libopencm3 libraries, for example
   git clone https://github.com/libopencm3/libopencm3.git
   git clone https://github.com/libopencm3/libopencm3-examples.git

 * Install the arm developer tools (and add to PATH) with
   debian:
   sudo apt-get install \
	gcc-arm-none-eabi \
	gdb-arm-none-eabi \
	binutils-arm-none-eabi

 * Build stlink from https://github.com/texane/stlink
 * put the st-flash command from stlink somewhere in your path
   (e.g. by creating a soft link)
 * install the udev rules from stlink as described in the stlink README

 * cd to an example directory, for instance "cd rotate-chars"
 * if needed, adjust the path of the LIBOPENCM3_DIR variable in the Makefile
 * run "make" in the example directory


To flash the example to the discovery board:

 * Connect the usb-mini cable to the board and your computer
   (not the usb-micro)

 * run "make flash"
   If you get a permissions error, either run it as root or chmod the device
   it's complaining about to give yourself write access

 * Press the reset button on the discovery board


To test an example:

 * It may be useful to unplug other USB devies like phones or Arduinos
   so you don't have to figure out which ttyACM* device to use

 * Connect the discovery board to the computer through the usb-micro
   connector (in addition to the usb-mini connection)

 * "dmesg" and "ls -l /dev/ttyACM*" to determine device name

 * run "screen -l /dev/ttyACM0" (or whatever the device name is)

 * Type some things, and confirm that the LEDs near the reset switch on
   the discovery board toggle as you type keys.

 * If running the "rotate-chars" example, you may confirm that the
   letters you see on the screen are thirteen letters further in the
   alphabet from what you typed. (An easy test of "rot13" is simply typing
   again the output and it should be transformed back to what you typed
   in the first place.)

 * Press 'Ctrl-A' 'k' and then 'y' to close the screen session when
   you're done.

Cheers!

