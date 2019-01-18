#!/bin/sh
echo "############## WatchDog Test START ############"
i=1
while [ $i -lt 5 ];
do
echo "1) WatchDog Tickle Enable"
echo "2) WatchDog Tickle Disable"
echo "Please input your answer "
read ans
case $ans in
	1)
	   /home/root/Scripts/dTest &
		;;
	2)
	   killall dTest
	   echo 1 > /dev/tfswatchdog
	   ;;
	*)
	   ;;
esac
echo " Do you want to continue the Test (y/n) ?"
read an
if [ $an == 'n' ];
then
        i=10
fi
done
echo "############## WatchDog Test END ############"
