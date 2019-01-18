#!/bin/sh
echo "**********<<<<<<<<<< SD TEST START>>>>>>>>>>**********"
parname=mmcblk1p1
echo "Connect the SD Card to the device"
while [ ! -d /run/media/$parname ];
do 
	sleep 1
done
echo "SD card is connected to the device "

echo " Showing the contents of the SD card"
sleep 1
ls -l /run/media/$parname
echo " Do you want to continue the Write test (y/n)"
read ans
echo " Writing the temp file of 256 MBytes to the SD card "

dd if=/dev/urandom of=/run/media/$parname/testfile bs=1024k count=256

echo " Written successfully "

echo " Do you want to continue the read test (y/n)"
read ans
echo " Reading from the SD Card "

dd if=/run/media/$parname/testfile of=/dev/null 

echo " Read Successfully "
rm -rf /run/media/$parname/testfile
sync

echo "**********<<<<<<<<<< SD TEST END>>>>>>>>>>**********"
