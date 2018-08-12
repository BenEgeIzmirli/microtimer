#!/bin/bash

. ../colors.inc

if [ ! -f instrtmr.ko ]; then
    make all > /dev/null
fi

if lsmod | grep instrtmr > /dev/null
then
    echo $LYELLOW"WARNING: kernel module already inserted."$RESTORE
else
    dmesg -C
    insmod instrtmr.ko
    major=$(dmesg | grep "instrtmr module loaded with device major number" | grep -oE "[^ ]+$")

    if [ -e /dev/instrtmr ]
    then
        rm /dev/instrtmr
    fi
    mknod /dev/instrtmr c $major 0
    echo $major > /tmp/instrtmr_major_num
fi
echo "Done"

