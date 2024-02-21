#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/compare_errors configs/compare_errors/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/compare_errors_${tag}.root
}

samples=(pp)
types=(bayes svd)
errors=(knoerror kerrors kcovariance kcovtoys kerrorstoys)

for sample in ${samples[@]}; do
    for types in ${samples[@]}; do
        for errors in ${samples[@]}; do
            run_pp ${sample}
        done
    done
done