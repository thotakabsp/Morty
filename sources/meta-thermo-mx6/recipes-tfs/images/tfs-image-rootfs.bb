DESCRIPTION = "TFS rootfs image with Qt5 and the End application"

inherit core-image
inherit systemd


IMAGE_FSTYPES = "tar ubi sdcard"

LICENSE = "CLOSED"

DEPENDS = "u-boot-variscite linux-variscite initscripts deeplaser variscite ofono connman sqlcipher testscripts systemd "
export IMAGE_BASENAME = "tfs-image-rootfs"

IMAGE_INSTALL += " \
			 packagegroup-tfs-base \
			 packagegroup-tfs-deeplaser \
			 packagegroup-tfs-qt5-modules \
			 packagegroup-tfs-app \
			 testscripts \
			 geoclue	\
			 geoip		\
			 "
EXTRA_IMAGE_FEATURES = "debug-tweaks ssh-server-openssh "
INSANE_SKIP_${PN} = "dev-so"

modify_systemd() {
        rm -rf ${IMAGE_ROOTFS}/etc/systemd/system/getty.target.wants/serial-getty@ttymxc0.service
        rm -rf ${IMAGE_ROOTFS}/etc/systemd/system/getty.target.wants/getty@tty1.service
        # sysv sysctl config
        IMAGE_SYSCTL_CONF="${IMAGE_ROOTFS}${sysconfdir}/sysctl.conf"
        echo "" >> ${IMAGE_SYSCTL_CONF} && echo "net.ipv6.conf.all.disable_ipv6 = 1" >> ${IMAGE_SYSCTL_CONF}
        echo "" >> ${IMAGE_SYSCTL_CONF} && echo "net.ipv6.conf.default.disable_ipv6 = 1" >> ${IMAGE_SYSCTL_CONF}
        echo "" >> ${IMAGE_SYSCTL_CONF} && echo "net.ipv6.conf.lo.disable_ipv6 = 1" >> ${IMAGE_SYSCTL_CONF}
}

ROOTFS_POSTPROCESS_COMMAND += "modify_systemd;"
