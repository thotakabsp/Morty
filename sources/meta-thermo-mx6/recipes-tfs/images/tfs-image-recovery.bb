DESCRIPTION = "TFS rootfs image with Qt5 and the End application"

inherit core-image

IMAGE_FSTYPES = "tar sdcard"

LICENSE = "CLOSED"

DEPENDS = "initscripts"
export IMAGE_BASENAME = "tfs-image-recovery"

FS_TOOLS_INSTALL = "util-linux e2fsprogs dosfstools parted"
NAND_IMAGE_INSTALL = "mtd-utils imx-kobs mtd-utils-ubifs"
SUPPORT_IMAGE_INSTALL = "bc udev udev-extraconf watchdog libpng"
APP_IMAGE_INSTALL = " u-boot-fw-utils kernel-modules" 

IMAGE_INSTALL_append += " \
			 ${FS_TOOLS_INSTALL} \
			 ${NAND_IMAGE_INSTALL} \
			 ${SUPPORT_IMAGE_INSTALL} \
			 ${APP_IMAGE_INSTALL} \
			 recovery	\
			 "
EXTRA_IMAGE_FEATURES = "debug-tweaks "

modify_systemd() {
        rm -rf ${IMAGE_ROOTFS}/etc/systemd/system/getty.target.wants/serial-getty@ttymxc0.service
        rm -rf ${IMAGE_ROOTFS}/etc/systemd/system/getty.target.wants/getty@tty1.service
	touch  ${IMAGE_ROOTFS}/etc/.recovery
        # sysv sysctl config
        IMAGE_SYSCTL_CONF="${IMAGE_ROOTFS}${sysconfdir}/sysctl.conf"
        echo "" >> ${IMAGE_SYSCTL_CONF} && echo "net.ipv6.conf.all.disable_ipv6 = 1" >> ${IMAGE_SYSCTL_CONF}
        echo "" >> ${IMAGE_SYSCTL_CONF} && echo "net.ipv6.conf.default.disable_ipv6 = 1" >> ${IMAGE_SYSCTL_CONF}
        echo "" >> ${IMAGE_SYSCTL_CONF} && echo "net.ipv6.conf.lo.disable_ipv6 = 1" >> ${IMAGE_SYSCTL_CONF}
}

ROOTFS_POSTPROCESS_COMMAND += "modify_systemd;"
