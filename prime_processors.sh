#!/bin/bash

. colors.inc

S=${LBLUE}
R=${RESTORE}

echo $S"[+] Setting processor frequency scaling to \"performance\" governor..."$R
cpupower frequency-set -g performance > /dev/null

echo $S"[+] Disabling C-states..."$R
if [ ! -e "/tmp/myfifo" ]
then
    mkfifo /tmp/myfifo
    tail -f /tmp/myfifo > /dev/cpu_dma_latency &
    echo $! > /tmp/c_state_suppression_pid
    echo "0x00000000" > /tmp/myfifo
fi

# http://notepad2.blogspot.com/2014/11/a-script-to-turn-off-intel-cpu-turbo.html
echo $S"[+] Disabling TURBO Boost..."$R
for i in $(cat /proc/cpuinfo | grep processor | awk '{print $3}')
do
    wrmsr -p$i 0x1a0 0x4000850089
done

echo $S"[+] Disabling hyperthreading..."$R
echo 0 > /sys/devices/system/cpu/cpu1/online
echo 1 > /sys/devices/system/cpu/cpu2/online
echo 0 > /sys/devices/system/cpu/cpu3/online
echo 1 > /sys/devices/system/cpu/cpu4/online
echo 0 > /sys/devices/system/cpu/cpu5/online
echo 1 > /sys/devices/system/cpu/cpu6/online
echo 0 > /sys/devices/system/cpu/cpu7/online

echo $S"[+] Configuring core 3 (processor 6) as dedicated RT processor..."$R
isolcpus_output=$(./set_isolcpus.sh)
echo $isolcpus_output

if [[ $isolcpus_output = *"RESTART"* ]]
then
    echo $LYELLOW"WARNING : Until a reboot is done, core 3 isolation will only be approximate."$R
fi

if [ ! -d "/cpuset" ]
then
    # this section based on:
    # https://stackoverflow.com/questions/9072060/one-core-exclusively-for-my-process
    mkdir /cpuset
    mount -t cpuset none /cpuset/
    cd /cpuset
    
    echo $S"[+] Configuring unique cpuset for core 3..."$R
    mkdir sys                                   # create sub-cpuset for system processes
    /bin/echo 0,2,4 > sys/cpuset.cpus             # assign cpus (cores) 0-2 to this set
                                                # adjust if you have more/less cores
    /bin/echo 1 > sys/cpuset.cpu_exclusive
    /bin/echo 0 > sys/cpuset.mems     
    
    mkdir rt                                    # create sub-cpuset for my process
    /bin/echo 6 > rt/cpuset.cpus                # assign cpu (core) 3 to this cpuset
                                                # adjust this to number of cores-1
    /bin/echo 1 > rt/cpuset.cpu_exclusive
    /bin/echo 0 > rt/cpuset.mems
    /bin/echo 0 > rt/cpuset.sched_load_balance
    /bin/echo 1 > rt/cpuset.mem_hardwall
    if [[ $isolcpus_output = *"RESTART"* ]]
    then
        echo $S"[+] Attempting to move all tasks off of core 3..."$R
        for T in `cat tasks`
        do
            /bin/echo "$T" > sys/tasks 2> /dev/null
        done
        ps -aeF | awk '{ if ($7 == 6) print $2 }' | xargs -L1 taskset -pc 0 > /dev/null 2> /dev/null
    
        num_kernel_tasks=$(ps -aeF | awk '{ if ($7 == 6) print $0}' | grep "\[" | wc -l)
        num_nonkernel_tasks=$(ps -aeF | awk '{ if ($7 == 6) print $0}' | grep -v "\[" | wc -l)
        printf "${LRED}WARNING : There are still %d kernel and %d non-kernel tasks running on processor 6.${R}\n" $num_kernel_tasks $num_nonkernel_tasks
        echo   "${LRED}          This will affect timing precision."$R
    else
        num_kernel_tasks=$(ps -aeF | awk '{ if ($7 == 6) print $0}' | wc -l)
        printf "${LYELLOW}INFO : There are still %d kernel tasks running on processor 6.${R}\n" $num_kernel_tasks
        echo   "${LYELLOW}       This is the minimum required to operate the CPU core."$R
    fi
    cd - > /dev/null
fi

echo $S"[+] Setting up wbinvd kernel module..."$R
cd ./wbinvd_kernmod
./deploy.sh > /dev/null
cd ..

echo $S"[+] Disabling hardware prefetching on core 6..."$R
wrmsr -p 6 0x1a4 0xf

echo $WHITE"[+] Configuration complete. Processor 6 ready for timing measurements."$R




