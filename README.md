# glitch-target
A simple target for glitching experiments.

Created to give a simple target for attempting to learn about glitch attacks, using simple tools like the [ChipShouter PicoEMP])(https://github.com/newaetech/chipshouter-picoemp).

This code is intended for the Raspberry Pi Pico.  It simulates a microcontroller boot program which is checking the CRC of the secure code that it will run.  If the CRC fails, such code might fall back to a less secure bootloader or similar.  There is no simulation of the "application" or "bootloader", the code merely cycles round and runs again.  This makes it easy to scan over the PCB with your glitching probe and experiment with finding sensitive areas.

It provides feedback when a glitch is successful by sounding a passive piezo transducer.  The code makes the following sounds:
* A short blip at startup (400Hz for 1ms)
* A longer, higher pitched, bleep if the main CRC-checking loop is exited and the "insecure bootloader" is entered (1600Hz for 0.5s)
* 10 lower pitched beeps if the "insecure bootloader" is skipped completely.

From these you can establish what results your experiments are having whilst keeping your eyes on the PCB.

The normal serial port also offers written feedback at 115200baud.

 