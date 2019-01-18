#!/bin/sh
hciconfig hci0 down
ifconfig wlan0 down
rmmod wl18xx wlcore mac80211 cfg80211 mxc_v4l2_capture 
rmmod wlcore_sdio compat
sleep 5
lsmod 
modprobe wlcore_sdio 
sleep 3 
ifconfig wlan0 up
hciconfig hci0 up
