insmod ./bin/pru_speak.ko
#echo BB-PRUSPEAK > /sys/devices/bone_capemgr.9/slots --this dts has a prblem with PWM if I'm not wrong
echo BB-BONE-PRU-07 > /sys/devices/bone_capemgr.9/slots
python src/userspace_lib/bs_fifo.py > ~/.pruspeak.log &
