#!/usr/bin/env bash

arithmetic() {
    tag=$1

    ./bin/accumulate configs/accumulate/preapproval/accumulate_${tag}.conf \
        data/preapproval/accumulate_${tag}.root
}

nominal() {
    sample=$1

    arithmetic ${sample}
}

systematic() {
    sample=$1
    syst=$2

    arithmetic ${sample}_${syst}
}

# samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

# for sample in ${samples[@]}; do
#     nominal $sample

#     for syst in qcd qcd_gen_iso wo_ele_rej; do
#         systematic $sample $syst
#     done
# done

# samples=(aa)

# for sample in ${samples[@]}; do
#     for syst in cent_up cent_down; do
#         systematic $sample $syst
#     done
# done

samples=(aa_mebs aa_qcd_mebs)

for sample in ${samples[@]}; do
    nominal $sample
done