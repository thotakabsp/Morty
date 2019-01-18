#!/bin/sh -x
if [ ! -d /sys/class/gpio/gpio142 ];
then
echo 142 > /sys/class/gpio/export
fi
j=3
while [ $j -lt 5 ];
do
v=$(cat /sys/class/gpio/gpio142/value)
if [ $v -lt 1 ]; then
echo " voltage is under 14 V "
echo heartbeat > /sys/class/leds/diag2/trigger
echo 1 > /sys/class/leds/diag2/brightness
sleep 3
echo " stopping the services"
init 1
sleep 2
cd /sys/class/leds/sombkpen
echo 1 > brightness
j=6
else
echo " voltage is above 14 V "
echo 1 > /sys/class/leds/diag2/brightness
fi
sleep 1
done

