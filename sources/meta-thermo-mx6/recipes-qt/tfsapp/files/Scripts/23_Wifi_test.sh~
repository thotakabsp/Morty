#!/bin/sh 
echo "**********<<<<<<<<<< Wi-fi TEST START>>>>>>>>>>**********"
killall wpa_supplicant udhcpc
ifconfig eth0 down
ip addr flush dev eth0
val=`ls -l /sys/class/net/* | grep -c wlan0`
while [ $val -eq '0' ];
do
echo "wlan0 interface is not available"
        sleep 1
        val=`ls -l /dev/tty* | grep -c USB0`
done

echo "wlan0 interface is available"
iw dev wlan0 disconnect
sleep 1
ifconfig wlan0 down
sleep 1 
ip addr flush dev wlan0
ifconfig wlan0 up
sleep 1
#iw dev wlan0 link
echo " please enter the Accesspoint name" 
read ap
echo " please enter the password"
read pass
wpa_passphrase $ap $pass > /etc/wpa_supplicant.conf
echo "ap_scan=1" >> /etc/wpa_supplicant.conf
echo "update_config=1" >> /etc/wpa_supplicant.conf
wpa_supplicant -B -iwlan0 -c /etc/wpa_supplicant.conf
i=0
route del default
status=`iw wlan0 link | grep signal`
while [ "$status" == "" ]
do 
	sleep 1
	status=`iw wlan0 link | grep signal`
done
udhcpc -q -i wlan0 
sleep 5
while [ $i -lt 5 ];
do
wifiip=$(ip addr | awk '/inet/ && /wlan0/{sub(/\/.*$/,"",$2); print $2}')
if [ $wifiip != '' ]; 
then 
	i=5
fi
	sleep 1
	i=$((i+1))
done
echo "wi-fi address is $wifiip "
iw dev wlan0 link

if [ $wifiip != '' ];
then
        echo " connection established " 
        ifconfig eth0 down
        echo "nameserver 8.8.8.8" > /etc/resolv.conf
	sleep 1
	echo "Please input the ping address"
	for i in 1 2 3
	do
		read IP
		ping -c 5 $IP
		if [ $? -eq 0 ]
		then 
			echo "PASS:Wi-Fi Test Success"
			break
		else
			echo "Please enter correct address"
		fi
	done
else
        echo " unable to connect .. quitting"
fi
        sleep 1
        ifconfig eth0 up
iw dev wlan0 disconnect
sleep 1
ifconfig wlan0 down
sleep 1 
ip addr flush dev wlan0
echo "**********<<<<<<<<<<Wi-fi TEST END>>>>>>>>>>**********"
exit 0
