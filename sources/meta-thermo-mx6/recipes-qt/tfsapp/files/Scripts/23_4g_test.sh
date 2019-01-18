#!/bin/sh  
killall connmand
killall ofonod
pack=0
INF=ACM0
stty -F /dev/tty$INF ignbrk -brkint -icrnl -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke
cat /dev/tty$INF &
echo -e "AT+CEREG=1\n" > /dev/tty$INF
sleep 3
killall cat
echo "**********<<<<<<<<<<DATA TEST START>>>>>>>>>>**********"
mval=0
testval=1
while [ $mval -eq 0 ];
do
if [ $testval -eq 1 ];
then
tval=0
while [ $tval -eq 0 ];
do
#./test
cat /dev/tty$INF &
echo -e "AT+CSQ\n" > /dev/tty$INF
sleep 1
killall cat
if [ $pack -eq 0 ];
then
rm -rf /etc/ppp
tar -xvzf ppp.tar.gz -C /etc/
rm -rf /etc/init.d/ppp
cp -r ppp1 /etc/init.d/ppp
sync
pack=1
fi
/etc/init.d/ppp start
i=3
var=`ip link show ppp0 | grep -c UP`
echo waiting for the connection 
while [ $var -eq '0' ];
do
        sleep 2
        var=`ip link show ppp0 | grep -c UP`
        i=$((i+1))
        if [ $i -eq 10 ];then
        var=2
        fi
done
if [ $var -eq '1' ];
then
        echo " connection established " 
        ifconfig ppp0
        echo "nameserver 8.8.8.8" > /etc/resolv.conf
        ping -c 5 google.com
else
        echo " unable to connect .. quitting"
fi
        /etc/init.d/ppp stop
        sleep 1
        killall pppd
echo "**********<<<<<<<<<<DATA TEST END>>>>>>>>>>**********"
j=1
while [ $j -lt 4 ];
do
echo " do you want to continue (y/n) ?"
read ans
if [ $ans == "n" ];
then
       exit 0 
else
	j=5
fi
done
done
else
echo "sleeping for 2 mins"
sleep 120
fi
done
sleep 1
exit 0
