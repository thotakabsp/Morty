require qt5.inc
require qt5-git.inc

LICENSE = "GPL-3.0 | The-Qt-Company-Commercial"
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

# To enabled Nuance T9 Write support, you need to provide the licensed components
# and enable "t9write" in PACKAGECONFIG. This can be done in a separate .bbappend file.
# for example:
#T9WRITEPACKAGE = "${HOME}/Downloads/zzEval_QT_T9Write_Alpha_v750_20150916.zip"
#SRC_URI += "file://${T9WRITEPACKAGE};subdir=git/src/virtualkeyboard/3rdparty/t9write"
#PACKAGECONFIG = "t9write lang-all"

PACKAGECONFIG ?= "lang-all lipi-toolkit"
PACKAGECONFIG[hunspell] = ",CONFIG+=disable-hunspell,hunspell"
PACKAGECONFIG[t9write] = "CONFIG+=t9write"
PACKAGECONFIG[lipi-toolkit] = "CONFIG+=lipi-toolkit"
PACKAGECONFIG[lang-all] = "CONFIG+=lang-all"
PACKAGECONFIG[lang-ar_AR] = "CONFIG+=lang-ar_AR"
PACKAGECONFIG[lang-da_DK] = "CONFIG+=lang-da_DK"
PACKAGECONFIG[lang-de_DE] = "CONFIG+=lang-de_DE"
PACKAGECONFIG[lang-en_GB] = "CONFIG+=lang-en_GB"
PACKAGECONFIG[lang-es_ES] = "CONFIG+=lang-es_ES"
PACKAGECONFIG[lang-fa_FA] = "CONFIG+=lang-fa_FA"
PACKAGECONFIG[lang-fi_FI] = "CONFIG+=lang-fi_FI"
PACKAGECONFIG[lang-fr_FR] = "CONFIG+=lang-fr_FR"
PACKAGECONFIG[lang-hi_IN] = "CONFIG+=lang-hi_IN"
PACKAGECONFIG[lang-it_IT] = "CONFIG+=lang-it_IT"
PACKAGECONFIG[lang-ja_JP] = "CONFIG+=lang-ja_JP"
PACKAGECONFIG[lang-ko_KR] = "CONFIG+=lang-ko_KR"
PACKAGECONFIG[lang-nb_NO] = "CONFIG+=lang-nb_NO"
PACKAGECONFIG[lang-pl_PL] = "CONFIG+=lang-pl_PL"
PACKAGECONFIG[lang-pt_PT] = "CONFIG+=lang-pt_PT"
PACKAGECONFIG[lang-ru_RU] = "CONFIG+=lang-ru_RU"
PACKAGECONFIG[lang-sv_SE] = "CONFIG+=lang-sv_SE"
PACKAGECONFIG[lang-zh_CN] = "CONFIG+=lang-zh_CN"
PACKAGECONFIG[lang-zh_TW] = "CONFIG+=lang-zh_TW"

EXTRA_QMAKEVARS_PRE += "${PACKAGECONFIG_CONFARGS}"
EXTRA_QMAKEVARS_PRE += "${@bb.utils.contains('DISTRO_FEATURES', 'x11', '', 'CONFIG+=disable-desktop', d)}"
EXTRA_QMAKEVARS_PRE += "CONFIG+=lang-all"

PACKAGES += "${PN}-dictionaries"
RRECOMMENDS_${PN} += "${PN}-dictionaries"
FILES_${PN}-dictionaries = "${OE_QMAKE_PATH_DATA}/qtvirtualkeyboard/*/*.dat"
FILES_${PN} += "${OE_QMAKE_PATH_DATA}/qtvirtualkeyboard/lipi_toolkit"

DEPENDS += "qtbase qtdeclarative qtmultimedia qtquickcontrols qtsvg qtxmlpatterns"

SRCREV = "master" 
