#!/usr/bin/env bash

set=$1
n=$2

run_aa() {
    tag=$1

    ./RunPopulateOriginalData.sh ${set} ${tag} ${n}
}

samples=(aa)

for sample in ${samples[@]}; do
    for syst in es; do
        run_aa populate_${sample}_${syst}_raw
        run_aa populate_${sample}_${syst}_bkg
    done
done
