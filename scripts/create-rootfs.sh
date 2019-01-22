#!/bin/sh
# This will clean rootfs and create the rootfs image  
# added all 
bitbake -c cleansstate linux-variscite 
bitbake -c cleansstate edimax7822ulc 
bitbake -c cleansstate rtl8192cu 
bitbake -c cleansstate rtwpriv 
bitbake -c cleansstate ofono 
bitbake -c cleansstate connman 
bitbake -c cleansstate sqlcipher 
bitbake -c cleansstate testscripts 
bitbake -c cleansstate deeplaser 
bitbake -c cleansstate initscripts 
bitbake -c cleansstate variscite 
bitbake -c cleansstate recovery 
bitbake -c cleansstate geoclue 
bitbake -c cleansstate geoip 
bitbake -c cleansstate smbus2
bitbake -c cleansstate tfs-image-rootfs
bitbake -c cleansstate tfs-image-recovery
bitbake -v -k tfs-image-rootfs
