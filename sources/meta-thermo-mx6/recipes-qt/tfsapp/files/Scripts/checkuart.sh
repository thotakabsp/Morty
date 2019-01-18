#!/bin/sh
echo "disabling UART"
/home/root/Scripts/memtool 0x20c407c=0x1
sleep 5
/home/root/Scripts/memtool 0x20c8010=0x80010000
sleep 30
/home/root/Scripts/memtool 0x20c8010=0x80003000
sleep 2
/home/root/Scripts/memtool 0x20c407c=0xf000001
echo "uart is back ON"
