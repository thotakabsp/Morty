SUMMARY = "Install progress app"
LICENSE = "CLOSED"

DEPENDS += "libusb1 libjpeg-turbo"

SRC_URI = "	\
	file://Scripts/*	\	
	"

TARGET_CC_ARCH += "${LDFLAGS}"

#This package doesn't have any files for the rootfs in it, option needed to create an empty 
# package so when the rootfs image is made it finds the mksd_xxx.deb package and doesn't complain
FILES_${PN} = "/home/root	\
		/home/root/Scripts	\
		/home/root/Scripts/*	\
		"

FILES_${PN}-dbg += "/home/root/Scripts/.debug	\
			"

S = "${WORKDIR}/Scripts"

do_configure () {
    # Specify any needed configure commands here
    :
}

do_compile () {
    # You will almost certainly need to add additional arguments here
    oe_runmake clean
    oe_runmake
}

# Copy script to the deploy area with u-boot, uImage, and rootfs
do_install () {
   install -d ${D}/home/root/Scripts
   rsync -a ${WORKDIR}/Scripts/* ${D}/home/root/Scripts/
   rm -rvf ${D}/home/root/Scripts/*.o
   rm -rvf ${D}/home/root/Scripts/*.c
   rm -rvf ${D}/home/root/Scripts/*.h
}

