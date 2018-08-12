#!/bin/bash

if [ "$#" -lt 2 ]; then
echo "Usage: execme.sh \"rax_start\" \"rax_end\" [\"reg=value\"] ..."
exit
fi

./prime_processors.sh
echo

gcc -m64 cpuid.c

rax_start=$1
rax_end=$2
shift
shift

rm -f output.txt

touch output.txt

for ((rax=$rax_start;rax<=$rax_end;rax++))
do
    ./do_on_processor_6.sh "./a.out 3 $rax 2 1" | tee -a output.txt
done

echo
./reset_processors.sh

./stats.py








