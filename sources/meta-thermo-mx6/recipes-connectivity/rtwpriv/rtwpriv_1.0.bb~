DESCRIPTION = "Tools for the Linux Standard Wireless Extension Subsystem for rtl8192"
LICENSE = "CLOSED"

SRC_URI = "file://rtwpriv_4.0_release.tar.gz \
"
do_compile() {
	cd ${WORKDIR}/rtwpriv_4.0_release/
	make ARCH=arm CROSS_COMPILE=arm-fslc-linux-gnueabi- 
}

do_install() {
	install -d ${D}/usr/bin
	cd ${WORKDIR}/rtwpriv_4.0_release/
	install -m 0755 rtwpriv ${D}/usr/bin/rtwpriv
}

FILES_${PN} = "		\
	/usr/bin		\	
"


