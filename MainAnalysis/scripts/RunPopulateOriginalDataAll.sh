#!/usr/bin/env bash

set=$1
n=$2

run_aa() {
    tag=$1

    ./RunPopulateOriginalData.sh ${set} ${tag} ${n}
}

samples=(aa)

for sample in ${samples[@]}; do
    run_aa populate_${sample}_raw
    run_aa populate_${sample}_bkg

    # for syst in wo_ele_rej no_eff cent_up cent_down; do
    #     run_aa populate_${sample}_${syst}_raw
    #     run_aa populate_${sample}_${syst}_bkg
    # done
done
