#!/bin/bash

. colors.inc

S=$LBLUE
R=$RESTORE

echo $S"[+] Undoing processor isolation..."$R
isolcpus_output=$(./unset_isolcpus.sh nofixgrub)

if [[ $isolcpus_output != *"RESTART"* ]]; then
    echo $LYELLOW"[+] Undoing cpusets..."$R
else
    echo $LYELLOW"WARNING : Kernel is currently configured to ignore core 3. This will be persistent"$R
    echo $LYELLOW"          across reboots. To fix, run unset_isolcpus.sh."$R
fi

if [ -d "/cpuset" ]
then
    # move tasks back from sys-cpuset to root cpuset
    for T in `cat /cpuset/sys/tasks`
    do
        /bin/echo $T > /cpuset/tasks 2> /dev/null
    done
    # remove sys-cpuset
    rmdir /cpuset/sys
    # move tasks back from rt-cpuset to root cpuset
    for T in `cat /cpuset/rt/tasks`; do echo "Moving " $T; /bin/echo $T > /cpuset/tasks; done
    # remove rt-cpuset
    rmdir /cpuset/rt
    # unmount and remove /cpuset
    umount /cpuset
    rmdir /cpuset
fi

echo $S"[+] Re-enabling hyperthreading..."$R
echo 1 > /sys/devices/system/cpu/cpu1/online
echo 1 > /sys/devices/system/cpu/cpu2/online
echo 1 > /sys/devices/system/cpu/cpu3/online
echo 1 > /sys/devices/system/cpu/cpu4/online
echo 1 > /sys/devices/system/cpu/cpu5/online
echo 1 > /sys/devices/system/cpu/cpu6/online
echo 1 > /sys/devices/system/cpu/cpu7/online

echo $S"[+] Enabling TURBO Boost..."$R
for i in $(cat /proc/cpuinfo | grep processor | awk '{print $3}')
do
    wrmsr -p$i 0x1a0 0x850089
done

echo $S"[+] Resetting C-states to kernel defaults..."$R
if [ -e "/tmp/c_state_suppression_pid" ]
then
    cat /tmp/c_state_suppression_pid | xargs kill -9
    rm /tmp/c_state_suppression_pid
    rm /tmp/myfifo
fi

echo $S"[+] Resetting frequency scaling to \"powersave\" governor..."$R
cpupower frequency-set -g powersave > /dev/null

echo $S"[+] Unloading wbinvd kernel module..."$R
if [ -e "/dev/wbinvd" ]
then
    cd ./wbinvd_kernmod
    ./undeploy.sh > /dev/null
    cd ..
fi

echo $S"[+] Enabling hardware prefetching..."$R
wrmsr -p 6 0x1a4 0x0

echo $WHITE"[+] Reset complete. Processor state should be restored to normal."$R












