SUMMARY = "Install smbus2 package to rootfs"
HOMEPAGE= " https://github.com/kplindegaard/smbus2"
LICENSE = "CLOSED"
#LIC_FILES_CHKSUM = "file://LICENSE;md5=40aef6c14627be3eab15ce0fe51d5d0f"
DEPENDS = "python python-pip"

SRC_URI = "file://smbus2-0.2.0.tar.gz"
#https://pypi.python.org/packages/97/00/47ed0ae68da93e1186fd45dbed1102469eef490dc20871ab537b69b8bcb7/smbus2-0.2.0.tar.gz#md5=40aef6c14627be3eab15ce0fe51d5d0f

S = "${WORKDIR}/smbus2-0.2.0"

inherit setuptools
CLEANBROKEN = "1"
