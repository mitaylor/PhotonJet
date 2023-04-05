#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo "nohup ./bin/vacillate_dr configs/vacillate_dr/preapproval/vacillate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/vacillate_${tag}.root > log/${set}_vacillate_${tag}.txt 2>&1 &"
}

run_aa() {
    tag=$1

    echo "nohup ./bin/vacillate_dr configs/vacillate_dr/preapproval/vacillate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/vacillate_${tag}.root > log/${set}_vacillate_${tag}.txt 2>&1 &"
}

samples=(pp pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in qcd qcd_gen_iso jer_up jeu_up jeu_down wo_ele_rej; do
        run_pp ${sample}_${syst}
    done
done

samples=(pp)

for sample in ${samples[@]}; do
    for syst in front back; do
        run_pp ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}

    for syst in qcd qcd_gen_iso jer_up jeu_up jeu_down wo_ele_rej cent_down cent_up; do
        run_aa ${sample}_${syst}
    done
done