#!/usr/bin/env bash

# ls /data/submit/mitay/photons/condor/regulate_aa_mb_data_???.root > files.txt
# awk -F "[_.]" '{print $(NF-1)}' files.txt > index.txt

# files=($(cat files.txt))
# index=($(cat index.txt))

# for i in ${!files[@]}; do
#     ../bin/pf_sum ${files[i]} /data/submit/mitay/photons/condor/regulate_aa_mb_data_sum_${index[i]}.root
# done

# rm files.txt
# rm index.txt

ls /data/submit/mitay/photons/condor/regulate_aa_mb_hydjet_??.root > files1.txt

awk -F "[_.]" '{print $(NF-1)}' files1.txt > index1.txt

files1=($(cat files1.txt))
index1=($(cat index1.txt))

for i in ${!files1[@]}; do
    ../bin/pf_sum ${files1[i]} /data/submit/mitay/photons/condor/regulate_aa_mb_hydjet_${index1[i]}.root
done

rm files1.txt
rm index1.txt

# ls /data/submit/mitay/photons/condor/regulate_aa_mb_hydjet_?.root > files2.txt
# awk -F "[_.]" '{print $(NF-1)}' files2.txt > index2.txt

# files2=($(cat files2.txt))
# index2=($(cat index2.txt))

# for i in ${!files2[@]}; do
#     ../bin/pf_sum ${files2[i]} /data/submit/mitay/photons/condor/regulate_aa_mb_hydjet_${index2[i]}.root
# done

# rm files2.txt
# rm index2.txt