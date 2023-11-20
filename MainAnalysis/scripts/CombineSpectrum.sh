#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1
    echo $tag

    ./bin/combine_spectrum configs/combine_spectrum/combine_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/spectrum_${tag}.root
}

run_aa() {
    tag=$1
    echo $tag

    ./bin/combine_spectrum configs/combine_spectrum/combine_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/spectrum_${tag}.root
}

samples=(pp_raw pp_qcd_raw pp_er_raw pp_qcd_er_raw)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa_raw aa_qcd_raw aa_no_er_raw aa_qcd_no_er_raw)

for sample in ${samples[@]}; do
    run_aa ${sample}
done