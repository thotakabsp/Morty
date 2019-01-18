#!/bin/sh

echo "**********<<<<<<<<<<AUDIO TEST START>>>>>>>>>>**********"

cd /home/root/Scripts
./enaudio

amixer set PCM 127
cp asound.state /var/lib/alsa/asound.state
cp asound.conf /etc/asound.conf
sync
aplay /home/root/Scripts/ont5.wav 

echo " Volume Test ->> "

i=3
while [ $i -lt 5 ];
do 
	echo " enter the volume (1 ~ 10):"
	read vol
	./lmamp $vol
	aplay /home/root/Scripts/ont5.wav 
	echo "do you want to close the test (y/n)?"
	read ans
	if [ $ans == "y" ]
	then
		i=10
	fi
done

amixer set PCM 127
./lmamp 10 

echo "**********<<<<<<<<<<AUDIO  TEST  END>>>>>>>>>>**********"
sleep 1
exit 0
