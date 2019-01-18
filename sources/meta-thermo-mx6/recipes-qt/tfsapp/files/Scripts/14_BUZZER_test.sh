#!/bin/sh
echo "**********<<<<<<<<<<BUZZER TEST START>>>>>>>>>>**********"
BUZ_INIT=/sys/class/leds/buzzinit
BUZ_DEV=/sys/class/backlight/buzzer
i=3
while [ $i -lt 10 ];
do 
	echo "1) Enable Buzzer"
	echo "2) Disable Buzzer"
	read ans
	case $ans in 
		1)
echo 1 > ${BUZ_INIT}/brightness 
			echo 0 > ${BUZ_DEV}/brightness
			;;
		2) 
echo 0 > ${BUZ_INIT}/brightness 
			echo 255 > ${BUZ_DEV}/brightness
			;;
		*)
			echo "please enter  correct value"
			;;
		esac
			echo " do you want to continue (y/n) ?"
			read ans
			if [ $ans == "n" ];
				then 
					i=10
			fi
done
echo 255 > ${BUZ_DEV}/brightness
echo "**********<<<<<<<<<<BUZZER TEST END>>>>>>>>>>**********"
sleep 1
exit 0
