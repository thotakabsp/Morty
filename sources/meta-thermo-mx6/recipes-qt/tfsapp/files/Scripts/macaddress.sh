#!/bin/bash
D='/sys/class/net'
if [ "$#" -ne 1 ]; then
echo "Invalid number of params, eg. sh macaddress.sh wlan0"
exit 1
fi

if !([ $1 = "wlan0" ] || [ $1 = "wlan1" ] || [ $1 = "eth0" ]);then
	echo "Invalid parameter, please pass either wlan0, wlan1, or eth0"
else
	mac=$( cat $D/$1/address )
	echo "The macaddress for $1 is: $mac"
fi
