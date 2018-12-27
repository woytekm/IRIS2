#!/bin/ash

PRODUCT=`cat /etc/product`

killall wpa_supplicant
killall udhcpc

ifconfig wlan0 down

nvram cache

WIFI_TYPE=`nvram get wifi_type`
WIFI_SSID=`nvram get wifi_ssid`
WIFI_PSK=`nvram get wifi_password`

/usr/sbin/generate_wpasupconf.sh $WIFI_TYPE $WIFI_SSID $WIFI_PSK > /etc/wpa_supplicant.conf

sleep 2

ifconfig wlan0 up

/usr/sbin/wpa_supplicant -i wlan0 -c /etc/wpa_supplicant.conf -P /var/run/wpa_supplicant.wlan0.pid -D nl80211,wext -B -s 2
sleep 2
udhcpc -i wlan0 -b -S -a -T 2 -V $PRODUCT -x hostname $HOSTNAME -O timezone -O ntpsrv -p /var/run/udhcpc.wlan0.pid


