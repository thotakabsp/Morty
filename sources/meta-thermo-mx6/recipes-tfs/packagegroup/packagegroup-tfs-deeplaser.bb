DESCRIPTION = "Deeplaser cloud connectivity modules"
LICENSE = "CLOSED"

inherit packagegroup

PACKAGEGROUP_DISABLE_COMPLEMENTARY = "1"

RDEPENDS_${PN} += " \
		python-json \
		python-argparse \
		python-subprocess \
		python-misc 	\
		python-pkgutil \
		python-netclient \
		python-netserver \
		python-importlib \
		python-db 	\
		python-debugger \
		python-email 	\
		python-ctypes 	\
		python-numbers 	\
		python-html 	\
		python-io 	\
		python-paho-mqtt \
		python-pip 	\
		python-crypt 	\
		python-pyserial \
		python-dbus	\
		python-smbus	\
		smbus2		\
		hostapd		\
		dhcpcd		\
		lighttpd 	\
		geoclue		\
		geoip		\
		ca-certificates \
		deeplaser 	\
"
