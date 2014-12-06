#!/bin/bash

for file in *.out; do
    cp $file $file.bak
    sed -n -e 's_Process 0:: Runtime: \(.*\) Waittime: \(.*\)$_M \1 \2_p' \
         -e 's_.*Runtime: \(.*\) Waittime: \(.*\)_P \1 \2_p' -i $file
done
