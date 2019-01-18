#!/bin/sh

echo "**********<<<<<<<<<<RG Health Indicator TEST START>>>>>>>>>>**********"
cd /sys/class/leds
for val in 0 1 2
do
cd lp5521_pri:channel$val
echo none > trigger
echo  0 > brightness
cd ..
done
i=3
while [ $i -lt 4 ];
do
echo " Which LED to Glow (0-Red, 1-Green) : "
read led
if [ $led -eq 0 ];
then
cd lp5521_pri:channel0
echo " Duty Cycle value (0~255) : "
read cycle
echo $cycle > brightness
cd ..
fi
if [ $led -eq 1 ];                           
then                                         
cd lp5521_pri:channel1                       
echo " Duty Cycle value (0~255) : "          
read cycle                         
echo $cycle > brightness           
cd ..                              
fi                  

echo " do you want to continue (y/n) ?"
read ans
if [ $ans == "n" ];
then 
        i=10
fi
cd /sys/class/leds
for val in 0 1 2
do
cd lp5521_pri:channel$val
echo none > trigger
echo  0 > brightness
cd ..
done
done

echo "**********<<<<<<<<<<RG Health Indicator TEST END>>>>>>>>>>**********"

cd /home/root
sleep 1
exit 0
