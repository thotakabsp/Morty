#!/bin/sh

echo "**********<<<<<<<<<<RG color TEST START>>>>>>>>>>**********"
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
echo " Enter the Value ( 0 ~ 255 ) for R :"
read red
echo " Enter the Value ( 0 ~ 255 ) for G :"
read green

cd lp5521_pri:channel0
echo $red > brightness
cd ..
cd lp5521_pri:channel1
echo $green > brightness
cd ..

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
echo "**********<<<<<<<<<<RG color TEST END>>>>>>>>>>**********"
cd /home/root
sleep 1
exit 0
