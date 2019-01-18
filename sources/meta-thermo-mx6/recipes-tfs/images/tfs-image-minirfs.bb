SUMMARY = "TFS minimal rootfs image for filesystem upgrade."

SUPPORT_IMAGE_INSTALL = "bc udev udev-extraconf watchdog libpng jpeg"
APP_IMAGE_INSTALL = " u-boot-fw-utils kernel-modules "
FS_TOOLS_INSTALL = "util-linux e2fsprogs dosfstools"
NAND_IMAGE_INSTALL = "mtd-utils imx-kobs mtd-utils-ubifs"
#		${CORE_IMAGE_EXTRA_INSTALL}

IMAGE_INSTALL = " packagegroup-core-boot	\
		${ROOTFS_PKGMANAGE_BOOTSTRAP}	\
		${SUPPORT_IMAGE_INSTALL}	\
		${FS_TOOLS_INSTALL}	\
		${NAND_IMAGE_INSTALL}	\
		${APP_IMAGE_INSTALL}	\
		minirfs	\
		"

# Do not pollute the initrd image with rootfs features
IMAGE_FEATURES = "debug-tweaks"

export IMAGE_BASENAME = "tfs-image-minirfs"

IMAGE_FSTYPES = "cpio.gz.u-boot"
IMAGE_CLASSES += "image_types_uboot"

IMAGE_LINGUAS = " "

LICENSE = "MIT"

inherit core-image

IMAGE_ROOTFS_SIZE ?= "8192"

BAD_RECOMMENDATIONS += "busybox-syslog"

modify_minirfs() {
        touch  ${IMAGE_ROOTFS}/etc/.minirfs
}

ROOTFS_POSTPROCESS_COMMAND += "modify_minirfs;"
