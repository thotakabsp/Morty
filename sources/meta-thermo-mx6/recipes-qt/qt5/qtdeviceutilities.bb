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

DESCRIPTION = "Qt Device Utilities"
LICENSE = "GPL-3.0 | The-Qt-Company-DCLA-2.1"
LIC_FILES_CHKSUM = "file://LICENSE.GPL3;md5=d32239bcb673463ab874e80d47fae504"

inherit qt5-module
require recipes-qt/qt5/qt5-git.inc

PACKAGECONFIG ??= ""
PACKAGECONFIG[wpasupplicant] = "CONFIG+=wpasupplicant,,wpa-supplicant"

EXTRA_QMAKEVARS_PRE += "${PACKAGECONFIG_CONFARGS}"

SRC_URI = " \
    git://codereview.qt-project.org/qt/qtdeviceutilities;${QT_MODULE_BRANCH_PARAM};protocol=http \
    "
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://0001-added-remove-to-wifi.patch \
    "
SRCREV = "48fb704e64300387f99c4192436728858c8aa4f8"

DEPENDS = "qtbase qtdeclarative qtconnectivity"
