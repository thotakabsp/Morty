#!/bin/sh
echo "<<<<<<<<<<<<<<< Soft switch test script >>>>>>>>>>>>>"
if [ ! -d "/sys/class/gpio/gpio146" ];
then
echo 146 > /sys/class/gpio/export     
fi

j=1
while [ $j -lt 60 ];
do
v=$(cat /sys/class/gpio/gpio146/value)
if [ $v -lt 1 ]; then
echo " soft switch is pressed "
else
echo " soft switch is not pressed"
fi
j=$((j+1))
sleep 1
done
echo "<<<<<<<<<<<<<<< Soft switch test script >>>>>>>>>>>>>"
