SUMMARY = "Install progress app"
LICENSE = "CLOSED"

SRC_URI = "file://psplash"

#This package doesn't have any files for the rootfs in it, option needed to create an empty 
# package so when the rootfs image is made it finds the mksd_xxx.deb package and doesn't complain
FILES_${PN} = "/usr/bin/	\
		/usr/bin/psplash	\
		/usr/bin/.debug	\
		"

# Copy script to the deploy area with u-boot, uImage, and rootfs
do_install () {
   install -d ${D}/usr/bin
   install -m 0755 ${WORKDIR}/psplash ${D}/usr/bin 
}
