stm32f4-discovery-example is a very simple example of how to program
an stm32f4 discovery board.

In short:

Use "make" to build the firmware.

Use "make flash" to push the firmware to the stm32f4-discovery.

Use GNU screen to test the stm32f4-discovery's firmware.


====================================================================

The project is a slightly modified version of the USB CDC example.

The original example just echoed back whatever it received, so
to be more convincing that something is happening, a little code was
added to swap each letter with the next letter in the alphabet and
flash the LEDs when data comes in.

To build the example:

 * Install the arm developer tools (and add to PATH) with
   summon-arm-toolchain from
   https://github.com/esden/summon-arm-toolchain

 * Build stlink from https://github.com/texane/stlink

 * put the st-flash command from stlink somewhere in your path
   (e.g. by creating a soft link)

 * run "make"


To flash the example to the discovery board:

 * Connect the usb-mini cable to the board and your computer
   (not the usb-micro)

 * run "make flash"
   If you get a permissions error, either run it as root or chmod the device
   it's complaining about to give yourself write access

 * Press the reset button on the discovery board


To test the example:

 * It may be useful to unplug other USB devies like phones or Arduinos
   so you don't have to figure out which ttyACM* device to use

 * Connect the discovery board to the computer through the usb-micro
   connector (in addition to the usb-mini connection)

 * "dmesg" and "ls -l /dev/ttyACM*" to determine device name

 * run "screen -l /dev/ttyACM0" (or whatever teh device name is)

 * Type some things, and confirm that the LEDs near the reset switch on
   the discovery board toggle as you type keys.  Also, confirm that the
   letters you see on the screen are thirteen letters further in the
   alphabet from what you typed. (An easy test of "rot13" is simply typing
   again the output and it should be transformed back to what you typed
   in the first place.)

 * Press 'Ctrl-A' '\' and then 'y' to close the screen session when
   you're done.

Cheers!

