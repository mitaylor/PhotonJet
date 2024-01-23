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

samples=(pp_raw pp_bkg pp_qcd_raw pp_qcd_bkg)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa_raw aa_bkg aa_qcd_raw aa_qcd_bkg)

for sample in ${samples[@]}; do
    run_aa ${sample}
done

# samples=(aa_mebs_raw aa_mebs_bkg aa_qcd_mebs_raw aa_qcd_mebs_bkg)

# for sample in ${samples[@]}; do
#     run_aa ${sample}
# done