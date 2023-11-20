#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/plot_hem configs/plot_hem/plot_hem_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/plot_hem_${tag}.root > log/${set}_plot_hem_${tag}.txt 2>&1 &
}

run_aa() {
    tag=$1

    ./bin/plot_hem configs/plot_hem/plot_hem_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/plot_hem_${tag}.root > log/${set}_plot_hem_${tag}.txt 2>&1 &
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa aa_no_correction)

for sample in ${samples[@]}; do
    run_aa ${sample}
done