#!/bin/sh 
echo "**********<<<<<<<<<< HOST TEST START>>>>>>>>>>**********" 
dev=sda1
i=3
echo " connect any of the mass storage (pen drive to any one of the port)"
echo " and press enter "
while [ $i -lt 5 ];
do
read ans
if [ -d /run/media/$dev ]; then
	i=6
	echo "Found the mass storage device"
	echo " Listing the files in the Device "
	ls -lht /run/media/$dev/*
	sleep 1
	echo "Continuing to Write test"
	sleep 1
	if [ -f "/run/media/$dev/testfile" ];
	then
	rm -rf /run/media/$dev/testfile
	sync
	fi
	echo " writing 64 MBytes of data to the device"
	time dd if=/dev/urandom of=/run/media/$dev/testfile bs=1024k count=64
	sync
	echo " writing completed "
	echo " size of the file is "
	ls -lh /run/media/$dev/testfile
	echo " Done"
	sleep 1
#	echo "Removing the file from the storage device"
#	rm -rf /run/media/$dev/testfile
	sync
#	echo "done"
	sleep 2
	exit 0
fi
clear 
echo " cannot find the USB device "
echo " please connect and press enter"
done
exit 0
