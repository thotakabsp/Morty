#!/bin/sh
echo "********* Ethernet Module Enable/Disable Test Script **************"
i=1
while [ $i -lt 5 ];
do
echo "1) Ethernet enable"
echo "2) Ethernet Disable"
echo "5) Exit Test"
read i
if [ $i -eq 1 ];
then
ifconfig eth0 up
sleep 1
fi
if [ $i -eq 2 ];
then
ifconfig eth0 down
sleep 1
fi
ifconfig
done
echo "********* Ethernet Module Enable/Disable Test Script **************"

