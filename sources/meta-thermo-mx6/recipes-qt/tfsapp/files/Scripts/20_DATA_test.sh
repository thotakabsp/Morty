#!/bin/sh 

echo "**********<<<<<<<<<<DATA TEST START>>>>>>>>>>**********"

#echo 63 > /sys/class/gpio/export
#val=`cat /sys/class/gpio/gpio63/value`
#while [ $val -eq '0' ];
#do
#	val=`cat /sys/class/gpio/gpio63/value`
#	echo "sim card is not connected" 
#	sleep 2
#	echo 63 > /sys/class/gpio/unexport           
#	exit 0 
#done
rm -rf /etc/ppp
tar -xvzf ppp.tar.gz -C /etc/ 
rm -rf /etc/init.d/ppp
cp -r ppp1 /etc/init.d/ppp
sync
echo 1 > /sys/class/leds/gsminit/brightness 

val=`ls -l /dev/tty* | grep -c USB0`
while [ $val -eq '0' ];
do 
#	echo $val
	sleep 1
	val=`ls -l /dev/tty* | grep -c USB0`
done
/etc/init.d/ppp start

i=3 

var=`ip link show ppp0 | grep -c UP`
echo waiting for the connection 
while [ $var -eq '0' ];
do 
#	echo $var
	sleep 1	
	var=`ip link show ppp0 | grep -c UP`
done

if [ $var -eq '1' ]; 
then
	echo " connection established " 
	ifconfig ppp0
	ifconfig eth0 down
	ifconfig wlan0 down
	echo "nameserver 8.8.8.8" > /etc/resolv.conf
	ping -c 5 google.co.in  
else
	echo " unable to connect .. quitting"
fi
	/etc/init.d/ppp stop
	sleep 1
	killall pppd	
	echo 0 > /sys/class/leds/gsminit/brightness 
	echo 63 > /sys/class/gpio/unexport
	ifconfig eth0 up
	ifconfig wlan0 up	

echo "**********<<<<<<<<<<DATA TEST END>>>>>>>>>>**********"
sleep 1
exit 0
