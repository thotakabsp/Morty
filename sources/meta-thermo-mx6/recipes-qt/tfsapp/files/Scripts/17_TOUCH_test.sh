#!/bin/sh
echo "**********<<<<<<<<<<TOUCH TEST START>>>>>>>>>>**********"
tar -xvzf /home/root/Scripts/tslib.tar.gz -C /
sync
export TSLIB_TSDEVICE=/dev/input/event0 
export TSLIB_CALIBFILE=/etc/pointercal
export TSLIB_PLUGINDIR=/lib/ts
export TSLIB_FBDEVICE=/dev/fb0
export TSLIB_CONFFILE=/etc/ts.conf
if [ ! -f "/etc/pointercal" ];
then
	touch /etc/pointercal
	sync
fi
ts_calibrate
sleep 1
ts_test_mt
echo "**********<<<<<<<<<<TOUCH TEST END>>>>>>>>>>**********"
sleep 1
echo "**********<<<<<<<<<< SOFT SWITCH TEST >>>>>>>>>>**********"
i=5
while [ $i -lt 10 ];
do
val=`cat /sys/class/gpio/gpio146/value`       
if [ $val -eq '0' ];                      
then
	echo " SOFT SWITCH NOT PRESSED"	
else
	echo "SOFT SWITCH PRESSED"
fi
       sleep 1                             
done              

exit $STATUS
