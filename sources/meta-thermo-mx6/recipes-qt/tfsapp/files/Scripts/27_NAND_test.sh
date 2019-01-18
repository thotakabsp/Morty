#!/bin/sh 
echo "**********<<<<<<<<<< NAND TEST END>>>>>>>>>>**********" 

show_nand_info()
{
	cat /proc/mtd
}

erase_nand()
{
flash_erase /dev/mtd0 0 0
flash_erase /dev/mtd1 0 0
flash_erase /dev/mtd2 0 0
flash_erase /dev/mtd3 0 0
flash_erase /dev/mtd4 0 0
flash_erase /dev/mtd5 0 0
flash_erase /dev/mtd6 0 0
flash_erase /dev/mtd7 0 0
}

update_SPL()
{
	if [ -f /run/media/sda1/jethro/SPL-nand ]; then
		flash_erase /dev/mtd0 0 0
		kobs-ng init -x /run/media/sda1/jethro/SPL-nand  --search_exponent=1 -v
	else
		echo "unable to find the needed files, not writing "
	fi
}

update_uboot()
{
	if [ -f /run/media/sda1/jethro/u-boot.img ]; then
		flash_erase /dev/mtd1 0 0
		nandwrite -p /dev/mtd1 /run/media/sda1/jethro/u-boot.img 

		flash_erase /dev/mtd2 0 0

		flash_erase /dev/mtd3 0 0
		nandwrite -p /dev/mtd3 /run/media/sda1/jethro/u-boot.img
	else
		echo "unable to find the needed files, not writing "
	fi
}

update_kernel()
{
	if [ -f	/run/media/sda1/jethro/imx6q-var-som.dtb ]; then
	if [ -f /run/media/sda1/jethro/uImage ]; then
		flash_erase /dev/mtd4 0 0
		nandwrite -p /dev/mtd4 /run/media/sda1/jethro/imx6q-var-som.dtb 

		flash_erase /dev/mtd5 0 0
		nandwrite -p /dev/mtd5 /run/media/sda1/jethro/uImage
	fi
	else
		echo "unable to find the needed files, not writing "
	fi
}

update_fs()
{
	if [ -f /run/media/sda1/jethro/tfs-image-test-var-som-mx6.ubi ]; then
		flash_erase /dev/mtd7 0 0
		ubiformat /dev/mtd7 -f /run/media/sda1/jethro/tfs-image-test-var-som-mx6.ubi -s 2048 -O 2048
	else
		echo "unable to find the needed files, not writing "	
	fi
}

update_all()
{
	update_SPL
	update_uboot
	update_kernel
	update_fs
}

update_nand()
{
	clear
	echo " 1) update SPL "
	echo " 2) update u-boot "
	echo " 3) update linux kernel "
	echo " 4) update file system"
	echo " 5) update all "
	echo " 6) Exit "
	read ans
	case $ans in
	1)
		update_SPL;;
	2) 
		update_uboot;;
	3) 
		update_kernel;;
	4)
		update_fs;;
	5)
		update_all;;
	6)
		exit 0;;
	esac
}



i=3                                                     
while [ $i -lt 5 ];                                             
do                                                   
	echo "1) Show nand partition info"
	echo "2) Erase nand partitions"
	echo "3) update NAND software "
        read var                                   
	case $var in
	1)
		show_nand_info
		;;
	2)
		erase_nand
		;;
	3)	update_nand
		;;
	*)
		;;
	esac
        echo "do you want to close the test (y/n)?"
        read ans          
        if [ $ans == "y" ]               
        then                     
                i=10                                       
        fi                                                 
done                                       

echo "**********<<<<<<<<<< NAND TEST END>>>>>>>>>>**********" 
