SUMMARY = "Install deeplaser to tfs image"
LICENSE = "CLOSED"

#SRC_URI = "file://deeplaser/*	\
#"
SRC_URI = "file://deeplaser_1.9.4.tar.gz	\
"
	

#This package doesn't have any files for the rootfs in it, option needed to create an empty 
# package so when the rootfs image is made it finds the mksd_xxx.deb package and doesn't complain
FILES_${PN} = "/home/root		\
	       /home/root/deeplaser	\
	       /home/root/deeplaser/*	\
"

FILES_${PN}-dbg += "/home/root/deeplaser/.debug	\
			"

# Copy script to the deploy area with u-boot, uImage, and rootfs
do_install () {
	install -d ${D}/home/root/deeplaser
   	rsync -avz ${WORKDIR}/deeplaser/* ${D}/home/root/deeplaser/
   	rsync -avz ${WORKDIR}/deeplaser/.version ${D}/home/root/deeplaser/
	chmod 777 -R ${D}/home/root/deeplaser/*
}
