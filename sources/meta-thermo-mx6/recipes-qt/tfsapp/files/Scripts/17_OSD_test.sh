#!/bin/sh

source /home/root/Scripts/test.sh

STATUS=0
export TERM=linux
setterm -blank 0 > /dev/tty0
echo 7 > /sys/class/backlight/backlight/brightness
echo "**********<<<<<<<<<<LCD TEST START>>>>>>>>>>**********"
# devnode test
check_devnode "/dev/fb0"
# Blank test
echo FB Blank test
echo -n 3 > /sys/class/graphics/fb0/blank
echo Screen should be off
sleep 3
echo -n 0 > /sys/class/graphics/fb0/blank
echo Screen should be on 
# Pan test
#
echo FB panning test
sleep 2
xres=$(fbset | awk '/geometry/ {print $2}')
yres=$(fbset | awk '/geometry/ {print $3}')
yvirt=$(expr $yres '*' 2)
echo $xres,$yvirt > /sys/class/graphics/fb0/virtual_size
if ! grep -sq $xres,$yvirt /sys/class/graphics/fb0/virtual_size;
then
	echo FAIL - Unable to set virtual size
	STATUS=1
fi

for i in $(seq 1 50); do
	echo This is line $i. > /dev/tty0
done
for i in $(seq 1 $yres); do
	echo 0,$i > /sys/class/graphics/fb0/pan
	if ! grep -sq $i /sys/class/graphics/fb0/pan;
	then
		echo FAIL - Unable to pan
		STATUS=1
	fi
done

print_status
setterm -clear all > /dev/tty0
sleep 1

./fb-test
echo LCD Brighness test 
cat /home/root/Scripts/wall-1024x768-565.rgb > /dev/fb0

BKL_DEVICE=/sys/class/backlight/backlight
for var in 7 6 5 4 3 2 1 0
do
echo $var > ${BKL_DEVICE}/brightness
sleep 2
done
for var in 0 1 2 3 4 5 6 7
do
echo $var > ${BKL_DEVICE}/brightness
sleep 2
done

echo "LCD Direction Test"
echo 147 > /sys/class/gpio/export
echo 148 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio147/direction
echo out > /sys/class/gpio/gpio148/direction
k=0
while [ $k -lt 2 ]; do
i=0
while [ $i -lt 2 ];
do
j=0
        while [ $j -lt 2 ];
        do
                echo $j > /sys/class/gpio/gpio147/value
                j=$((j+1))
                sleep 1
        done
                echo $i > /sys/class/gpio/gpio148/value
                i=$((i+1))
                sleep 1
done
        k=$((k+1))
done

echo 0 > /sys/class/gpio/gpio147/value
echo 1 > /sys/class/gpio/gpio148/value
echo 147 > /sys/class/gpio/unexport
echo 148 > /sys/class/gpio/unexport


i=3                              
while [ $i -lt 5 ];              
do                               
        echo " enter the brightness value ( 0 ~ 7 ):"
        read var                           
	echo $var > ${BKL_DEVICE}/brightness
        echo "do you want to close the test (y/n)?"
        read ans                                   
        if [ $ans == "y" ]                         
        then                                       
                i=10                               
        fi                                         
done                    
echo 7 > ${BKL_DEVICE}/brightness

echo "**********<<<<<<<<<<LCD TEST END>>>>>>>>>>**********"
sleep 1
exit $STATUS
