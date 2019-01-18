SUMMARY = "Create time zone information for All major cities"
LICENSE = "CLOSED"
DEPENDS = "tzdata"

SRC_URI = "file://tfstzdata-0.1.tar.gz"

do_install(){
	cd ${WORKDIR}
	python init.py ./*.csv
}

