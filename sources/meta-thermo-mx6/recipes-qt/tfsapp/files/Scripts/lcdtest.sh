#!/bin/sh
i=0
while [ $i -lt 2 ]; do
echo 1 > /sys/class/graphics/fb$i/blank
i=$((i+1))
done
sleep 5
i=0
while [ $i -lt 2 ]; do
echo 0 > /sys/class/graphics/fb$i/blank
i=$((i+1))
done
