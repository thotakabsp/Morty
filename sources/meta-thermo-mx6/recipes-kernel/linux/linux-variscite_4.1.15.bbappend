SRCREV = "c35aefda03d077fd28a32253737bc2ace9dc86ce"

FILESEXTRAPATHS_append := "${THISDIR}/${PN}:"

SRC_URI += "    \
	file://0001-Initial-commit.patch	\
"
#	file://0001-Thermo-Base.patch	\
#	file://0002-network-support.patch  \
#	file://0003-Fix-issue-with-RTC-driver-info-in-dtb-file.patch \
#	file://0004-Removed-cdc_ether-support.patch 	\
#	file://0005-enabled-rtl8192-driver.patch 	\
#	file://0006-wifi-driver-config-enabled.patch	\
#	file://0007-EXT4-we-need-goto-next-group-in-case-we-found-reserved-inode.patch 	\
#"
