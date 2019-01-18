#!/bin/sh
echo "<<<<<<<<<<<<<<<<<<<<< RTC TEST START >>>>>>>>>>>>>>>>>>>>>"
i=3
while [ $i -lt 5 ];
do
echo "1) Read date and time"
echo "2) set date and time"
#echo "3) alarm test"
#echo "4) Generic test"
read ans
case $ans in 
	1)
		date;;
	2)
		echo "set date and time in <mm><dd><hh><mm><yyyy>.<sec>"
		read val
		date $val
		hwclock --systohc
		;;
	3)
		echo "setting wake alarm for 30 secs"
		echo "System will go to stand by mode and wake up after 10 secs"
		echo +10 > /sys/class/rtc/rtc0/wakealarm
		echo mem > /sys/power/state	
		sleep 2
		echo " Test done"
		;;
	4)
		./rtc-test;; 
	*)
		echo "please choose correct option"
		;;
esac
	sleep 1
#	clear
	echo "Do you want to continue (y/n)"
	read rep
	if [ $rep == "n" ];
	then
		exit 0
	fi
done
echo "<<<<<<<<<<<<<<<<<<<<< RTC TEST END >>>>>>>>>>>>>>>>>>>>>"


