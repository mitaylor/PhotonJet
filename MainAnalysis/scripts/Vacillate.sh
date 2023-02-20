#!/usr/bin/env bash

histogram() {
    tag=$1

    echo "nohup ./bin/vacillate_dr configs/vacillate_dr/preapproval/vacillate_${tag}.conf data/preapproval/vacillate_${tag}.root > log/vacillate_${tag}_log.txt 2>&1 &"
}

samples=(pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)
# samples=(pp)

for sample in ${samples[@]}; do
    histogram ${sample}

    for syst in qcd qcd_gen_iso jer_up jeu_up jeu_down wo_ele_rej; do
        histogram ${sample}_${syst}
    done
done

# samples=(pp)

# for sample in ${samples[@]}; do
#     for syst in front back; do
#         histogram ${sample}_${syst}
#     done
# done

# samples=(aa)

# for sample in ${samples[@]}; do
#     for syst in cent_down cent_up; do
#         histogram ${sample}_${syst}
#     done
# done