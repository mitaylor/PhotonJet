#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo "nohup ./bin/vacillate_weight_dr configs/vacillate_dr/preapproval/vacillate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/vacillate_weight_${tag}.root > log/${set}_vacillate_weight_${tag}.txt 2>&1 &"
}

run_aa() {
    tag=$1

    echo "nohup ./bin/vacillate_weight_dr configs/vacillate_dr/preapproval/vacillate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/vacillate_weight_${tag}.root > log/${set}_vacillate_weight_${tag}.txt 2>&1 &"
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in qcd qcd_gen_iso jer_up jeu_up jeu_down wo_ele_rej loose_purity tight_purity; do
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

    for syst in qcd qcd_gen_iso jer_up jeu_up jeu_down wo_ele_rej cent_down cent_up loose_purity tight_purity small; do
        run_aa ${sample}_${syst}
    done
done