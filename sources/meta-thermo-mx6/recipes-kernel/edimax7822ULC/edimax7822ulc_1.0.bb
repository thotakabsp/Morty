SUMMARY = "Add the Edimax 7822 ULC Wi-Fi driver as an out-of-tree module"
LICENSE = "CLOSED"
SRC_URI = " \
		file://edimax7822ULC.tar.gz \
"
inherit module

do_compile(){
	cd ${WORKDIR}
	make KSRC=${STAGING_KERNEL_DIR} ARCH=arm CROSS_COMPILE=arm-fslc-linux-gnueabi-
}

do_install() {
    install -d ${D}/lib/firmware/
    mkdir -p ${D}/lib/firmware/rtlwifi/
    install -d $D}/lib/firmware/rtlwifi/
    cd ${WORKDIR}
    mkdir -p ${D}/lib/modules/${KERNEL_VERSION}/kernel/drivers/net/wireless/rtlwifi/edimax7822ulc/
    install -m 0644 ${WORKDIR}/88x2bu.ko ${D}/lib/modules/${KERNEL_VERSION}/kernel/drivers/net/wireless/rtlwifi/edimax7822ulc/
}

FILES_${PN} = "	\
	/lib/firmware	\
	/lib/firmware/rtlwifi	\
	"
