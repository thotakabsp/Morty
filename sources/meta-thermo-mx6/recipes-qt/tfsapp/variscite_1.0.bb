SUMMARY = "Qt5 Thermofisher application"
DESCRIPTION = "Qt5.5.1 Thermofisher application with QML/Database."
LICENSE = "CLOSED"

SRC_URI = "file://Variscite.tar.gz	\
	file://NotoSansCJK-Regular.ttc	\
	file://check.service	\
	file://first.service	\
	 "

S = "${WORKDIR}/Variscite/"

PR = "r1"

inherit systemd

SYSTEMD_SERVICE_${PN} = " check.service first.service "
SYSTEMD_AUTO_ENABLE = "enable"

DEPENDS = "qtbase qtdeclarative qtgraphicaleffects qtserialport qtconnectivity qtquickcontrols qtquickcontrols2 qtmultimedia qtvirtualkeyboard qtcharts"
RDEPENDS_${PN} = "qtdeclarative-qmlplugins qtgraphicaleffects-qmlplugins"

require recipes-qt/qt5/qt5.inc


OE_QMAKE_CXXFLAGS+="-DUI_BOARD "

do_install() {
	install -d ${D}/usr/share/fonts
	install -d ${D}/usr/share/fonts/opentype
	install -d ${D}/usr/share/fonts/opentype/noto
	install -d ${D}${systemd_unitdir}/system
	install -m 0755 ${WORKDIR}/NotoSansCJK-Regular.ttc ${D}/usr/share/fonts/opentype/noto/
	install -m 0644 ${WORKDIR}/check.service ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/first.service ${D}${systemd_unitdir}/system
	install -d ${D}/home/root/QML
	install -m 0755 ${B}/Variscite ${D}/home/root/
	rsync -a ${B}/QML/* ${D}/home/root/QML/    
        sed -i -e 's,@LIBEXECDIR@,${libexecdir},g' \
               -e 's,@BASE_BINDIR@,${base_bindir},g' \
                   ${D}${systemd_unitdir}/system/*.service

}

FILES_${PN} = " \
		/home/root/* \
		/home/root/Variscite \
	        /usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc \
	        ${systemd_unitdir}/system/check.service \
	        ${systemd_unitdir}/system/first.service \
		"
