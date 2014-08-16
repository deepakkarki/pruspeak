insmod ./bin/pru_speak.ko
echo BB-PRUSPEAK > /sys/devices/bone_capemgr.9/slots
python src/userspace_lib/bs_fifo.py > ~/.pruspeak.log &
