#make driver
cd src/drivers
make
make install
cd ../..

#make pru_firmware
cd src/pru-firmware
make
make install
cd ../..

#make dts
cd src/dts
make
make install
cd ../..

#make userspace
cd src/userspace_lib
make
make install
cd ../..


