#!/bin/sh

inputs=`find "../configs/regulate/ran/" -type f -name "*.conf" -exec basename {} \;  | sed "s/\.conf//"`

for input in $inputs
	do
		cat SubmitRegulate_template.condor | sed "s#__INPUT__#${input}#" > SubmitRegulate_${input}.condor
        condor_submit SubmitRegulate_${input}.condor
	done