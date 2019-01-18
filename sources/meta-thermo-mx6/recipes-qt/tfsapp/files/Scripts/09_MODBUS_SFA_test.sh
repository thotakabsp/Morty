#!/bin/sh

echo "**********<<<<<<<<<< MODBUS TEST START>>>>>>>>>>**********"
cd /home/root/Scripts
./unit-test-server rtu &
./unit-test-client rtu 2&>clientlog 
echo "**********<<<<<<<<<< MODBUS TEST START>>>>>>>>>>**********"
