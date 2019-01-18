#!/bin/sh
i=1
echo " Switching off the Non primary cores "
while [ $i -lt 4 ]; do
echo 0 > /sys/devices/system/cpu/cpu$i/online
i=$((i+1))
done
sleep 2
echo " Disabling Bluetooth Module"
hciconfig hci0 down
sleep 1
echo " Disabling Ethernet Module"
sleep 1
ifconfig eth0 down
echo " Disabling Wi-Fi Module"
ifconfig wlan0 down
rmmod wl18xx wlcore mac80211 cfg80211 mxc_v4l2_capture 
rmmod wlcore_sdio compat
sleep 2
echo " Disabling USB Host controller Module "
echo -n ci_hdrc.1 > /sys/bus/platform/drivers/ci_hdrc/unbind
sleep 2
echo " Setting Primary CPU to low speed mode(396 MHz) "
echo userspace > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo 396000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
sleep 2
echo " Disabling the Frame Buffer Module"
i=0
while [ $i -lt 2 ]; do
echo 1 > /sys/class/graphics/fb$i/blank
i=$((i+1))
done
sleep 2
echo " Disabling the Buzzer Module"
i=3
while [ $i -lt 4 ];do
echo 1 > /sys/class/leds/buzzinit/brightness
echo 5 > /sys/class/backlight/buzzer/brightness
date
sleep 1
echo 0 > /sys/class/leds/buzzinit/brightness
sleep 5
done
