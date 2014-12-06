#!/bin/bash

for host in `cat $1`; do
    ssh $host hostname
done
