DESCRIPTION = "Packagegroup for base embedded Linux image"
LICENSE = "CLOSED"
PR = "r0"

inherit packagegroup

MACHINE_EXTRA_INSTALL ?= ""
WIFI_IMAGE_INSTALL = "rtl8723bu rtl8192cu edimax7822ulc rtwpriv "
TEST_IMAGE_INSTALL = "testscripts alsa-utils"
NTP_IMAGE_INSTALL = "ntp-tickadj ntp ntpdate ntp-utils"
FS_TOOLS_INSTALL = "util-linux e2fsprogs dosfstools parted"
NET_TOOLS_INSTALL = "net-tools iperf iptables curl"
NAND_IMAGE_INSTALL = "mtd-utils imx-kobs mtd-utils-ubifs u-boot-fw-utils"
3G_IMAGE_INSTALL = "minicom ppp"
PACKAGE_IMAGE_INSTALL = "smartpm"
SUPPORT_IMAGE_INSTALL = "bc i2c-tools udev udev-extraconf udisks sqlite3 dbus watchdog libmodbus openssh-sftp-server"

RDEPENDS_${PN} = "\
        kernel-modules \
        ca-certificates \
        tzdata tzdata-misc tzdata-posix tzdata-right tzdata-africa \
	tzdata-americas tzdata-antarctica tzdata-arctic tzdata-asia \
	tzdata-atlantic tzdata-australia tzdata-europe tzdata-pacific \
        connman \
	connman-client \
	ofono	\
	ofono-tests \
	usb-modeswitch \
	usb-modeswitch-data \
	${NTP_IMAGE_INSTALL} \
	${FS_TOOLS_INSTALL} \
	${NET_TOOLS_INSTALL} \
	${NAND_IMAGE_INSTALL} \
	${3G_IMAGE_INSTALL} \
	${SUPPORT_IMAGE_INSTALL} \
	${PACKAGE_IMAGE_INSTALL} \
	${WIFI_IMAGE_INSTALL} \
	${MACHINE_EXTRA_INSTALL} \
"
