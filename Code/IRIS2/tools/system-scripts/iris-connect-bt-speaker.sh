#!/bin/ash

if [ ! -f /tmp/bt_speaker.tried ]
 then
  touch /tmp/bt_speaker.tried
  RETRY=0
 else 
  RETRY=1
 fi

if [ "$1" = "" ]
 then
   SPEAKER_ADDR=`/usr/sbin/nvram get bt_speaker`
   if [ "$SPEAKER_ADDR" = "" ]
    then
     echo "No bt_speaker specified in nvram! I don't know BT speaker address!"
     exit 0
    fi
 else
  SPEAKER_ADDR=$1
 fi

if [ "$RETRY" = "1" ]
 then
  /usr/local/bin/bt-device -d $SPEAKER_ADDR
  /usr/local/bin/bt-device -r $SPEAKER_ADDR
 fi

sleep 1

if [ "$RETRY" = "0" ]
 then
  /usr/local/bin/bt-adapter -d
  sleep 1
 fi

/usr/local/bin/bt-device -c $SPEAKER_ADDR
usleep 700000
/usr/local/bin/bt-audio -c $SPEAKER_ADDR
usleep 700000
/usr/local/bin/bt-device --set $SPEAKER_ADDR Trusted 1

IS_CONNECTED=`/usr/local/bin/bt-device -i $SPEAKER_ADDR | grep Connected | awk '{print $2;}'`

if [ "$IS_CONNECTED" = "1" ]
 then
   echo "BT speaker pairing/connecting done and successful. Switching default ALSA output to BT."
   /usr/sbin/iris-generate-a2dp-cfg.sh $SPEAKER_ADDR  > /usr/local/share/alsa/alsa.conf.d/bluetooth.conf
   exit 1
 else
   echo "BT speaker pairing/connecting failed."
   exit 0
 fi
