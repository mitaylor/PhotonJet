#!/bin/bash

types=`find . -type f -name "*.conf" -exec basename {} \;  | sed "s/\.conf//"`

for type in $types
	do
		echo "nohup ./bin/regulate configs/regulate/ran/${type}.conf /mnt/submit-hi1/d00/higroup/mitay/photons/flowjets/${type}.root > log/${type}_flowjets_log.txt 2>&1 &"
	done