#!/bin/bash

. ../colors.inc

if [ -f instrtmr.ko ]; then
    make clean
fi

if ! lsmod | grep instrtmr > /dev/null
then
    echo $LYELLOW"WARNING: kernel module was not inserted, could not remove."$RESTORE
else
    rmmod instrtmr
    rm -f /dev/instrtmr
    rm -f /tmp/instrtmr_major_num
fi
echo "Done"

