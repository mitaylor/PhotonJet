#!/usr/bin/env bash

set=$1
n=$2

run_aa() {
    tag=$1

    ./RunPopulateOriginalMC.sh ${set} ${tag} ${n}
}

samples=(aa_qcd)

for sample in ${samples[@]}; do
    for syst in no_er; do
        run_aa populate_${sample}_${syst}_raw
        run_aa populate_${sample}_${syst}_bkg
    done
done
