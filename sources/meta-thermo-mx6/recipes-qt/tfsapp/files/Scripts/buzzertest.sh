#!/bin/sh
echo "<<<<<<<<<<<<<<<< Buzzer Power Test >>>>>>>><<START>>>>>>>>>>>"
i=3
while [ $i -lt 4 ];do
echo " Enabling Buzzer "
echo 1 > /sys/class/leds/buzzinit/brightness
echo 5 > /sys/class/backlight/buzzer/brightness
date
sleep 5
echo " Disabling Buzzer"
echo 0 > /sys/class/leds/buzzinit/brightness
sleep 5
done
echo "<<<<<<<<<<<<<<<< Buzzer Power Test >>>>>>>><<END>>>>>>>>>>>"
