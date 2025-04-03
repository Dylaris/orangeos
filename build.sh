#!/usr/bin/bash

# dd if=/dev/null of=pm.img bs=512 count=1 conv=notrunc
# sudo losetup /dev/loop0 pm.img
# sudo mkfs.msdos /dev/loop0
# sudo fsck.msdos /dev/loop0
# sudo losetup -d /dev/loop0
# file pm.img

sudo mount -o loop pmtest/pm.img /mnt/floppy
sudo cp pmtest/pmtest7.com /mnt/floppy
sudo umount /mnt/floppy
