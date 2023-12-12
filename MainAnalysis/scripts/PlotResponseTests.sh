#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/plot_response_matrices configs/plot_response_matrices/plot_response_matrices_${tag}.conf configs/analysis/${set}/analysis_pp.conf
    ./bin/plot_response_matrices configs/plot_response_matrices/plot_response_matrices_original_${tag}.conf configs/analysis/${set}/analysis_pp.conf
}

run_aa() {
    tag=$1

    ./bin/plot_response_matrices configs/plot_response_matrices/plot_response_matrices_${tag}.conf configs/analysis/${set}/analysis_aa.conf
    ./bin/plot_response_matrices configs/plot_response_matrices/plot_response_matrices_original_${tag}.conf configs/analysis/${set}/analysis_aa.conf
}

samples=(aa)

for sample in ${samples[@]}; do
    for syst in qcd_no_jes qcd_gen_cut; do
        run_aa ${sample}_${syst}
    done
done
