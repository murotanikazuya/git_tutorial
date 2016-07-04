#!/bin/sh

rmmod e1000e
rmmod igb
insmod /lib/modules/$(uname -r)/extra/atemsys.ko
mknod /dev/atemsys c 101 0
dmesg
