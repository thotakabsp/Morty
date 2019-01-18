#!/bin/sh
./memtool 0x2184384 1
sleep 1
#./memtool 0x2184384=0x18461205  # For Full speed
#./memtool 0x2184384=0x18451205  # For High speed
./memtool 0x2184384=0x18441205
exit 0
i=3
while [ $i -lt 4 ];do
./memtool 0x2184384=0x18441205
j=0
while [ $j -lt 500 ]; do
j=$((j+1))
done
done
