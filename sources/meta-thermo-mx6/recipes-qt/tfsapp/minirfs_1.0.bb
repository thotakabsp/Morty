SUMMARY = "Install progress app"
LICENSE = "CLOSED"

RDEPENDS_${PN} = "jpeg libpng"
SRC_URI = "file://fbv	\
		file://wd_keepalive	\
		file://UI_Recovery_03.png	\
	"

#This package doesn't have any files for the rootfs in it, option needed to create an empty 
# package so when the rootfs image is made it finds the mksd_xxx.deb package and doesn't complain
FILES_${PN} = "/usr/bin/	\
		/usr/bin/fbv	\
		/usr/bin/wd_keepalive \
		/usr/bin/.debug	\
		/home/root	\
		"

# Copy script to the deploy area with u-boot, uImage, and rootfs
do_install () {
   install -d ${D}/usr/bin
   install -d ${D}/home/root
   install -m 0755 ${WORKDIR}/fbv ${D}/usr/bin 
   install -m 0755 ${WORKDIR}/wd_keepalive ${D}/usr/bin 
   install -m 0755 ${WORKDIR}/UI_Recovery_03.png ${D}/home/root/
}
