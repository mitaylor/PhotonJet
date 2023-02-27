#!/usr/bin/env bash

dataset=$1

files=$(ls /data/submit/mitay/photons/condor/230227/${dataset}*)

hadd /data/submit/mitay/photons/${dataset}.root $files

# n=$(ls /data/submit/mitay/photons/condor/${dataset}* | wc -l)

# limit=`expr $n / 2`

# for ((i=0 ; i<$limit ; i++)); 
# do
#     index_1=`expr $i \* 2`
#     index_2=`expr $i \* 2 + 1`
#     echo "${files[index_1]} ${files[index_2]}"
#     echo hadd /data/submit/mitay/photons/${dataset}_$i.root ${files[index_1]} ${files[index_2]}
#     hadd /data/submit/mitay/photons/${dataset}_$i.root ${files[index_1]} ${files[index_2]}
# done

# if [ `expr $n % 2` == 0 ]
#     index=`expr $n - 1`
#     cp ${files[index]} /data/submit/mitay/photons/${dataset}_$limit.root
# fi