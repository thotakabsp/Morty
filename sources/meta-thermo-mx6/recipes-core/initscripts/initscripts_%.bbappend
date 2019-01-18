FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append			= " file://watchdog \
                                    file://watchdog.conf \
				    file://ThermoApp	\
				    file://fw_env.config	\
				    file://tfs-update	\
				    file://tfs-recovery	\
				    file://banner.sh	\
				    file://.version	\
				    file://.miniver	\
				    file://bootmisc.sh	\
				    file://sysctl.conf \
				file://ppp/options	\
				file://ppp/ip-up	\
				file://ppp/options-mobile	\
				file://ppp/ip-up.d/08setupdns	\
				file://ppp/connect-errors	\
				file://ppp/ppp_on_boot	\
				file://ppp/ip-down	\
				file://ppp/ip-down.d/92removedns	\
				file://ppp/peers/mobile-noauth	\
				file://ppp/peers/mobile-auth	\
				file://ppp/peers/provider	\
				file://ppp/chatscripts/mode.NONE	\
				file://ppp/chatscripts/mode	\
				file://ppp/chatscripts/pin.NONE	\
				file://ppp/chatscripts/mobile-modem.chat	\
				file://ppp/chatscripts/apn.es.airtel	\
				file://ppp/chatscripts/mode.GPRS-only	\
				file://ppp/chatscripts/pin.CODE	\
				file://ppp/chatscripts/apn	\
				file://ppp/chatscripts/pin	\
				file://ppp/chatscripts/mode.GPRS-pref	\
				  "
SRC_URI_watchdog[md5sum] = "2cd429be6a17a60b4da5fb6aab3b1e19"


do_install_append() {
        update-rc.d -f watchdog remove
        update-rc.d -f watchdog.conf remove
	update-rc.d -f ThermoApp remove
	update-rc.d -f fw_env.config remove

	update-rc.d -f tfs-update remove
        update-rc.d -f banner.sh remove
        update-rc.d -f bootmisc.sh remove
        update-rc.d -f .version remove
        update-rc.d -f .miniver remove
	update-rc.d -f variscite-touch remove
	update-rc.d -f sysctl.conf remove

	install -m 0755	${WORKDIR}/watchdog ${D}${sysconfdir}/init.d
	install -m 0755	${WORKDIR}/watchdog.conf ${D}${sysconfdir}/
	install -m 0755	${WORKDIR}/banner.sh ${D}${sysconfdir}/init.d
	install -m 0755	${WORKDIR}/bootmisc.sh ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/fw_env.config ${D}${sysconfdir}/
	install -m 0755 ${WORKDIR}/sysctl.conf ${D}${sysconfdir}/
	update-rc.d -r ${D} watchdog defaults 1

	echo "<<<<<<<<<<<<< " ${BUILD_MINIRFS} 
	if [ ${BUILD_RECOVERY} -eq 1 ];
	then
		install -m 0755 ${WORKDIR}/tfs-recovery ${D}${sysconfdir}/init.d/
		update-rc.d -r ${D} tfs-recovery start 99 5 . stop 01 5 .
	elif [ ${BUILD_MINIRFS} -eq 1 ];
	then
		install -m 0755 ${WORKDIR}/.miniver ${D}${sysconfdir}/
		install -m 0755 ${WORKDIR}/tfs-update ${D}${sysconfdir}/init.d/
		update-rc.d -r ${D} tfs-update start 99 5 . stop 01 5 .
	else
		rm -rf ${D}${sysconfdir}/ppp
		install -d ${D}${sysconfdir}/ppp 
		rsync -a ${WORKDIR}/ppp/* ${D}${sysconfdir}/ppp/
		install -m 0755 ${WORKDIR}/.version ${D}${sysconfdir}/
		install -m 0755	${WORKDIR}/ThermoApp ${D}${sysconfdir}/init.d/
		update-rc.d -r ${D} ThermoApp start 99 4 5 . stop 01 4 5 .
	fi
}
