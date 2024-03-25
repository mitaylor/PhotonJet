#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/process_vacillate configs/process_vacillate/vacillate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/process_vacillate_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/process_vacillate configs/process_vacillate/vacillate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/process_vacillate_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in qcd qcd_gen_iso jer_up jeu_up jeu_down wo_ele_rej loose_purity tight_purity closure_a closure_b; do
        run_pp ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}

    for syst in qcd qcd_gen_iso jer_up jeu_up jeu_down wo_ele_rej cent_down cent_up loose_purity tight_purity small closure_a closure_b; do
        run_aa ${sample}_${syst}
    done
done