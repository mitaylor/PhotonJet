#!/usr/bin/env bash

set=$1
n=$2

run_aa() {
    tag=$1

    ./RunPopulateOriginalNominalMC.sh ${set} ${tag} ${n}
}

samples=(aa_qcd_no_jes aa_qcd aa_no_rho)

for sample in ${samples[@]}; do
    run_aa populate_${sample}_raw
    run_aa populate_${sample}_bkg
done
