jos
===

jos task from lab4

The source code is from MIT public class and modified until lab4 is finished. The source can be compiled under CentOs x86-64 
and run against bochs and real PC.

For bochs: disk configuration should be configured if the Makefile doesn't touch:

ata0-master: type=disk, mode=flat, translation=auto, path="F:\boches\startup.img", cylinders=4, heads=16, spt=63, biosdetect=auto, model="Generic 1234"

For real PC, it can startup on lenovo thinkpad T510 by make usb flash disk as startup disk.
