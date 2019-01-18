#!/bin/sh
echo " Setting the Screen brightness to low "
echo 0 > /sys/class/backlight/backlight/brightness
sleep 5
echo " Setting the Screen brightness to high "
echo 7 > /sys/class/backlight/backlight/brightness
