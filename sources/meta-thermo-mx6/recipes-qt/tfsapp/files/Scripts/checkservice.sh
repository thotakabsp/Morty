#!/bin/sh
date_now=`date`
update_firmware="false"
avahi_status=`systemctl show -p SubState avahi-daemon.service | sed 's/SubState=//g'`
connman_status=`systemctl show -p SubState connman.service | sed 's/SubState=//g'`
wpa_status=`systemctl show -p SubState wpa_supplicant.service | sed 's/SubState=//g'`
restart_variscite="false"

if [ -f /etc/.minirfs ] && [ -f /etc/.recovery ];
then
	update_firmware="true"
fi

if [ "$update_firmware" = "false" ];
then
	if [ "$avahi_status" = "running" ];
	then
		echo "$date_now : Avahi service is running " > /run/media/mmcblk0p1/service
	else
		echo "$date_now : Avahi service is exited, Restarting  "  > /run/media/mmcblk0p1/service
		systemctl restart avahi.daemon
		restart_variscite="true"
		sleep 2
	fi
	if [ "$connman_status" = "running" ];
	then
		echo "$date_now : Connman service is running " >> /run/media/mmcblk0p1/service
	else
		echo "$date_now : Connman service is exited, Restarting  "  >> /run/media/mmcblk0p1/service
		systemctl restart connman.service
		sleep 2
		restart_variscite="true"
	fi

	if [ "$restart_variscite" = "true" ];
	then
		echo "$date_now : Restarting ThermoApp  "  >> /run/media/mmcblk0p1/service
		systemctl restart ThermoApp.service
		sleep 1
	fi
fi

