#!/bin/ash

if [ "$1" = "" ]
 then
   SPEAKER_ADDR=`/usr/sbin/nvram get bt_speaker`
   if [ "$SPEAKER_ADDR" = "" ]
    then
     echo "No bt_speaker specified in nvram! I don't know the speaker address!"
     exit 0
    fi
 else
  SPEAKER_ADDR=$1
 fi


IS_CONNECTED=`/usr/local/bin/bt-device -i $SPEAKER_ADDR | grep "Connected" | awk '{print $2}'`

if [ "$IS_CONNECTED" = "1" ]
 then
  touch /tmp/bt_speaker.connected
 else
  rm -f /tmp/bt_speaker.connected
 fi


