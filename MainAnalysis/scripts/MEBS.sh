#!/usr/bin/env bash

n=$1

# ls /data/submit/mitay/photons/condor/regulate_aa_mb_data_${n}??.root > files.txt
# awk -F "[_.]" '{print $(NF-1)}' files.txt > index.txt

# files=($(cat files.txt))
# index=($(cat index.txt))

# rm files.txt
# rm index.txt

# for i in ${!files[@]}; do
#     bin/mebs_inputs ${files[i]} /data/submit/mitay/photons/condor/regulate_aa_mb_data_mebs_${index[i]}.root
# done

ls /data/submit/mitay/photons/regulate_aa_mb_hydjet.root > files.txt

awk -F "[_.]" '{print $(NF-1)}' files.txt > index.txt

files=($(cat files.txt))
index=($(cat index.txt))

for i in ${!files[@]}; do
    ../bin/mebs_inputs ${files[i]} /data/submit/mitay/photons/condor/regulate_aa_mb_hydjet_mebs_${index[i]}.root
done

rm files.txt
rm index.txt