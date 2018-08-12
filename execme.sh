#!/bin/bash

. ./colors.inc

if [ "$#" -lt 2 ]; then
echo "Usage: execme.sh \"rax_start\" \"rax_end\" [\"reg=value\"] ..."
exit
fi

./prime_processors.sh
echo

#sleep 3

#echo "DEBUG: non-kernel processes still running on core 6:"
#ps -aeF | awk '{ if ($7 == 6) print $0}' | grep -v "\["

gcc cpuid.c

rax_start=$1
rax_end=$2
shift
shift

rm -f output.txt
touch output.txt

ct=0

{ time for ((rax=$rax_start;rax<=$rax_end;rax++))
do
    if (( $rax % 0x00100000 == 0 ))
    then
        printf "%-10x\n" $rax
    fi
    { ./do_on_processor_6.sh ./a.out 1 $rax ; } >> output.txt
    ct=$[$ct +1]
done ; } 2> /tmp/timing.txt

timing=$(grep -Ev "^$" /tmp/timing.txt)
rm /tmp/timing.txt

echo $LGREEN"Completed $ct trials. Timing statistics:"$RESTORE
echo $CYAN"$timing"$RESTORE

echo
./reset_processors.sh

echo
echo "  # cycles : # commands"
./stats.py


rm ./a.out
#rm output.txt







