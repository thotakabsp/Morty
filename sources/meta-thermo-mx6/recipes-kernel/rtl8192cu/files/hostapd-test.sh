#!/bin/bash
count=0
Var1=$1
Var2=$2
Vas3=$3
hostapdPID=$(ps | grep hostapd | grep -v grep | awk '{print $1}')
wpaPID=$(ps | grep wpa_supplicant | grep -v grep | awk '{print $1}')

if [ "$#" != 3 ]; then
    	echo "Illegal number of parameters"
	echo "please give arguments <channel> <hw_mode> <country-code> "
else
	echo "in else case"
	if [ $count == 0 ]
	then
		count=1
		kill -9 $wpaPID
		sleep 1
		ifconfig wlan0 down
	else
		kill -9 $hostapdPID
		ifconfig wlan0 down
	fi
	echo "2.in else case"

	if [ "$1" ]                                                   
	then                                                          
   		echo "selected channel number is = $1"                     
   		sed -i 's/^channel.*/channel='$1'/' /home/root/hostapd.conf
	else                                                          
  		echo "Please Enter the channel number 1 to 11"              
	fi                                                                       
                                                                         
	if [ "$2" ]                                                              
	then                                                                     
   		echo "selected configured mode = $2"                                  
   		sed -i 's/^hw_mode.*/hw_mode='$2'/' /home/root/hostapd.conf           
	else                                                                     
  		echo "Please Enter the mode"                                           
	fi                                                                       
                                                                         
	if [ "$3" ]                                                              
	then                                                                     
    		sed -i 's/^country_code.*/country_code='$3'/' /home/root/hostapd.conf
	else                                   
   		echo "Please Enter the country code"
	fi             
	
	ifconfig wlan0 up
	sleep 2
	hostapd /home/root/hostapd.conf -B
fi
