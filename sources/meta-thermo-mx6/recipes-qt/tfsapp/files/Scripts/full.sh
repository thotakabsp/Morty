#!/bin/sh
echo 7 > /sys/class/backlight/backlight/brightness
echo " Enabling the Frame Buffer Module"                   
i=0                                                         
while [ $i -lt 1 ]; do                                                
echo 0 > /sys/class/graphics/fb$i/blank                               
i=$((i+1))                                                            
done            
i=1
echo " Switching on the Non primary cores "
while [ $i -lt 4 ]; do
echo 1 > /sys/devices/system/cpu/cpu$i/online
i=$((i+1))
done

echo " Enabling Ethernet Module"
ifconfig eth0 up 
echo " Enabling USB Host controller Module "
echo -n ci_hdrc.1 > /sys/bus/platform/drivers/ci_hdrc/bind
echo " Setting Primary CPU to low speed mode(396 MHz) "
echo userspace > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo 996000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
sleep 2
