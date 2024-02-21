#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/closure configs/closure/closure_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/quantitate_${tag}.root
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