sudo modprobe i2c-dev

echo "* assign GPIO's..."
sudo gpio mode 25 out # faulty MUTE (1 = MUTE)
sudo gpio mode 26 out # fault/I2C addr  (pin 32)
sudo gpio mode 27 out # reset           (pin 36)
sudo gpio mode 28 out # power down      (pin 38)

echo "* try resetting the TAS..."
sudo gpio write 25 1 # set mute to off
sleep 0.01
sudo gpio write 26 1  # set I2C addr to 0x36
sleep 0.01
sudo gpio write 27 1 # set reset to high
sleep 0.01
sudo gpio write 28 0 # power down
sleep 0.01
sudo gpio write 28 1 # power up
sleep 0.01
sudo gpio write 27 0 # toggle reset (1)
sleep 0.01
sudo gpio write 27 1 # toggle reset (2)

printf "* search for TAS on I2C..."

TAS_ID=`sudo i2cget -y 1 0x1b 0x00`

if [ "$TAS_ID" == "0x6c" ]
 then
  printf " found TAS5721 on I2C!"
 else
  print " TAS5721 not found on I2C. Exit..."
  exit
 fi

echo "* setting oscillator factory trim..."

i2cset -y 1 0x1b 0x1b 0x00

echo "* setting I2S word length to 16 bit (for RPi mpc5102 driver)"

i2cset -y 1 0x1b 0x04 0x03

echo "* setting sub-channel mixer to 2.1 mode..."

i2cset -y 1 0x1b 0x21 0x00 0x00 0x42 0x03 i

echo "* exiting shutdown mode..."

i2cset -y 1 0x1b 0x05 0x04 # a nie 0x84  ?

echo "* setting gain to 0 db..."

i2cset -y 1 0x1b 0x07 0x5a

echo "* all done."

