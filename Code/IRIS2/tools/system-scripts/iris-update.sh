#!/bin/ash
#
# $1 - firmware file
#

STAMP=`date +%s`
UPGRADE_DIR="/mnt/sdcard/boot/tmp/upgrade.$STAMP"

cleanup_files()
 {
  rm -rf $UPGRADE_DIR
 }


if [ ! -f "$1" ]
 then
  echo "parameter error - cannot find $1 file - stop."
  exit 0
 fi

echo "starting firmware upgrade..."

mount /dev/mmcblk0p1 /mnt/sdcard/boot/

if [ ! -f /mnt/sdcard/boot/bootcode.bin ] 
 then
  echo "there is something wrong with your boot partition - no bootcode.bin - stop."
  umount /mnt/sdcard/boot
  exit 0
 fi

mkdir -p $UPGRADE_DIR
mv $1 $UPGRADE_DIR
cd $UPGRADE_DIR

tar -zxvf $1

if [ -f zImage -a -f zImage.md5sum -a -f initramfs.cpio.gz -a -f initramfs.cpio.gz.md5sum ] 
 then

  ZIMAGE_MD5_CLAIMED=`cat zImage.md5sum | awk '{print $1;}'`
  RAMFS_MD5_CLAIMED=`cat initramfs.cpio.gz.md5sum | awk '{print $1;}'`
  echo "calculating MD5 checksums..."
  ZIMAGE_MD5_ACTUAL=`md5sum zImage | awk '{print $1;}'`
  RAMFS_MD5_ACTUAL=`md5sum initramfs.cpio.gz | awk '{print $1;}'`

  if [ "$ZIMAGE_MD5_CLAIMED" = "$ZIMAGE_MD5_ACTUAL" -a "$RAMFS_MD5_CLAIMED" = "$RAMFS_MD5_ACTUAL" ] 
   then
    echo "copying firmware files to boot partition..."
    cp initramfs.cpio.gz /mnt/sdcard/boot/
    cp zImage /mnt/sdcard/boot/
    cp initramfs.cpio.gz.md5sum /mnt/sdcard/boot/
    cp zImage.md5sum /mnt/sdcard/boot/
    echo "done."
    cleanup_files
    cd /tmp
    umount /mnt/sdcard/boot
    echo "all done - reboot to load new firmware."
    exit 1
   else
    echo "firmware file checksum is not matching provided checksum - stop. upgrade NOT performed."
    cleanup_files
    cd /tmp
    umount /mnt/sdcard/boot
    exit 0
   fi

 else
  echo "incomplete firmware image - stop. upgrade NOT performed."
  cleanup_files
  cd /tmp
  umount /mnt/sdcard/boot
  exit 0
 fi

# end
