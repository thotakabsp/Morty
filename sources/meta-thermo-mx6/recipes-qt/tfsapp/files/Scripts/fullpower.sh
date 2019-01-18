#!/bin/sh
i=1
echo " Switching on the Non primary cores "
while [ $i -lt 4 ]; do
if [ -d /sys/devices/system/cpu/cpu$i ];
then
echo 1 > /sys/devices/system/cpu/cpu$i/online
fi
i=$((i+1))
done
echo performance > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo " Enabling Ethernet Module"
ifconfig eth0 up 
echo " Enabling USB Host controller Module "
echo -n ci_hdrc.1 > /sys/bus/platform/drivers/ci_hdrc/bind
echo " Enabling Wi-fi & Bluetooth "
modprobe wlcore_sdio
hciconfig hci0 up
