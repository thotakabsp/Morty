DESCRIPTION = "TFS rootfs image with Qt5 and the End application"

inherit core-image

IMAGE_FSTYPES = "tar ubi sdcard"

LICENSE = "CLOSED"

DEPENDS = "u-boot-variscite linux-variscite initscripts testscripts"
export IMAGE_BASENAME = "tfs-image-debug-rootfs"

IMAGE_INSTALL += " \
  packagegroup-tfs-base \
  packagegroup-tfs-deeplaser \
  packagegroup-tfs-qt5-modules \
  packagegroup-tfs-app \
  packagegroup-core-tools-debug \
  packagegroup-tfs-debug \
	"
EXTRA_IMAGE_FEATURES = "debug-tweaks ssh-server-openssh "
INSANE_SKIP_${PN} = "dev-so"
