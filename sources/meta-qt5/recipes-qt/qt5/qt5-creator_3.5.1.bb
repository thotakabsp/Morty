SUMMARY = "Qt Creator is a new cross-platform Qt IDE"

# Note:
# The toolchain auto detection does not work completely yet. To compile/debug
# open menu 'Tools/Options and select 'Build & Run'. In tab 'Kits' select 'Desktop'
# 'Compiler/Manage...' and add local gcc'. At 'Debugger' select
# 'System GDB at /usr/bin/gdb.

HOMEPAGE = "https://qt-project.org/"
LICENSE = "LGPLv2.1 | GPLv3"
LIC_FILES_CHKSUM = " \
	file://.QT-ENTERPRISE-LICENSE-AGREEMENT;md5=1524eebc5d2d92939737981faa928eeb   \
	file://.QT-ENTERPRISE-LICENSE-AGREEMENT-US;md5=8660e597a7c58b646bfe98e6e500901c   \
	file://.QT-FOR-APPLICATION-DEVELOPMENT-LICENSE-AGREEMENT;md5=e5a48ca205f0a07d5c3eb1e9d11eeb9f   \
	file://.QT-FOR-APPLICATION-DEVELOPMENT-LICENSE-AGREEMENT-US;md5=ca4a85d4b241a36b3b60924c122946f2   \
	file://.QT-FOR-AUTOMOTIVE-LICENSE-AGREEMENT;md5=f6bb6e2be25ffd02bc9fcf572cfc9c66   \
	file://.QT-FOR-AUTOMOTIVE-LICENSE-AGREEMENT-US;md5=ee81132682ae93827b2d4e09b5dd5e0f   \
	file://.QT-FOR-DEVICE-CREATION-LICENSE-AGREEMENT;md5=dd8af08426c401f947bf15ac37fa87c1   \
	file://.QT-FOR-DEVICE-CREATION-LICENSE-AGREEMENT-US;md5=c7ff76fa9e6bc75f8622346416c84da2   \
	file://.QT-INDIE-LICENSE-AGREEMENT;md5=2bf2cae6317acb3edd063771f3708410   \
	file://.QT-PROFESSIONAL-LICENSE-AGREEMENT;md5=58a3f91b002dacc81fe4d8a025337ac4   \
"

inherit qmake5

DEPENDS = "qtbase qtscript qtwebkit qtxmlpatterns qtx11extras qtdeclarative qttools qttools-native qtsvg qtquick1"

SRC_URI = " \
    http://download.qt.io/official_releases/qtcreator/3.5/${PV}/qt-creator-opensource-src-${PV}.tar.gz \
    file://0001-Fix-Allow-qt-creator-to-build-on-arm-aarch32-and-aar.patch \
    file://qtcreator.desktop.in \
"
SRC_URI[md5sum] = "77aef7df837eba07c7ce6037ee504c05"
SRC_URI[sha256sum] = "5925ac818a08be919094e0f28fb4c5d8896765e0975d54d353e4c50f13d63e65"

S = "${WORKDIR}/qt-creator-opensource-src-${PV}"

EXTRA_QMAKEVARS_PRE += "IDE_LIBRARY_BASENAME=${baselib}${QT_DIR_NAME}"

do_configure_append() {
    # Find native tools
    sed -i 's:${STAGING_BINDIR}.*/lrelease:${STAGING_BINDIR_NATIVE}${QT_DIR_NAME}/lrelease:g' ${B}/share/qtcreator/translations/Makefile
    sed -i 's:${STAGING_BINDIR}.*/qdoc:${STAGING_BINDIR_NATIVE}${QT_DIR_NAME}/qdoc:g' ${B}/Makefile

    # see qtbase-native.inc
    # sed -i 's:QT_INSTALL_DOCS=${docdir}:QT_INSTALL_DOCS=${STAGING_DATADIR_NATIVE}${QT_DIR_NAME}/doc:g' ${B}/Makefile
}

do_compile_append() {
    # build docs
    #oe_runmake docs_online
}

do_install() {
    oe_runmake install INSTALL_ROOT=${D}${prefix}
    oe_runmake install_inst_qch_docs INSTALL_ROOT=${D}${prefix}
    # install desktop and ensure that qt-creator finds qmake
    install -d ${D}${datadir}/applications
    install -m 0644 ${WORKDIR}/qtcreator.desktop.in ${D}${datadir}/applications/qtcreator.desktop
    sed -i 's:@QT5_QMAKE@:${bindir}${QT_DIR_NAME}:g' ${D}${datadir}/applications/qtcreator.desktop
}

FILES_${PN} += " \
    ${datadir}/qtcreator \
    ${datadir}/icons \
    ${libdir}${QT_DIR_NAME}/qtcreator \
"
FILES_${PN}-dbg += " \
    ${libdir}${QT_DIR_NAME}/qtcreator/.debug \
    ${libdir}${QT_DIR_NAME}/qtcreator/plugins/.debug \
    ${libdir}${QT_DIR_NAME}/qtcreator/plugins/qmldesigner/.debug \
    ${libdir}${QT_DIR_NAME}/qtcreator/plugins/qbs/plugins/.debug \
"

FILES_${PN}-dev += " \
    ${libdir}${QT_DIR_NAME}/qtcreator/*${SOLIBSDEV} \
"

RDEPENDS_${PN} += "perl"
RCONFLICTS_${PN} = "qt-creator"

# To give best user experience out of the box..
RRECOMMENDS_${PN} += " \
    packagegroup-qt5-toolchain-target \
    binutils \
    ccache \
    make \
    gcc-symlinks g++-symlinks cpp-symlinks \
    gdb \
"
