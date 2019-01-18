#!/bin/sh
echo "how many cpu cores to be shutdown?"
echo "enter (1 to 3) and press enter"
read val
i=0
while [ $i -lt $val ]; do
i=$((i+1))
echo 0 > /sys/devices/system/cpu/cpu$i/online
done
cat /proc/cpuinfo | grep processor 
#echo userspace > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
#echo 396000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
sleep 5
i=1
#echo ondemand > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo " waking up all the processors "
while [ $i -lt 4 ]; do
echo 1 > /sys/devices/system/cpu/cpu$i/online
#echo ondemand > /sys/devices/system/cpu/cpu$i/cpufreq/scaling_governor
i=$((i+1))
done
#echo 996000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
cat /proc/cpuinfo | grep processor
