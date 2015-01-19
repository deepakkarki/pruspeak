#!/bin/sh
modprobe pru_speak
config-pin overlay BB-PRUSPEAK
bs_tcp_server.py > /var/log/pruspeak.log &
