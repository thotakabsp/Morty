#!/bin/sh
echo "**********<<<<<<<<<< ETH TEST START>>>>>>>>>>**********"
myip=192.168.1.29
ifconfig eth0 $myip up
link_up_down()
{
i=0
while [ $i -lt 2 ];
do
        ifconfig eth0 down
        sleep 1
        ifconfig eth0 $myip up
        sleep 5
        i=$((i+1))
done
}

ping_test()
{
echo "enter ip address to test"
read ip
echo "nameserver 8.8.8.8" >> /etc/resolv.conf
 ping -c 5 $ip
}

band_test()
{
        echo "band test"
        echo "enter server ip address"
        read ip
        echo "enter port number"
        read port
        iperf -c $ip -t 30 -i 2 -p $port
}

var=0
while [ $var -lt 5 ];
do
        echo "1) link up down "
        echo "2) ping test"
        echo "3) bandwith test"
        echo "7) quit"
        read var
        case $var in
        1) link_up_down
                ;;
        2) ping_test
                ;;
        3) band_test
                ;;
        *)
                ;;
        esac
done
ifconfig eth0 $myip up
echo "**********<<<<<<<<<< ETH TEST END>>>>>>>>>>**********"
