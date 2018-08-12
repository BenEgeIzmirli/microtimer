#!/bin/bash

. ../colors.inc

if [ -f wbinvd.ko ]; then
    make clean
fi

if ! lsmod | grep wbinvd > /dev/null
then
    echo $LYELLOW"WARNING: kernel module was not inserted, could not remove."$RESTORE
else
    rmmod wbinvd
    rm -f /dev/wbinvd
    rm -f /tmp/wbinvd_major_num
fi
echo "Done"

