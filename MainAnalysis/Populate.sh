#!/usr/bin/env bash

histogram() {
    tag=$1

    ./bin/populate configs/populate/populate_${tag}_raw.conf \
        data/populate_${tag}_raw.root &
    ./bin/populate configs/populate/populate_${tag}_bkg.conf \
        data/populate_${tag}_bkg.root &
}

nominal() {
    sample=$1

    histogram ${sample}
}

systematic() {
    sample=$1
    syst=$2

    histogram ${sample}_${syst}
}

samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    nominal $sample

    for syst in jeu_down jeu_up wo_ele_rej qcd qcd_gen_iso; do
        systematic $sample $syst
    done
done
