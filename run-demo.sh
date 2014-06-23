config-pin overlay cape-universaln
rmmod uio_pruss
cd src/driver && make load && cd ../..
cd src/dts && make install && cd ../..
cd src/pru-firmware && make install && cd ../..
config-pin overlay BB-PRUSPEAK
config-pin p9.92 pruout

