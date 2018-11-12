#!/usr/bin/env bash
make -C /lib/modules/$(uname -r)/build M=$PWD modules
sudo rmmod pilote_usb.ko
sudo insmod pilote_usb.ko