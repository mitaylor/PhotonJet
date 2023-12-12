#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1
    echo $tag

    ./bin/combine_populate configs/combine_populate/combine_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/populate_${tag}.root
}

run_aa() {
    tag=$1
    echo $tag

    ./bin/combine_populate configs/combine_populate/combine_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/populate_${tag}.root
}


samples=(aa_qcd_nominal_raw aa_qcd_nominal_bkg aa_qcd_nominal_no_jes_raw aa_qcd_nominal_no_jes_bkg aa_qcd_nominal_no_rho_raw aa_qcd_nominal_no_rho_bkg)

for sample in ${samples[@]}; do
    run_aa ${sample}
done