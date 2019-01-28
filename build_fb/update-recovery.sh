#!/bin/sh 
echo "Copying image files to recovery folder "
DIR=$PWD
echo $PWD
cd ../sources/meta-thermo-mx6/recipes-qt/tfsapp/files/recovery
rm -rf SPL 
rm -rf u-boot.img 
rm -rf u-boot_gb.img 
rm -rf tfs-image-rootfs-var-som-mx6.ubi
rm -rf uImage
rm -rf uImage-imx6dl-var-som-cap.dtb
rm -rf uImage-imx6q-var-som-cap.dtb 
sync
echo "Sync/Updating image files to recovery folder "
cp -Lr $DIR/tmp/deploy/images/var-som-mx6/SPL-nand SPL
cp -Lr $DIR/tmp/deploy/images/var-som-mx6/u-boot.img-nand  u-boot.img
cp -Lr $DIR/tmp/deploy/images/var-som-mx6/u-boot.img-nand  u-boot_gb.img
cp -Lr $DIR/tmp/deploy/images/var-som-mx6/uImage . 
cp -Lr $DIR/tmp/deploy/images/var-som-mx6/uImage-imx6q-var-som-cap.dtb .
cp -Lr $DIR/tmp/deploy/images/var-som-mx6/uImage-imx6dl-var-som-cap.dtb .
cp -Lr $DIR/tmp/deploy/images/var-som-mx6/tfs-image-rootfs-var-som-mx6.ubi .
cp -Lr $DIR/tmp/deploy/images/var-som-mx6/tfs-image-minirfs-var-som-mx6.cpio.gz.u-boot .
chmod +x *
sync

