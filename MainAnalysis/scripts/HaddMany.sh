#!/usr/bin/env bash

dataset=$1
n=$2

ls /data/submit/mitay/photons/condor/${dataset}* > ${dataset}
split -n l/${n} --numeric-suffixes=0 -a 3 ${dataset} ${dataset}_

files=($(ls ${dataset}_*))

# for i in 1; do
for i in ${!files[@]}; do
    list=$(cat ${files[i]})
    hadd /data/submit/mitay/photons/${dataset}_$i.root $list
done

# rm ${dataset}*