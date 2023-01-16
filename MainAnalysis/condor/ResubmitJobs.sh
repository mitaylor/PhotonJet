#!/usr/bin/env bash


folder=${1}

cd ${folder}

numbers=`grep -rl "FATAL" . | awk -F "." '{print $3}'`
rm ${folder}.list

for number in ${numbers}; do
    echo "${number}, ${folder}_${number}.conf" >> ${folder}.list
done

condor_submit SubmitCondor_${folder}.condor