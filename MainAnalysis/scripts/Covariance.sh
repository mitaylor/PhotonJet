#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/covariance configs/compare_unfolding_errors/compare_unfolding_errors_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/compare_unfolding_errors_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/covariance configs/compare_unfolding_errors/compare_unfolding_errors_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_unfolding_errors_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    for input in bayes_original; do
        run_pp ${sample}_${input}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    for input in bayes_original; do
        run_aa ${sample}_${input}
    done
done