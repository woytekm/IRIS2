#!/bin/ash

SPEAKER_MAC=`/usr/local/bin/bt-adapter -d | awk '{if($1~/^\[.*/){mac=substr($1,2,17);} if($2~/audio-card/) { if(mac~/^([0-9A-F]{2}[:-]){5}([0-9A-F]{2})$/){print mac; exit;} } }'`

if [ "$SPEAKER_MAC" = "" ]
 then
  echo "No BT speaker found."
  exit 0
 else
  /usr/sbin/nvram set bt_speaker=$SPEAKER_MAC
  /usr/sbin/nvram commit
  echo "BT speaker successfully detected and saved in nvram"
  exit 1
 fi

