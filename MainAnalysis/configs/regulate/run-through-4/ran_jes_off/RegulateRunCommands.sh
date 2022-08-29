#!/bin/bash

types=`find . -type f -name "*.conf" -exec basename {} \;  | sed "s/\.conf//"`

for type in $types
	do
		echo "nohup ./bin/regulate configs/regulate/ran_jes_off/${type}.conf /mnt/submit-hi1/d00/higroup/mitay/photons/jes_off/${type}.root > log/${type}_jes_off_log.txt 2>&1 &"
	done