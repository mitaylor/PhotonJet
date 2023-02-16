#!/usr/bin/env bash

tag=$1
n=$2

ls /data/submit/mitay/photons/condor/regulate_aa_data_* > files
split -n l/7 --numeric-suffixes=0 -a 1 files files_
awk -F "[_.]" '{print $(NF-1)}' files_${n} > index_${n}

files=($(cat files_${n}))
index=($(cat index_${n}))

# rm files*
# rm index*

for i in ${!files[@]}; do
    echo "cat ../configs/populate/preapproval-parallel/${tag}_template.conf  | sed "s/__INPUT__/${files[i]}/g" > nohup/${tag}_${index[i]}.conf"
    echo ../bin/populate_test nohup/${tag}_${index[i]}.conf /data/submit/mitay/populate/${tag}_${index[i]}.root
done