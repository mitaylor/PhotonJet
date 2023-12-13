#!/usr/bin/env bash

set=$1
tag=$2

ls /data/submit/mitay/photons/regulate_aa_qcd_weighted_* > files
awk -F "[_.]" '{print $(NF-1)}' files > index

mkdir -p /data/submit/mitay/mebs/${set}/
mkdir -p nohup/log/

files=($(cat files))
index=($(cat index))

rm files*
rm index*

for i in ${!files[@]}; do
    echo "std::vector<std::string> input = \\" > nohup/${tag}_${index[i]}.conf
    echo "${files[i]}" >> nohup/${tag}_${index[i]}.conf
    cat ../configs/populate/parallel/${tag}_template.conf >> nohup/${tag}_${index[i]}.conf

    nohup ../bin/populate_eta_scan nohup/${tag}_${index[i]}.conf ../configs/analysis/${set}/analysis_aa.conf /data/submit/mitay/mebs/${set}/${tag}_eta_scan_${index[i]}.root  > nohup/log/${set}_${tag}_eta_scan_${index[i]}.txt 2>&1 &
done