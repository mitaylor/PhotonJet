#!/usr/bin/env bash

set=$1
n=$2

run_aa() {
    tag=$1

    ./RunPopulateData.sh ${set} ${tag} ${n}
}

samples=(aa)

for sample in ${samples[@]}; do
    run_aa populate_${sample}_raw
    run_aa populate_${sample}_bkg
done
