DESCRIPTION = "debug modules "
LICENSE = "CLOSED"

inherit packagegroup

PACKAGEGROUP_DISABLE_COMPLEMENTARY = "1"

RDEPENDS_${PN} += " \
		valgrind \
"
