#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1
    echo $tag

    ./bin/combine_binning configs/combine_binning/combine_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/binning_${tag}.root
}

run_aa() {
    tag=$1
    echo $tag

    ./bin/combine_binning configs/combine_binning/combine_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/binning_${tag}.root
}

samples=(pp_raw)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa_raw)

for sample in ${samples[@]}; do
    run_aa ${sample}
done