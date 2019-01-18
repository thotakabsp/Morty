SUMMARY = "Add the Edimax 8723 bluetooth driver as an out-of-tree module"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${WORKDIR}/EW-7611ULB_Bluetooth_driver/Linux_BT_USB_v3.1_20150526_8723BU_BTCOEX_20150119-5844_Edimax/Readme.txt;md5=5a945da7b7d6fe5bb1e6ded7375078b8"

# When building on openembedded Jethro, or older versions, make sure to apply
# this commit:
# https://github.com/openembedded/openembedded-core/commit/afcea61e8eb39234d336c706fdfd4680dea7c060
# to prevent warnings exactly like mentioned in that commit message.

inherit module

SRC_URI = " \
		file://EW-7611ULB_Bluetooth_driver.tar.gz	\
"

#S = "${WORKDIR}/git"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
do_compile(){
	cd ${WORKDIR}/EW-7611ULB_Bluetooth_driver/Linux_BT_USB_v3.1_20150526_8723BU_BTCOEX_20150119-5844_Edimax/bluetooth_usb_driver
	make KDIR=${STAGING_KERNEL_DIR} ARCH=arm CROSS_COMPILE=arm-fslc-linux-gnueabi-
}

do_install() {
    # Module
    install -d ${D}/lib/modules/${KERNEL_VERSION}/kernel/drivers/bluetooth
    install -d ${D}/lib/firmware/
    install -m 0644 ${WORKDIR}/EW-7611ULB_Bluetooth_driver/Linux_BT_USB_v3.1_20150526_8723BU_BTCOEX_20150119-5844_Edimax/bluetooth_usb_driver/rtk_btusb.ko ${D}/lib/modules/${KERNEL_VERSION}/kernel/drivers/bluetooth/rtk_btusb.ko
    install -m 0644 ${WORKDIR}/EW-7611ULB_Bluetooth_driver/Linux_BT_USB_v3.1_20150526_8723BU_BTCOEX_20150119-5844_Edimax/bluetooth_usb_driver/../8723BU/rtl8723b_fw ${D}/lib/firmware/
    install -m 0644 ${WORKDIR}/EW-7611ULB_Bluetooth_driver/Linux_BT_USB_v3.1_20150526_8723BU_BTCOEX_20150119-5844_Edimax/bluetooth_usb_driver/../8723BU/rtl8723bu_config ${D}/lib/firmware/
}

FILES_${PN} = "	\
	/lib/firmware	\
	/lib/firmware/rtl8723b_fw	\
	/lib/firmware/rtl8723bu_config	\
	"
