#!/usr/bin/env bash

set=$1
tag=$2
n=$3

ls /data/submit/mitay/photons/regulate_aa_qcd_* > files
split -n l/8 --numeric-suffixes=0 -a 1 files files_
awk -F "[/.]" '{print $(NF-1)}' files_${n} > index_${n}

files=($(cat files_${n}))
index=($(cat index_${n}))

rm files*
rm index*

for i in ${!files[@]}; do
    echo "std::vector<std::string> input = \\" > nohup/${tag}_${index[i]}.conf
    echo "${files[i]}" >> nohup/${tag}_${index[i]}.conf
    cat ../configs/populate/parallel/${tag}_template.conf >> nohup/${tag}_${index[i]}.conf

    nohup ../bin/populate_original nohup/${tag}_${index[i]}.conf ../configs/analysis/${set}/analysis_aa.conf /data/submit/mitay/populate/arc/${set}/${tag}_original_${index[i]}.root  > nohup/log/${set}_${tag}_original_${index[i]}.txt 2>&1 &
done