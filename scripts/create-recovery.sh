#!/bin/sh
./update-recovery.sh
cd /home/bspeng09/var-fslc-yocto/build_fb 
bitbake -c cleansstate linux-variscite 
bitbake -c cleansstate rtl8192cu 
bitbake -c cleansstate edimax7822ulc 
bitbake -c cleansstate rtwpriv 
bitbake -c cleansstate testscripts 
bitbake -c cleansstate deeplaser 
bitbake -c cleansstate initscripts 
bitbake -c cleansstate variscite 
bitbake -c cleansstate recovery 
bitbake -c cleansstate tfs-image-recovery 
bitbake -v -k tfs-image-recovery
