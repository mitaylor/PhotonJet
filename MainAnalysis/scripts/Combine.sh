#!/usr/bin/env bash

histogram() {
    tag=$1

    echo "./bin/combine_populate configs/combine_populate/combine_${tag}.conf data/preapproval/populate_${tag}.root > log/combine_populate_${tag}_log.txt"
    
    ./bin/combine_populate configs/combine_populate/combine_${tag}.conf data/preapproval/populate_${tag}.root > log/combine_populate_${tag}_log.txt
}

samples=(pp_raw pp_bkg pp_qcd_raw, pp_qcd_bkg, pp_qcd_gen_iso_raw, pp_qcd_gen_iso_bkg, pp_wo_ele_rej_raw, pp_wo_ele_rej_bkg)

for sample in ${samples[@]}; do
    histogram ${sample}
done