#!/bin/sh
flash_erase /dev/mtd0 0 0
flash_erase /dev/mtd1 0 0
flash_erase /dev/mtd2 0 0
flash_erase /dev/mtd3 0 0
flash_erase /dev/mtd4 0 0
flash_erase /dev/mtd5 0 0
flash_erase /dev/mtd6 0 0
flash_erase /dev/mtd7 0 0
kobs-ng init -x /run/media/sda1/nand/SPL  --search_exponent=1 -v
nandwrite -p /dev/mtd3 /run/media/sda1/nand/u-boot.img 
nandwrite -p /dev/mtd4 /run/media/sda1/nand/imx6q-var-som.dtb 
nandwrite -p /dev/mtd5 /run/media/sda1/nand/uImage 
ubiformat /dev/mtd7 -f /run/media/sda1/nand/tfs-image-rootfs-var-som-mx6.ubi -s 2048 -O 2048
