#!/bin/bash

types=`find . -type f -name "*.conf" -exec basename {} \;  | sed "s/\.conf//"`

for type in $types
	do
		echo "nohup ./bin/regulate configs/regulate/new/${type}.conf /mnt/submit03/d00/scratch/mitay/photons/${type}_jes_er.root > log/${type}_jes_er_log.txt 2>&1 &"
	done