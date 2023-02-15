#!/usr/bin/env bash

ls /data/submit/mitay/photons/condor/regulate_aa_mb_data_???.root > files.txt
# ls /data/submit/mitay/photons/condor/regulate_aa_mb_hydjet_??.root > files.txt
# ls /data/submit/mitay/photons/condor/regulate_aa_mb_hydjet_?.root > files.txt

awk -F "[_.]" '{print $(NF-1)}' files.txt > index.txt

files=($(cat files.txt))
index=($(cat index.txt))

for i in ${!files[@]}; do
    ../bin/pf_sum ${files[i]} /data/submit/mitay/photons/condor/regulate_aa_mb_data_sum_${index[i]}.root
done

rm files.txt
rm index.txt