#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/compare_unfolding_prior configs/compare_unfolding_prior/compare_unfolding_prior_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/compare_unfolding_prior_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/compare_unfolding_prior configs/compare_unfolding_prior/compare_unfolding_prior_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_unfolding_prior_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    for input in pyquen_pp pyquen_aa pyquen_nowide_aa jewel_pp jewel_aa jewel_norecoil_aa; do
        for algorithm in svd bayes; do
            run_pp ${sample}_${algorithm}_${input}
        done
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    for input in pyquen_pp pyquen_aa pyquen_nowide_aa jewel_pp jewel_aa jewel_norecoil_aa; do
        for algorithm in svd bayes; do
            run_aa ${sample}_${algorithm}_${input}
        done
    done
done