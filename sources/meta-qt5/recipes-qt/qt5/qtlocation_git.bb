require qt5.inc
require qt5-git.inc

LICENSE = "GFDL-1.3 & BSD & (LGPL-2.1 & The-Qt-Company-Qt-LGPL-Exception-1.1 | LGPL-3.0) | GPL-2.0"
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

DEPENDS += "qtbase qtxmlpatterns qtdeclarative qtquickcontrols"

PACKAGECONFIG ??= ""
# older geoclue 0.x is needed
PACKAGECONFIG[geoclue] = "OE_GEOCLUE_ENABLED,,gconf geoclue"
PACKAGECONFIG[gypsy] = "OE_GYPSY_ENABLED,,gconf gypsy"

do_configure_prepend() {
    # disable geoclue tests if it isn't enabled by PACKAGECONFIG
    sed -i -e 's/^\(qtCompileTest(geoclue)\)/OE_GEOCLUE_ENABLED:\1/' ${S}/qtlocation.pro
    sed -i -e 's/^\(qtCompileTest(geoclue-satellite)\)/OE_GEOCLUE_ENABLED:\1/' ${S}/qtlocation.pro
    # disable gypsy test if it isn't enabled by PACKAGECONFIG
    sed -i -e 's/^\(qtCompileTest(gypsy)\)/OE_GYPSY_ENABLED:\1/' ${S}/qtlocation.pro
}

EXTRA_QMAKEVARS_PRE += "${@bb.utils.contains('PACKAGECONFIG', 'geoclue', 'CONFIG+=OE_GEOCLUE_ENABLED', '', d)}"
EXTRA_QMAKEVARS_PRE += "${@bb.utils.contains('PACKAGECONFIG', 'gypsy', 'CONFIG+=OE_GYPSY_ENABLED', '', d)}"

SRCREV = "master" 
