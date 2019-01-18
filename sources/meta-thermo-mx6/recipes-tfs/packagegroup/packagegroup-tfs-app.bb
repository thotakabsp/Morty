DESCRIPTION = "Packagegroup for DeltaK UI application"
LICENSE = "CLOSED"
PR = "r0"

inherit packagegroup

MACHINE_EXTRA_INSTALL ?= ""

RDEPENDS_${PN} = "\
	sqlcipher \
        ${MACHINE_EXTRA_INSTALL} \
	variscite	\
"
