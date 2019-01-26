#!/bin/sh

RFS_VERIONS="6.17.0"
U_BOOT_CUSTOM_VERSION="1.11.0"
DTB_VERSION="2.6.0"
VERSION="1.11.0"

#echo $RFS-VERSION > ~/var-fslc-yocto/sources/meta-thermo-mx6/recipes-core/initscripts/files/.version
#sed -i '/+#define U_BOOT_CUSTOM_VERSION	"1.10.0"/c\+#define U_BOOT_CUSTOM_VERSION	"1.11.0"' 
#~/var-fslc-yocto/sources/meta-thermo-mx6/recipes-bsp/u-boot/patches/0001-Morty-base.patch
#sed -i '/+#define U_BOOT_CUSTOM_VERSION	"1.10.0"/c\+#define U_BOOT_CUSTOM_VERSION	"1.11.0"' 
#+#define DTB_VERSION    "2.6.0"
#sed -i '/+#define DTB_VERSION    "2.5.0"/c\+#define DTB_VERSION    "2.6.0"
#+#define VERSION "1.11.0"
#sed -i '/+#define VERSION    "1.10.0"/c\+#define VERSION    "1.11.0"
#~/var-fslc-yocto/sources/meta-thermo-mx6/recipes-kernel/linux/linux-variscite/0001-Initial-commit.patch
