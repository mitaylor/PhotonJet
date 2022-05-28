#!/usr/bin/env bash

histogram() {
    tag=$1

    echo "nohup ./bin/vacillate_dr configs/vacillate_dr/new/jet_20_small/vacillate_${tag}.conf data/jet_20/vacillate_${tag}.root > log/vacillate_small_${tag}_jet_20_log.txt 2>&1 &"
}

# samples=(pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90 pp aa)
samples=(pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    histogram ${sample}

    for syst in wo_ele_rej jeu_up jeu_down jer_up qcd qcd_gen_iso; do
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