(cd src/driver; make load)
(cd src/dts && make install)
(cd src/pru-firmware && make install)
config-pin overlay BB-PRUSPEAK
echo 1 > /sys/devices/ocp.*/4a300000.prurproc/reset
echo pru_firmware > /sys/devices/ocp.*/4a300000.prurproc/load
config-pin p9.92 pruout
python src/userspace_lib/client.py
