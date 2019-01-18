#!/bin/sh
i=1                                                         
echo " Switching off the Non primary cores "
while [ $i -lt 4 ]; do
if [ -d /sys/devices/system/cpu/cpu$i ]; 
then
echo 0 > /sys/devices/system/cpu/cpu$i/online
fi
i=$((i+1))
done
echo " Disabling Bluetooth Module"
hciconfig hci0 down
echo " Disabling Ethernet Module"
ifconfig eth0 down
echo " Disabling Wi-Fi Module"
ifconfig wlan0 down
rmmod wl18xx wlcore mac80211 cfg80211 mxc_v4l2_capture 
rmmod wlcore_sdio compat
echo " Disabling USB Host controller Module "
echo -n ci_hdrc.1 > /sys/bus/platform/drivers/ci_hdrc/unbind
echo userspace > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo 396000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
