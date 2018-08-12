#!/bin/bash

. colors.inc

nofixgrub=
if [ "$#" -eq 1 ]
then
    if [[ $1 = "nofixgrub" ]]
    then
        nofixgrub="true"
    fi
fi

R=${RESTORE}

if grep isolcpus /etc/default/grub > /dev/null
then
    echo "${LGREEN}GRUB is already configured to boot Linux kernel ignoring processors 6 and 7."$R
else
    if [ ! $nofixgrub ]
    then
        echo "${CYAN}Configuring GRUB setting for isolcpus. This may take a moment..."$R
        echo "GRUB_CMDLINE_LINUX_DEFAULT=\"quiet splash isolcpus=6,7\"" >> /etc/default/grub
        update-grub 2> /dev/null
        echo "${LGREEN}Success - Kernel task scheduler has been configured to ignore processors 6 and 7."$R
    fi
fi
if ! grep 6 /sys/devices/system/cpu/isolated > /dev/null
then
    echo "${RED}#### A RESTART IS REQUIRED."$R
fi
