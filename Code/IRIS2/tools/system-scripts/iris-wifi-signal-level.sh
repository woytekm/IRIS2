#!/bin/ash

WIRELESS_INTERFACE="wlan0"
LEVEL_FILE="/tmp/wifi.signal.level"

/sbin/iwconfig $WIRELESS_INTERFACE | grep "Signal level" | awk '{printf $4;}' | awk 'BEGIN{FS="=";}{printf $2;}' | awk '{ \
     if($1~/\//) \
     {gotit=1; split($1,a,"/"); lev=(a[1]/25)+1; if(a[1]==0)lev=0; if(a[1]==100)lev=4; print(lev);} \
     else if($1~/-/) \
     {gotit=1; if($1>-50) print "4"; else if($1>-60) print "3"; else if ($1>=-70) print "2"; else if($1<-70) print "1"; else print 0;} else print "0";}END{if(!gotit) print "0";}' > $LEVEL_FILE


