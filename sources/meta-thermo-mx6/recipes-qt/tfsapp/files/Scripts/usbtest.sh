#!/bin/sh
echo -n ci_hdrc.1 > /sys/bus/platform/drivers/ci_hdrc/unbind
sleep 5
echo -n ci_hdrc.1 > /sys/bus/platform/drivers/ci_hdrc/bind
