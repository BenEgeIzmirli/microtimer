#!/bin/bash

. ./colors.inc

nofixgrub=
if [ "$#" -eq 1 ]
then
    if [[ $1 = "nofixgrub" ]]
    then
        nofixgrub="true"
    fi
fi

if ! grep isolcpus /etc/default/grub > /dev/null
then
    echo "${LGREEN}GRUB is already configured to boot Linux kernel using all cores."
else
    if [ ! $nofixgrub ]
    then
        echo "${CYAN}Configuring GRUB setting for isolcpus. This may take a moment..."
        sed -i '/.*isolcpus.*/d' /etc/default/grub 2> /dev/null
        update-grub 2> /dev/null
        echo "${LGREEN}Success - Kernel task scheduler has been configured to use all processor cores."
    fi
fi
if grep 6 /sys/devices/system/cpu/isolated
then
    echo "${RED}#### A RESTART IS REQUIRED."
fi
