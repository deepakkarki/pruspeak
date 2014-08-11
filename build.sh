#make driver
cd src/drivers
make
cp pru_speak.ko ../../bin/
make clean
cd ../..

#make pru_firmware
cd src/pru-firmware
make
cp pru0_firmware ../../bin/
cp pru1_firmware ../../bin/
cd ../..

#make dts
cd src/dts
make
cp BB-PRUSPEAK-00A0.dtbo ../../bin/
cd ../..


