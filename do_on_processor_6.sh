#!/bin/bash

taskset --cpu-list 6 chrt -r 1 $@ &
pid=$!
if [ -e /cpuset ]
then
    echo $pid > /cpuset/rt/tasks
fi
wait $pid

