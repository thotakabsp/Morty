# Copyright (C) 2012-2016 O.S. Systems Software LTDA.
# Copyright (C) 2013-2016 Martin Jansa <martin.jansa@gmail.com>

QT_MODULE ?= "${BPN}"
QT_MODULE_BRANCH ?= "5.7"
QT_MODULE_BRANCH_PARAM ?= "branch=${QT_MODULE_BRANCH}"

# each module needs to define valid SRCREV
SRC_URI = " \
    git:///opt/qt/qt-everywhere-enterprise-src-5.7.1/${QT_MODULE};branch=master;protocol=file \
"

S = "${WORKDIR}/git"

PV = "5.7.1"
