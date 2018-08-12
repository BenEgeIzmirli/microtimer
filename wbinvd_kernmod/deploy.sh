#!/bin/bash

. ../colors.inc

if [ ! -f wbinvd.ko ]; then
    make all > /dev/null
fi

if lsmod | grep wbinvd > /dev/null
then
    echo $LYELLOW"WARNING: kernel module already inserted."$RESTORE
else
    dmesg -C
    insmod wbinvd.ko
    major=$(dmesg | grep "wbinvd module loaded with device major number" | grep -oE "[^ ]+$")

    if [ -e /dev/wbinvd ]
    then
        rm /dev/wbinvd
    fi
    mknod /dev/wbinvd c $major 0
    echo $major > /tmp/wbinvd_major_num
fi
echo "Done"

