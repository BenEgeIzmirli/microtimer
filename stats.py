#!/usr/bin/python3

counts = {}

with open("output.txt","r") as f:
    for line in f.readlines():
        if ":" not in line:
            continue
        l = line.split()
        rax_val = l[0]
        cycles = l[2]
        if int(cycles) not in counts.keys():
            counts[int(cycles)] = 1
        else:
            counts[int(cycles)] += 1

for k in sorted(counts.keys()):
    print("{:>10} : {}".format(k,counts[k]))
