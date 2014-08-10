echo SCRIPT > /dev/pruspeak
echo SET DIO[0], 1 > /dev/pruspeak
echo WAIT 1000 > /dev/pruspeak
echo SET DIO[0], 0 > /dev/pruspeak
echo WAIT 1000 > /dev/pruspeak
echo GOTO 0 > /dev/pruspeak
echo ENDSCRIPT > /dev/pruspeak
echo RUN > /dev/pruspeak

