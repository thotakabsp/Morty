#!/bin/sh
if [ ! -d /sys/class/gpio/gpio144 ];
then
echo 144 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio144/direction
fi
if [ ! -d /sys/class/gpio/gpio164 ];
then
echo 164 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio164/direction
fi
j=3
while [ $j -lt 5 ];
do
echo 1 > /sys/class/gpio/gpio144/value 
echo 1 > /sys/class/gpio/gpio164/value 
sleep 1
echo 0 > /sys/class/gpio/gpio144/value 
echo 0 > /sys/class/gpio/gpio164/value 
sleep 1
done
