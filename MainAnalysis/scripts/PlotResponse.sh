#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/plot_response_matrices configs/plot_response_matrices/plot_response_matrices_${tag}.conf configs/analysis/${set}/analysis_pp.conf
}

run_aa() {
    tag=$1

    ./bin/plot_response_matrices configs/plot_response_matrices/plot_response_matrices_${tag}.conf configs/analysis/${set}/analysis_aa.conf
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp $sample

    for syst in wo_ele_rej qcd qcd_gen_iso jer_up jeu_up jeu_down loose_purity tight_purity; do
        run_pp ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa $sample

    for syst in wo_ele_rej qcd qcd_gen_iso jer_up jeu_up jeu_down loose_purity tight_purity cent_up cent_down; do
        run_aa ${sample}_${syst}
    done
done
