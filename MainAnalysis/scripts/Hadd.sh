#!/usr/bin/env bash

dataset=regulate_aa_qcd_extra_80_F

files=$(ls /data/submit/mitay/photons/condor/${dataset}*)
n=$(ls /data/submit/mitay/photons/condor/${dataset}* | wc -l)

if [ `expr $n % 2` == 0 ]
then
    limit=`expr $n / 2`

	for ((i=0 ; i<$limit ; i++)); 
    do
        index_1=`expr $i \* 2`
        index_2=`expr $i \* 2 + 1`
        hadd /data/submit/mitay/photons/${dataset}_$i.root ${files[index_1]} ${files[index_2]}
    done
else
    limit=`expr $n / 2`

    echo $limit

	for ((i=0 ; i<$limit ; i++)); 
    do
        index_1=`expr $i \* 2`
        index_2=`expr $i \* 2 + 1`

        echo "$index_1 $index_2"
        hadd /data/submit/mitay/photons/${dataset}_$i.root ${files[index_1]} ${files[index_2]}
    done

    index=`expr $n - 1`
    cp ${files[index]} /data/submit/mitay/photons/${dataset}_$limit.root
fi