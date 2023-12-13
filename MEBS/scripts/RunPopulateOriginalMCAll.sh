#!/usr/bin/env bash

set=$1

run_aa() {
    tag=$1

    ./RunPopulateOriginalMC.sh ${set} ${tag}
}

samples=(aa_qcd)

for sample in ${samples[@]}; do
    run_aa populate_${sample}_raw
done
