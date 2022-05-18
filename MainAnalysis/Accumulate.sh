#!/usr/bin/env bash

arithmetic() {
    tag=$1

    ./bin/accumulate configs/accumulate/new/jet_20/accumulate_${tag}.conf \
        data/jet_20/accumulate_${tag}.root
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

# samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)
samples=(pp aa)

for sample in ${samples[@]}; do
    nominal $sample

    for syst in wo_ele_rej qcd qcd_gen_iso; do
        systematic $sample $syst
    done
done

samples=(a)

for sample in ${samples[@]}; do
    for syst in cent_up cent_down; do
        systematic $sample $syst
    done
done
