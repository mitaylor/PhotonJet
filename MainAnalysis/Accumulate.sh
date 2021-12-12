#!/usr/bin/env bash

arithmetic() {
    tag=$1

    ./bin/accumulate configs/accumulate/ran/accumulate_${tag}.conf \
        data/ran/accumulate_${tag}.root
}

nominal() {
    sample=$1

    arithmetic ${sample}
    arithmetic ${sample}_loose_purity
    arithmetic ${sample}_tight_purity
}

systematic() {
    sample=$1
    syst=$2

    arithmetic ${sample}_${syst}
}

samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    nominal $sample

    for syst in jeu_down jeu_up wo_ele_rej qcd; do #qcd_gen_iso
        systematic $sample $syst
    done
done
