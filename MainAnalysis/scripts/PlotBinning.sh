#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1
    echo $tag

    ./bin/plot_binning configs/plot_binning/plot_binning_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/plot_binning_${tag}.root
}

run_aa() {
    tag=$1
    echo $tag

    ./bin/plot_binning configs/plot_binning/plot_binning_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/plot_binning_${tag}.root
}

samples=(pp_raw pp_qcd_raw)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa_raw aa_qcd_raw)

for sample in ${samples[@]}; do
    run_aa ${sample}
done