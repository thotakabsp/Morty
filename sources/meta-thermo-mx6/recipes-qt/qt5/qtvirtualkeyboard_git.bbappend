############################################################################
##
## Copyright (C) 2016 The Qt Company Ltd.
## Contact: https://www.qt.io/licensing/
##
## This file is part of the Boot to Qt meta layer.
##
## $QT_BEGIN_LICENSE:GPL$
## Commercial License Usage
## Licensees holding valid commercial Qt licenses may use this file in
## accordance with the commercial license agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and The Qt Company. For licensing terms
## and conditions see https://www.qt.io/terms-conditions. For further
## information use the contact form at https://www.qt.io/contact-us.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3 or (at your option) any later version
## approved by the KDE Free Qt Foundation. The licenses are as published by
## the Free Software Foundation and appearing in the file LICENSE.GPL3
## included in the packaging of this file. Please review the following
## information to ensure the GNU General Public License requirements will
## be met: https://www.gnu.org/licenses/gpl-3.0.html.
##
## $QT_END_LICENSE$
##
############################################################################

#inherit qtquickcompiler

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://0001-Thermo-keyboard-layout-patch.patch \
    file://0002-Added-support-for-Asian-Languages.patch \
    file://0003-Fixed-issue-with-enterprise-keyword.patch \
	file://KeyBoard_NextIcon.png \
	file://backspace-868482.png 	\
	file://check-868482.png 	\
	file://enter-868482.png		\
	file://globe-868482.png 	\
	file://hidekeyboard-868482.png 	\
	file://search-868482.png 	\
	file://shift-80c342.png 	\
	file://shift-868482.png 	\
	file://shift-c5d6b6.png 	\
	file://backspace-c5a96f.png 	\
	file://check-c5a96f.png 	\
	file://enter-c5a96f.png 	\
	file://globe-110b05.png 	\
	file://hidekeyboard-c5a96f.png 	\
	file://search-c5a96f.png 	\
	file://shift-c5a96f.png 	\
	file://shift-cd8865.png 	\
	file://shift-dc4f28.png 	\
    "

do_configure_prepend() {
install -m 0644 ${WORKDIR}/KeyBoard_NextIcon.png	${S}/src/virtualkeyboard/content/styles/default/images/KeyBoard_NextIcon.png
install -m 0644 ${WORKDIR}/backspace-868482.png		${S}/src/virtualkeyboard/content/styles/default/images/backspace-868482.png
install -m 0644 ${WORKDIR}/check-868482.png		${S}/src/virtualkeyboard/content/styles/default/images/check-868482.png
install -m 0644 ${WORKDIR}/enter-868482.png		${S}/src/virtualkeyboard/content/styles/default/images/enter-868482.png
install -m 0644 ${WORKDIR}/globe-868482.png		${S}/src/virtualkeyboard/content/styles/default/images/globe-868482.png
install -m 0644 ${WORKDIR}/hidekeyboard-868482.png	${S}/src/virtualkeyboard/content/styles/default/images/hidekeyboard-868482.png
install -m 0644 ${WORKDIR}/search-868482.png		${S}/src/virtualkeyboard/content/styles/default/images/search-868482.png
install -m 0644 ${WORKDIR}/shift-80c342.png		${S}/src/virtualkeyboard/content/styles/default/images/shift-80c342.png
install -m 0644 ${WORKDIR}/shift-868482.png		${S}/src/virtualkeyboard/content/styles/default/images/shift-868482.png
install -m 0644 ${WORKDIR}/shift-c5d6b6.png	 	${S}/src/virtualkeyboard/content/styles/default/images/shift-c5d6b6.png
install -m 0644 ${WORKDIR}/backspace-c5a96f.png		${S}/src/virtualkeyboard/content/styles/retro/images/backspace-c5a96f.png
install -m 0644 ${WORKDIR}/check-c5a96f.png		${S}/src/virtualkeyboard/content/styles/retro/images/check-c5a96f.png
install -m 0644 ${WORKDIR}/enter-c5a96f.png		${S}/src/virtualkeyboard/content/styles/retro/images/enter-c5a96f.png
install -m 0644 ${WORKDIR}/globe-110b05.png		${S}/src/virtualkeyboard/content/styles/retro/images/globe-110b05.png
install -m 0644 ${WORKDIR}/hidekeyboard-c5a96f.png	${S}/src/virtualkeyboard/content/styles/retro/images/hidekeyboard-c5a96f.png
install -m 0644 ${WORKDIR}/search-c5a96f.png		${S}/src/virtualkeyboard/content/styles/retro/images/search-c5a96f.png
install -m 0644 ${WORKDIR}/shift-c5a96f.png		${S}/src/virtualkeyboard/content/styles/retro/images/shift-c5a96f.png
install -m 0644 ${WORKDIR}/shift-cd8865.png		${S}/src/virtualkeyboard/content/styles/retro/images/shift-cd8865.png
install -m 0644 ${WORKDIR}/shift-dc4f28.png 		${S}/src/virtualkeyboard/content/styles/retro/images/shift-dc4f28.png
}                                                       

PACKAGECONFIG = "lang-all"
