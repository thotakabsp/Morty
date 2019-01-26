require qt5.inc
require qt5-git.inc

LICENSE = "GFDL-1.3 & BSD & ( GPL-3.0 & The-Qt-Company-GPL-Exception-1.0 | The-Qt-Company-Commercial ) & ( GPL-2.0+ | LGPL-3.0 | The-Qt-Company-Commercial )"
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
# common for qtbase-native, qtbase-nativesdk and qtbase
SRC_URI += "\
    file://0001-Add-linux-oe-g-platform.patch \
    file://0002-qlibraryinfo-allow-to-set-qt.conf-from-the-outside-u.patch \
    file://0003-Add-external-hostbindir-option.patch \
    file://0004-qt_module-Fix-pkgconfig-and-libtool-replacements.patch \
    file://0005-configure-bump-path-length-from-256-to-512-character.patch \
    file://0006-QOpenGLPaintDevice-sub-area-support.patch \
    file://0007-linux-oe-g-Invert-conditional-for-defining-QT_SOCKLE.patch \
    file://0008-configure-paths-for-target-qmake-properly.patch \
    file://0009-Reorder-EGL-libraries-from-pkgconfig-and-defaults.patch \
    file://0010-Pretend-Qt5-wasn-t-found-if-OE_QMAKE_PATH_EXTERNAL_H.patch \
"

DEPENDS += "qtbase-native"

# LGPL-3.0 is used only in src/plugins/platforms/android/extract.cpp

# for syncqt
RDEPENDS_${PN}-tools += "perl"

# separate some parts of PACKAGECONFIG which are often changed
# be aware that you need to add icu to build qtwebkit, default
# PACKAGECONFIG is kept rather minimal for people who don't need
# stuff like webkit (and it's easier to add options than remove)

PACKAGECONFIG_GL ?= "${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'gl', '', d)}"
PACKAGECONFIG_FB ?= "${@bb.utils.contains('DISTRO_FEATURES', 'directfb', 'directfb', '', d)}"
PACKAGECONFIG_X11 ?= "${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'xcb xsync xshape xrender xrandr xfixes xinput2 xcursor glib xkb', '', d)}"
PACKAGECONFIG_FONTS ?= ""
PACKAGECONFIG_SYSTEM ?= "jpeg libpng zlib"
PACKAGECONFIG_MULTIMEDIA ?= "${@bb.utils.contains('DISTRO_FEATURES', 'pulseaudio', 'pulseaudio', '', d)}"
PACKAGECONFIG_DISTRO ?= ""
# Either release or debug, can be overridden in bbappends
PACKAGECONFIG_RELEASE ?= "release"
# This is in qt5.inc, because qtwebkit-examples are using it to enable ca-certificates dependency
# PACKAGECONFIG_OPENSSL ?= "openssl"
PACKAGECONFIG_DEFAULT ?= "dbus udev evdev widgets tools libs"

PACKAGECONFIG ?= " \
    ${PACKAGECONFIG_RELEASE} \
    ${PACKAGECONFIG_DEFAULT} \
    ${PACKAGECONFIG_OPENSSL} \
    ${PACKAGECONFIG_GL} \
    ${PACKAGECONFIG_FB} \
    ${PACKAGECONFIG_X11} \
    ${PACKAGECONFIG_FONTS} \
    ${PACKAGECONFIG_SYSTEM} \
    ${PACKAGECONFIG_MULTIMEDIA} \
    ${PACKAGECONFIG_DISTRO} \
"

PACKAGECONFIG[release] = "-release,-debug"
PACKAGECONFIG[debug] = ""
PACKAGECONFIG[developer] = "-developer-build"
PACKAGECONFIG[sm] = "-sm,-no-sm"
PACKAGECONFIG[tests] = "-make tests,-nomake tests"
PACKAGECONFIG[examples] = "-make examples -compile-examples,-nomake examples"
PACKAGECONFIG[tools] = "-make tools,-nomake tools"
# only for completeness, configure will add libs even if you try to explicitly remove it
PACKAGECONFIG[libs] = "-make libs,-nomake libs"
# accessibility is required to compile qtquickcontrols
PACKAGECONFIG[accessibility] = "-accessibility,-no-accessibility"
PACKAGECONFIG[glib] = "-glib,-no-glib,glib-2.0"
# use either system freetype or bundled freetype, if you disable freetype completely
# fontdatabases/basic/qbasicfontdatabase.cpp will fail to build and system freetype
# works only together with fontconfig
PACKAGECONFIG[freetype] = "-system-freetype,-freetype,freetype"
PACKAGECONFIG[harfbuzz] = "-system-harfbuzz,-no-harfbuzz,harfbuzz"
PACKAGECONFIG[jpeg] = "-system-libjpeg,-no-libjpeg,jpeg"
PACKAGECONFIG[libpng] = "-system-libpng,-no-libpng,libpng"
PACKAGECONFIG[zlib] = "-system-zlib,-qt-zlib,zlib"
PACKAGECONFIG[pcre] = "-system-pcre,-qt-pcre,pcre"
PACKAGECONFIG[eglfs] = "-eglfs,-no-eglfs,drm"
PACKAGECONFIG[gl] = "-opengl desktop,,virtual/libgl"
PACKAGECONFIG[gles2] = "-opengl es2,,virtual/libgles2 virtual/egl"
PACKAGECONFIG[tslib] = "-tslib,-no-tslib,tslib"
PACKAGECONFIG[cups] = "-cups,-no-cups,cups"
PACKAGECONFIG[dbus] = "-dbus,-no-dbus,dbus"
PACKAGECONFIG[xcb] = "-xcb -xcb-xlib -system-xcb,-no-xcb,libxcb xcb-util-wm xcb-util-image xcb-util-keysyms xcb-util-renderutil"
PACKAGECONFIG[sql-ibase] = "-sql-ibase,-no-sql-ibase"
PACKAGECONFIG[sql-mysql] = "-sql-mysql,-no-sql-mysql,mysql5"
PACKAGECONFIG[sql-psql] = "-sql-psql,-no-sql-psql,postgresql"
PACKAGECONFIG[sql-odbc] = "-sql-odbc,-no-sql-odbc"
PACKAGECONFIG[sql-oci] = "-sql-oci,-no-sql-oci"
PACKAGECONFIG[sql-tds] = "-sql-tds,-no-sql-tds"
PACKAGECONFIG[sql-db2] = "-sql-db2,-no-sql-db2"
PACKAGECONFIG[sql-sqlite2] = "-sql-sqlite2,-no-sql-sqlite2,sqlite"
PACKAGECONFIG[sql-sqlite] = "-sql-sqlite -system-sqlite,-no-sql-sqlite,sqlite3"
PACKAGECONFIG[xcursor] = "-xcursor,-no-xcursor,libxcursor"
PACKAGECONFIG[xinput2] = "-xinput2,-no-xinput2,libxi"
PACKAGECONFIG[xfixes] = "-xfixes,-no-xfixes,libxfixes"
PACKAGECONFIG[xrandr] = "-xrandr,-no-xrandr,libxrandr"
PACKAGECONFIG[xrender] = "-xrender,-no-xrender,libxrender"
PACKAGECONFIG[xshape] = "-xshape,-no-xshape"
PACKAGECONFIG[xsync] = "-xsync,-no-xsync"
PACKAGECONFIG[openvg] = "-openvg,-no-openvg"
PACKAGECONFIG[iconv] = "-iconv,-no-iconv,virtual/libiconv"
PACKAGECONFIG[xkb] = "-xkb,-no-xkb -no-xkbcommon,libxkbcommon"
PACKAGECONFIG[xkbcommon-evdev] = "-xkbcommon-evdev,-no-xkbcommon-evdev,libxkbcommon,xkeyboard-config"
PACKAGECONFIG[evdev] = "-evdev,-no-evdev"
PACKAGECONFIG[mtdev] = "-mtdev,-no-mtdev,mtdev"
# depends on glib
PACKAGECONFIG[fontconfig] = "-fontconfig,-no-fontconfig,fontconfig"
PACKAGECONFIG[gtk] = "-gtk,-no-gtk,gtk+"
PACKAGECONFIG[directfb] = "-directfb,-no-directfb,directfb"
PACKAGECONFIG[linuxfb] = "-linuxfb,-no-linuxfb"
PACKAGECONFIG[mitshm] = "-mitshm,-no-mitshm,mitshm"
PACKAGECONFIG[kms] = "-kms,-no-kms,virtual/mesa virtual/egl"
# needed for qtwebkit
PACKAGECONFIG[icu] = "-icu,-no-icu,icu"
PACKAGECONFIG[udev] = "-libudev,-no-libudev,udev"
PACKAGECONFIG[openssl] = "-openssl,-no-openssl,openssl,libssl"
PACKAGECONFIG[alsa] = "-alsa,-no-alsa,alsa-lib"
PACKAGECONFIG[pulseaudio] = "-pulseaudio,-no-pulseaudio,pulseaudio"
PACKAGECONFIG[widgets] = "-widgets,-no-widgets"
PACKAGECONFIG[libproxy] = "-libproxy,-no-libproxy,libproxy"
PACKAGECONFIG[libinput] = "-libinput,-no-libinput,libinput"
PACKAGECONFIG[journald] = "-journald,-no-journald,systemd"
PACKAGECONFIG += " \
    accessibility \
    cups \
    fontconfig \
    freetype \
    gif \
    glib \
    libinput \
    linuxfb \
    qml-debug \
    sql-sqlite \
    tslib \
"
QT_CONFIG_FLAGS += " \
    -shared \
    -silent \
    -no-pch \
    -no-rpath \
    -pkg-config \
    ${PACKAGECONFIG_CONFARGS} \
"

do_generate_qt_config_file_append() {
    cat >> ${QT_CONF_PATH} <<EOF

[EffectivePaths]
Prefix=..
EOF
}

# qtbase is exception, we need to use mkspecs from ${S}
QMAKE_MKSPEC_PATH = "${B}"

# another exception is that we need to run bin/qmake, because EffectivePaths are relative to qmake location
OE_QMAKE_QMAKE_ORIG = "${STAGING_BINDIR_NATIVE}${QT_DIR_NAME}/qmake"
OE_QMAKE_QMAKE = "bin/qmake"

# qtbase is exception, configure script is using our get(X)QEvalMakeConf and setBootstrapEvalVariable functions to read it from shell
export OE_QMAKE_COMPILER
export OE_QMAKE_CC
export OE_QMAKE_CFLAGS
export OE_QMAKE_CXX
export OE_QMAKE_CXXFLAGS
export OE_QMAKE_LINK
export OE_QMAKE_LDFLAGS
export OE_QMAKE_AR
export OE_QMAKE_STRIP

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://oe-device-extra.pri \
    "
do_configure_prepend() {
    install -m 0644 ${WORKDIR}/oe-device-extra.pri ${S}/mkspecs
}

do_configure() {
    # we need symlink in path relative to source, because
    # EffectivePaths:Prefix is relative to qmake location
    if [ ! -e ${B}/bin/qmake ]; then
        mkdir -p ${B}/bin
        ln -sf ${OE_QMAKE_QMAKE_ORIG} ${B}/bin/qmake
    fi

    ${S}/configure -v \
        -opensource -confirm-license \
        -sysroot ${STAGING_DIR_TARGET} \
        -no-gcc-sysroot \
        -prefix ${OE_QMAKE_PATH_PREFIX} \
        -bindir ${OE_QMAKE_PATH_BINS} \
        -libdir ${OE_QMAKE_PATH_LIBS} \
        -datadir ${OE_QMAKE_PATH_DATA} \
        -sysconfdir ${OE_QMAKE_PATH_SETTINGS} \
        -docdir ${OE_QMAKE_PATH_DOCS} \
        -headerdir ${OE_QMAKE_PATH_HEADERS} \
        -archdatadir ${OE_QMAKE_PATH_ARCHDATA} \
        -libexecdir ${OE_QMAKE_PATH_LIBEXECS} \
        -plugindir ${OE_QMAKE_PATH_PLUGINS} \
        -importdir ${OE_QMAKE_PATH_IMPORTS} \
        -qmldir ${OE_QMAKE_PATH_QML} \
        -translationdir ${OE_QMAKE_PATH_TRANSLATIONS} \
        -testsdir ${OE_QMAKE_PATH_TESTS} \
        -examplesdir ${OE_QMAKE_PATH_EXAMPLES} \
        -hostbindir ${OE_QMAKE_PATH_HOST_BINS} \
        -external-hostbindir ${OE_QMAKE_PATH_EXTERNAL_HOST_BINS} \
        -hostdatadir ${OE_QMAKE_PATH_HOST_DATA} \
        -platform ${OE_QMAKESPEC} \
        -xplatform linux-oe-g++ \
        ${QT_CONFIG_FLAGS}

    qmake5_base_do_configure
}

do_install_append() {
    # Avoid qmake error "Cannot read [...]/usr/lib/qt5/mkspecs/oe-device-extra.pri: No such file or directory"
    touch ${D}/${OE_QMAKE_PATH_QT_ARCHDATA}/mkspecs/oe-device-extra.pri

    install -m 0755 ${B}/bin/qmake-target ${D}/${bindir}${QT_DIR_NAME}/qmake

    # Remove example.pro file as it is useless
    rm -f ${D}${OE_QMAKE_PATH_EXAMPLES}/examples.pro

    # Remove macx-ios-clang directory because /usr/lib/qt5/mkspecs/macx-ios-clang/rename_main.sh:#!/bin/bash
    # triggers QA Issue: qtbase-mkspecs requires /bin/bash, but no providers in its RDEPENDS [file-rdeps]
    rm -rf ${D}/${OE_QMAKE_PATH_QT_ARCHDATA}/mkspecs/macx-ios-clang
    # and this one has /bin/bash shebang, but checkbashisms doesn't show any reason for it
    sed -i 's@^#!/bin/bash$@#!/bin/sh@g' ${D}/${OE_QMAKE_PATH_QT_ARCHDATA}/mkspecs/features/data/mac/objc_namespace.sh

    # Replace host paths with qmake built-in properties
    sed -i -e 's|${STAGING_DIR_NATIVE}${prefix_native}|$$[QT_HOST_PREFIX]|g' \
        -e 's|${STAGING_DIR_HOST}|$$[QT_SYSROOT]|g' \
        ${D}/${OE_QMAKE_PATH_QT_ARCHDATA}/mkspecs/*.pri
}

RRECOMMENDS_${PN}-plugins += "${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'libx11-locale', '', d)}"

SRCREV = "master" 
