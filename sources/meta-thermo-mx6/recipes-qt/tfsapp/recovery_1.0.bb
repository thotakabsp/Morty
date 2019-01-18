SUMMARY = "Install recovery Image to SD card"
LICENSE = "CLOSED"

SRC_URI = "file://recovery/*	\
	   file://fbv	\
	   file://wd_keepalive	\
	"
	

#This package doesn't have any files for the rootfs in it, option needed to create an empty 
# package so when the rootfs image is made it finds the mksd_xxx.deb package and doesn't complain
FILES_${PN} = "/home/root	\
		/home/root/recovery	\
		/home/root/recovery/*	\
		/usr/bin/	\
		/usr/bin/fbv	\
		/usr/bin/wd_keepalive	\
		"

FILES_${PN}-dbg += "/home/root/recovery/.debug	\
			"

# Copy script to the deploy area with u-boot, uImage, and rootfs
do_install () {
   install -d ${D}/home/root/recovery
   install -d ${D}/usr/bin
   install -m 0755 ${WORKDIR}/fbv ${D}/usr/bin/
   install -m 0755 ${WORKDIR}/wd_keepalive ${D}/usr/bin/
   rsync -a ${WORKDIR}/recovery/* ${D}/home/root/recovery/
}
