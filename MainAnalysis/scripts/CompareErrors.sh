#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/compare_errors_nolabels configs/compare_errors/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/compare_errors_${tag}.root
}

samples=(pp)
types=(bayes svd)
errors=(knoerror kerrors kcovariance kcovtoys kerrorstoys)

for sample in ${samples[@]}; do
    for type in ${types[@]}; do
        for error in ${errors[@]}; do
            run_pp ${sample}_${type}_${error}
        done
    done
done

run_aa() {
    tag=$1

    ./bin/compare_errors_nolabels configs/compare_errors/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_errors_${tag}.root
}

samples=(aa)
types=(bayes svd)
errors=(knoerror kerrors kcovariance kcovtoys kerrorstoys)

for sample in ${samples[@]}; do
    for type in ${types[@]}; do
        for error in ${errors[@]}; do
            run_aa ${sample}_${type}_${error}
        done
    done
done