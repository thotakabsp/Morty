#!/bin/sh

source /home/root/Scripts/test.sh

STATUS=0
export TERM=linux
setterm -blank 0 > /dev/tty0
BKL_DEVICE=/sys/class/backlight/backlight
echo 7 > /sys/class/backlight/backlight/brightness
echo "**********<<<<<<<<<<LCD Luminescence test START>>>>>>>>>>**********"
check_devnode "/dev/fb0"
./lum-test
echo "**********<<<<<<<<<<LCD Luminescence test END>>>>>>>>>>**********"
sleep 1
exit $STATUS
