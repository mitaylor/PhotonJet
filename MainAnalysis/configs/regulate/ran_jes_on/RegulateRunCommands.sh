#!/bin/bash

types=`find . -type f -name "*.conf" -exec basename {} \;  | sed "s/\.conf//"`

for type in $types
	do
		echo "nohup ./bin/regulate configs/regulate/ran_jes_on/${type}.conf /mnt/submit-hi1/d00/higroup/mitay/photons/${type}.root > log/${type}_jes_on_log.txt 2>&1 &"
	done