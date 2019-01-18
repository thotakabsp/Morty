#!/bin/sh 
echo "**********<<<<<<<<<< Temparature Sensor TEST START>>>>>>>>>>**********"

i=3
while [ $i -lt 10 ];
do 
in_temp_raw=$(cat /sys/bus/iio/devices/iio:device0/in_temp_raw || true)
in_temp_offset=$(cat /sys/bus/iio/devices/iio:device0/in_temp_offset || true)
in_temp_scale=$(cat /sys/bus/iio/devices/iio:device0/in_temp_scale || true)
  
if [ ! "x${in_temp_raw}" = "x" ] && [ ! "x${in_temp_offset}" = "x" ] && [ ! "x${in_temp_scale}" = "x" ] ; then
    temp=$(echo "${in_temp_raw} + ${in_temp_offset}" | bc)
    temp=$(echo "scale=1; ${temp} * ${in_temp_scale}" | bc)
    temp=$(echo "scale=1; ${temp} / 1000" | bc)
    echo "Current Temp is : ${temp} °C"
    temp=$(echo "scale=1; ${temp} * 9" | bc)
    temp=$(echo "scale=1; ${temp} / 5" | bc)
    temp=$(echo "scale=1; ${temp} + 32" | bc)
    echo "Current Temp is : ${temp} °F"
fi


in_humidityrelative_raw=$(cat /sys/bus/iio/devices/iio:device0/in_humidityrelative_raw || true)
in_humidityrelative_offset=$(cat /sys/bus/iio/devices/iio:device0/in_humidityrelative_offset || true)
in_humidityrelative_scale=$(cat /sys/bus/iio/devices/iio:device0/in_humidityrelative_scale || true)  
                                                                             
if [ ! "x${in_humidityrelative_raw}" = "x" ] && [ ! "x${in_humidityrelative_offset}" = "x" ] && [ ! "x${in_humidityrelative_scale}" = "x" ] ; then
    humidityrelative=$(echo "${in_humidityrelative_raw} + ${in_humidityrelative_offset}" | bc)                      
    humidityrelative=$(echo "scale=1; ${humidityrelative} * ${in_humidityrelative_scale}" | bc)                     
    humidityrelative=$(echo "scale=1; ${humidityrelative} / 1000" | bc)                                 
    echo "Current humidityrelative is : ${humidityrelative} %"                  
fi 

echo "do you want to continue (y/n): "
read ans
if [ $ans == "n" ];
then 
	i=10
fi

done
echo "**********<<<<<<<<<< Temparature Sensor TEST END>>>>>>>>>>**********"
sleep 1
exit 0
